#include "stdafx.h"
#include "constants.h"
#include "gm.h"
#include "messenger_manager.h"
#include "buffer_manager.h"
#include "desc_client.h"
#include "log.h"
#include "config.h"
#include "p2p.h"
#include "crc32.h"
#include "char.h"
#include "char_manager.h"
#include "questmanager.h"

MessengerManager::MessengerManager()
{
}

MessengerManager::~MessengerManager()
{
}

void MessengerManager::Initialize()
{
}

void MessengerManager::Destroy()
{
#if defined(ENABLE_MESSENGER_TEAM)
	m_vecTeamList.clear();
#endif
}

void MessengerManager::P2PLogin(MessengerManager::keyA account)
{
	Login(account);
}

void MessengerManager::P2PLogout(MessengerManager::keyA account)
{
	Logout(account);
}

void MessengerManager::Login(MessengerManager::keyA account)
{
	if (m_set_loginAccount.find(account) != m_set_loginAccount.end())
	{
		return;
	}

	char __account[CHARACTER_NAME_MAX_LEN * 2 + 1];

	DBManager::instance().EscapeString(__account, sizeof(__account), account.c_str(), account.size());

	if (account.compare(__account))
	{
		return;
	}

	DBManager::instance().FuncQuery([this](SQLMsg* msg) { LoadList(msg); },
			"SELECT account, companion FROM messenger_list%s WHERE account='%s'", get_table_postfix(), __account);

#if defined(ENABLE_MESSENGER_BLOCK)
	DBManager::instance().FuncQuery([this](SQLMsg* msg) { LoadBlockList(msg); },
			"SELECT account, companion FROM messenger_block_list%s WHERE account='%s'", get_table_postfix(), __account);
#endif

	m_set_loginAccount.insert(__account);
#if defined(ENABLE_MESSENGER_TEAM)
	SendTeamList(__account, false);
#endif
}

void MessengerManager::LoadList(SQLMsg * msg)
{
	if (!msg || !msg->Get() || msg->Get()->uiNumRows == 0)
	{
		return;
	}

	std::string account;

	sys_log(1, "Messenger::LoadList");

	for (uint32_t i = 0; i < msg->Get()->uiNumRows; ++i)
	{
		MYSQL_ROW row = mysql_fetch_row(msg->Get()->pSQLResult);

		if (row[0] && row[1])
		{
			if (account.length() == 0)
				account = row[0];

			m_Relation[row[0]].insert(row[1]);
			m_InverseRelation[row[1]].insert(row[0]);
		}
	}

	SendList(account);

	for (const auto& companion : m_InverseRelation[account])
		SendLogin(companion, account);
}

void MessengerManager::Logout(MessengerManager::keyA account)
{
	if (m_set_loginAccount.find(account) == m_set_loginAccount.end())
		return;

	m_set_loginAccount.erase(account);

	for (const auto& it : m_InverseRelation[account])
	{
		SendLogout(it, account);
	}

	m_Relation.erase(account);

#if defined(ENABLE_MESSENGER_TEAM)
	UpdateTeam(account);
#endif

#if defined(ENABLE_MESSENGER_BLOCK)
	for (const auto& it : m_InverseBlockRelation[account])
	{
		SendBlockLogout(it, account);
	}

	m_BlockRelation.erase(account);
#endif
}

void MessengerManager::RequestToAdd(LPCHARACTER ch, LPCHARACTER target)
{
	if (!ch->IsPC() || !target->IsPC())
		return;

	if (quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID())->IsRunning() == true)
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_INFO, 607, "");
#endif
		return;
	}

	if (quest::CQuestManager::instance().GetPCForce(target->GetPlayerID())->IsRunning() == true)
		return;

	DWORD dw1 = GetCRC32(ch->GetName(), strlen(ch->GetName()));
	DWORD dw2 = GetCRC32(target->GetName(), strlen(target->GetName()));

	char buf[64];
	snprintf(buf, sizeof(buf), "%u:%u", dw1, dw2);
	DWORD dwComplex = GetCRC32(buf, strlen(buf));

	m_set_requestToAdd.insert(dwComplex);

	target->ChatPacket(CHAT_TYPE_COMMAND, "messenger_auth %s", ch->GetName());
}

// @fixme130 void -> bool
bool MessengerManager::AuthToAdd(MessengerManager::keyA account, MessengerManager::keyA companion, bool bDeny)
{
	DWORD dw1 = GetCRC32(companion.c_str(), companion.length());
	DWORD dw2 = GetCRC32(account.c_str(), account.length());

	char buf[64];
	snprintf(buf, sizeof(buf), "%u:%u", dw1, dw2);
	DWORD dwComplex = GetCRC32(buf, strlen(buf));

	if (m_set_requestToAdd.find(dwComplex) == m_set_requestToAdd.end())
	{
		sys_log(0, "MessengerManager::AuthToAdd : request not exist %s -> %s", companion.c_str(), account.c_str());
		return false;
	}

	m_set_requestToAdd.erase(dwComplex);

	if (!bDeny)
	{
		AddToList(companion, account);
		AddToList(account, companion);
	}
	return true;
}

void MessengerManager::__AddToList(MessengerManager::keyA account, MessengerManager::keyA companion)
{
	m_Relation[account].insert(companion);
	m_InverseRelation[companion].insert(account);

	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindPC(account.c_str());
#if defined(ENABLE_TEXTS_RENEWAL)
	if (ch) {
		ch->ChatPacketNew(CHAT_TYPE_INFO, 183, "%s", companion.c_str());
	}
#endif
	LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(companion.c_str());

	if (tch)
		SendLogin(account, companion);
	else
		SendLogout(account, companion);
}

void MessengerManager::AddToList(MessengerManager::keyA account, MessengerManager::keyA companion)
{
	if (companion.size() == 0)
		return;

	if (m_Relation[account].find(companion) != m_Relation[account].end())
		return;

	// @fixme142 BEGIN
	char __account[CHARACTER_NAME_MAX_LEN * 2 + 1];
	char __companion[CHARACTER_NAME_MAX_LEN * 2 + 1];

	DBManager::instance().EscapeString(__account, sizeof(__account), account.c_str(), account.size());
	DBManager::instance().EscapeString(__companion, sizeof(__companion), companion.c_str(), companion.size());

	if (account.compare(__account) || companion.compare(__companion))
	{
		return;
	}
	// @fixme142 END

	sys_log(0, "Messenger Add %s %s", account.c_str(), companion.c_str());
	DBManager::instance().Query("INSERT INTO messenger_list%s VALUES ('%s', '%s')",
			get_table_postfix(), __account, __companion);

	__AddToList(account, companion);

	TPacketGGMessenger p2ppck;

	p2ppck.bHeader = HEADER_GG_MESSENGER_ADD;
	strlcpy(p2ppck.szAccount, account.c_str(), sizeof(p2ppck.szAccount));
	strlcpy(p2ppck.szCompanion, companion.c_str(), sizeof(p2ppck.szCompanion));
	P2P_MANAGER::instance().Send(&p2ppck, sizeof(TPacketGGMessenger));
}

void MessengerManager::__RemoveFromList(MessengerManager::keyA account, MessengerManager::keyA companion)
{
	m_Relation[account].erase(companion);
	m_InverseRelation[companion].erase(account);

	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindPC(account.c_str());
#if defined(ENABLE_TEXTS_RENEWAL)
	if (ch) {
		ch->ChatPacketNew(CHAT_TYPE_INFO, 182, "%s", companion.c_str());
	}
#endif
}

void MessengerManager::RemoveFromList(MessengerManager::keyA account, MessengerManager::keyA companion)
{
	if (companion.size() == 0)
		return;

	// @fixme142 BEGIN
	char __account[CHARACTER_NAME_MAX_LEN * 2 + 1];
	char __companion[CHARACTER_NAME_MAX_LEN * 2 + 1];

	DBManager::instance().EscapeString(__account, sizeof(__account), account.c_str(), account.size());
	DBManager::instance().EscapeString(__companion, sizeof(__companion), companion.c_str(), companion.size());

	if (account.compare(__account) || companion.compare(__companion))
	{
		return;
	}
	// @fixme142 END

	sys_log(1, "Messenger Remove %s %s", account.c_str(), companion.c_str());
	DBManager::instance().Query("DELETE FROM messenger_list%s WHERE account='%s' AND companion = '%s'",
			get_table_postfix(), __account, __companion);

	__RemoveFromList(account, companion);

	TPacketGGMessenger p2ppck;

	p2ppck.bHeader = HEADER_GG_MESSENGER_REMOVE;
	strlcpy(p2ppck.szAccount, account.c_str(), sizeof(p2ppck.szAccount));
	strlcpy(p2ppck.szCompanion, companion.c_str(), sizeof(p2ppck.szCompanion));
	P2P_MANAGER::instance().Send(&p2ppck, sizeof(TPacketGGMessenger));
}

void MessengerManager::RemoveAllList(keyA account)
{
	std::set<keyT>	company(m_Relation[account]);

	// @fixme142 BEGIN
	char __account[CHARACTER_NAME_MAX_LEN * 2 + 1];

	DBManager::instance().EscapeString(__account, sizeof(__account), account.c_str(), account.size());

	if (account.compare(__account))
	{
		return;
	}
	// @fixme142 END

	/* SQL Data 삭제 */
	DBManager::instance().Query("DELETE FROM messenger_list%s WHERE account='%s' OR companion='%s'",
			get_table_postfix(), __account, __account);

	/* 내가 가지고있는 리스트 삭제 */
	for (std::set<keyT>::iterator iter = company.begin();
			iter != company.end();
			iter++ )
	{
		this->RemoveFromList(account, *iter);
	}

	/* 복사한 데이타 삭제 */
	for (std::set<keyT>::iterator iter = company.begin();
			iter != company.end();
			)
	{
		company.erase(iter++);
	}

	company.clear();
}


void MessengerManager::SendList(MessengerManager::keyA account)
{
	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindPC(account.c_str());

	if (!ch)
		return;

	LPDESC d = ch->GetDesc();

	if (!d)
		return;

	if (m_Relation.find(account) == m_Relation.end())
		return;

	if (m_Relation[account].empty())
		return;

	TPacketGCMessenger pack;

	pack.header		= HEADER_GC_MESSENGER;
	pack.subheader	= MESSENGER_SUBHEADER_GC_LIST;
	pack.size		= sizeof(TPacketGCMessenger);

	TPacketGCMessengerListOffline pack_offline;
	TPacketGCMessengerListOnline pack_online;

	TEMP_BUFFER buf(128 * 1024); // 128k

	auto it = m_Relation[account].begin(), eit = m_Relation[account].end();

	while (it != eit)
	{
		if (m_set_loginAccount.find(*it) != m_set_loginAccount.end())
		{
			pack_online.connected = 1;
			int iSize = it->size();
			// Online
			pack_online.length = iSize;

			buf.write(&pack_online, sizeof(TPacketGCMessengerListOnline));
			buf.write(it->c_str(), iSize);
		}
		else
		{
			pack_offline.connected = 0;
			int iSize = it->size();
			// Offline
			pack_offline.length = iSize;

			buf.write(&pack_offline, sizeof(TPacketGCMessengerListOffline));
			buf.write(it->c_str(), iSize);
		}

		++it;
	}

	pack.size += buf.size();

	d->BufferedPacket(&pack, sizeof(TPacketGCMessenger));
	d->Packet(buf.read_peek(), buf.size());
}

void MessengerManager::SendLogin(MessengerManager::keyA account, MessengerManager::keyA companion)
{
	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindPC(account.c_str());
	LPDESC d = ch ? ch->GetDesc() : NULL;

	if (!d)
		return;

	if (!d->GetCharacter())
		return;

	if (ch->GetGMLevel() == GM_PLAYER && gm_get_level(companion.c_str()) != GM_PLAYER)
		return;

	BYTE bLen = companion.size();

	TPacketGCMessenger pack;

	pack.header			= HEADER_GC_MESSENGER;
	pack.subheader		= MESSENGER_SUBHEADER_GC_LOGIN;
	pack.size			= sizeof(TPacketGCMessenger) + sizeof(BYTE) + bLen;

	d->BufferedPacket(&pack, sizeof(TPacketGCMessenger));
	d->BufferedPacket(&bLen, sizeof(BYTE));
	d->Packet(companion.c_str(), companion.size());
}

void MessengerManager::SendLogout(MessengerManager::keyA account, MessengerManager::keyA companion)
{
	if (!companion.size())
		return;

	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindPC(account.c_str());
	LPDESC d = ch ? ch->GetDesc() : NULL;

	if (!d)
		return;

	BYTE bLen = companion.size();

	TPacketGCMessenger pack;

	pack.header		= HEADER_GC_MESSENGER;
	pack.subheader	= MESSENGER_SUBHEADER_GC_LOGOUT;
	pack.size		= sizeof(TPacketGCMessenger) + sizeof(BYTE) + bLen;

	d->BufferedPacket(&pack, sizeof(TPacketGCMessenger));
	d->BufferedPacket(&bLen, sizeof(BYTE));
	d->Packet(companion.c_str(), companion.size());
}

#if defined(ENABLE_MESSENGER_BLOCK)
bool MessengerManager::CheckMessengerList(const std::string& ch, const std::string& tch, uint8_t type)
{
	auto* relation = type == SYST_BLOCK ? &m_BlockRelation : &m_Relation;
	if (relation->empty())
		return false;

	auto it = relation->find(ch);

	if (it != relation->end())
	{
		if (it->second.find(tch) != it->second.end())
			return true;
	}

	it = relation->find(tch);
	if (it != relation->end())
	{
		if (it->second.find(ch) != it->second.end())
			return true;
	}

	return false;
}

void MessengerManager::LoadBlockList(SQLMsg* msg) {
	if (!msg || !msg->Get() || msg->Get()->uiNumRows == 0)
	{
		return;
	}

	std::string account;

	MYSQL_ROW row = NULL;

	for (auto i = 0; i < msg->Get()->uiNumRows; ++i) {
		row = mysql_fetch_row(msg->Get()->pSQLResult);

		if (row[0] && row[1]) {
			if (account.length() == 0) {
				account = row[0];
			}

			m_BlockRelation[row[0]].insert(row[1]);
			m_InverseBlockRelation[row[1]].insert(row[0]);
		}
	}

	SendBlockList(account);

	auto it = m_InverseBlockRelation[account].begin();

	for (; it != m_InverseBlockRelation[account].end(); ++it) {
		SendBlockLogin(*it, account);
	}
}

void MessengerManager::SendBlockList(MessengerManager::keyA account) {
	const LPCHARACTER pkChar = CHARACTER_MANAGER::instance().FindPC(account.c_str());
	const LPDESC d = pkChar ? pkChar->GetDesc() : nullptr;

	if (!d) {
		return;
	}

	TPacketGCMessenger p;

	p.header = HEADER_GC_MESSENGER;
	p.subheader = MESSENGER_SUBHEADER_GC_BLOCK_LIST;
	p.size = sizeof(TPacketGCMessenger);

	TPacketGCMessengerBlockList pack_offline;
	TPacketGCMessengerBlockList pack_online;

	TEMP_BUFFER buf(128 * 1024);

	auto it = m_BlockRelation[account].begin(), eit = m_BlockRelation[account].end();

	while (it != eit) {
		if (m_set_loginAccount.find(*it) != m_set_loginAccount.end()) {
			pack_online.connected = 1;
			pack_online.length = it->size();

			buf.write(&pack_online, sizeof(TPacketGCMessengerBlockList));
			buf.write(it->c_str(), it->size());
		} else {
			pack_offline.connected = 0;
			pack_offline.length = it->size();

			buf.write(&pack_offline, sizeof(TPacketGCMessengerBlockList));
			buf.write(it->c_str(), it->size());
		}

		++it;
	}

	p.size += buf.size();

	d->BufferedPacket(&p, sizeof(TPacketGCMessenger));
	d->Packet(buf.read_peek(), buf.size());
}

void MessengerManager::SendBlockLogin(MessengerManager::keyA account, MessengerManager::keyA companion) {
	const LPCHARACTER pkChar = CHARACTER_MANAGER::instance().FindPC(account.c_str());
	const LPDESC d = pkChar ? pkChar->GetDesc() : nullptr;

	if (!d) {
		return;
	}

	uint8_t bLen = companion.size();

	TPacketGCMessenger p;

	p.header = HEADER_GC_MESSENGER;
	p.subheader = MESSENGER_SUBHEADER_GC_BLOCK_LOGIN;
	p.size = sizeof(TPacketGCMessenger) + sizeof(uint8_t) + bLen;

	d->BufferedPacket(&p, sizeof(TPacketGCMessenger));
	d->BufferedPacket(&bLen, sizeof(uint8_t));
	d->Packet(companion.c_str(), companion.size());
}

void MessengerManager::SendBlockLogout(MessengerManager::keyA account, MessengerManager::keyA companion) {
	if (!companion.size()) {
		return;
	}

	const LPCHARACTER pkChar = CHARACTER_MANAGER::instance().FindPC(account.c_str());
	const LPDESC d = pkChar ? pkChar->GetDesc() : nullptr;

	if (!d) {
		return;
	}

	uint8_t bLen = companion.size();

	TPacketGCMessenger p;

	p.header = HEADER_GC_MESSENGER;
	p.subheader = MESSENGER_SUBHEADER_GC_BLOCK_LOGOUT;
	p.size = sizeof(TPacketGCMessenger) + sizeof(BYTE) + bLen;

	d->BufferedPacket(&p, sizeof(TPacketGCMessenger));
	d->BufferedPacket(&bLen, sizeof(BYTE));
	d->Packet(companion.c_str(), companion.size());
}

void MessengerManager::AddToBlockList(MessengerManager::keyA account, MessengerManager::keyA companion) {
	if (companion.size() == 0) {
		return;
	}

	if (m_BlockRelation[account].find(companion) != m_BlockRelation[account].end()) {
		return;
	}

	char __account[CHARACTER_NAME_MAX_LEN * 2 + 1];
	char __companion[CHARACTER_NAME_MAX_LEN * 2 + 1];

	DBManager::instance().EscapeString(__account, sizeof(__account), account.c_str(), account.size());
	DBManager::instance().EscapeString(__companion, sizeof(__companion), companion.c_str(), companion.size());

	if (account.compare(__account) || companion.compare(__companion))
	{
		return;
	}

	DBManager::instance().Query("INSERT INTO messenger_block_list%s VALUES ('%s', '%s', NOW())", 
	get_table_postfix(), __account, __companion);

	__AddToBlockList(__account, __companion);

	TPacketGGMessengerBlock p;
	p.bHeader = HEADER_GG_MESSENGER_BLOCK;
	p.bAdd = 1;
	strlcpy(p.szAccount, __account, sizeof(p.szAccount));
	strlcpy(p.szCompanion, __companion, sizeof(p.szCompanion));

	P2P_MANAGER::instance().Send(&p, sizeof(p));
}

void MessengerManager::__AddToBlockList(MessengerManager::keyA account, MessengerManager::keyA companion) {
	m_BlockRelation[account].insert(companion);
	m_InverseBlockRelation[companion].insert(account);

	const LPCHARACTER pkChar = CHARACTER_MANAGER::instance().FindPC(account.c_str());
	const LPDESC d = pkChar ? pkChar->GetDesc() : nullptr;

	if (d) {
#if defined(ENABLE_TEXTS_RENEWAL)
		pkChar->ChatPacketNew(CHAT_TYPE_INFO, 1430, "%s", companion.c_str());
#else
		pkChar->ChatPacket(CHAT_TYPE_INFO, "%s has been blocked.", companion.c_str());
#endif
	}

	const LPCHARACTER pkTargetChar = CHARACTER_MANAGER::instance().FindPC(companion.c_str());

	if (pkTargetChar) {
		SendBlockLogin(account, companion);
		return;
	}

	SendBlockLogout(account, companion);
}

void MessengerManager::RemoveFromBlockList(MessengerManager::keyA account, MessengerManager::keyA companion) {
	if (companion.size() == 0)
		return;

	char __account[CHARACTER_NAME_MAX_LEN * 2 + 1];
	char __companion[CHARACTER_NAME_MAX_LEN * 2 + 1];

	DBManager::instance().EscapeString(__account, sizeof(__account), account.c_str(), account.size());
	DBManager::instance().EscapeString(__companion, sizeof(__companion), companion.c_str(), companion.size());

	if (account.compare(__account) || companion.compare(__companion))
	{
		return;
	}

	DBManager::instance().Query("DELETE FROM messenger_block_list%s WHERE account='%s' AND companion = '%s'",
	get_table_postfix(), __account, __companion);

	__RemoveFromBlockList(__account, __companion);

	TPacketGGMessengerBlock p;
	p.bHeader = HEADER_GG_MESSENGER_BLOCK;
	p.bAdd = 0;
	strlcpy(p.szAccount, __account, sizeof(p.szAccount));
	strlcpy(p.szCompanion, __companion, sizeof(p.szCompanion));

	P2P_MANAGER::instance().Send(&p, sizeof(p));
}

void MessengerManager::__RemoveFromBlockList(MessengerManager::keyA account, MessengerManager::keyA companion) {
	m_BlockRelation[account].erase(companion);
	m_InverseBlockRelation[companion].erase(account);

	const LPCHARACTER pkChar = CHARACTER_MANAGER::instance().FindPC(account.c_str());
	const LPDESC d = pkChar ? pkChar->GetDesc() : nullptr;

	if (d) {
#if defined(ENABLE_TEXTS_RENEWAL)
		pkChar->ChatPacketNew(CHAT_TYPE_INFO, 1431, "%s", companion.c_str());
#else
		pkChar->ChatPacket(CHAT_TYPE_INFO, "%s has been unlocked.", companion.c_str());
#endif
	}
}

void MessengerManager::RemoveAllBlockList(keyA account)
{
	char __account[CHARACTER_NAME_MAX_LEN * 2 + 1];

	DBManager::instance().EscapeString(__account, sizeof(__account), account.c_str(), account.size());

	if (account.compare(__account))
	{
		return;
	}

	std::set<keyBL> company(m_BlockRelation[account]);

	DBManager::instance().Query("DELETE FROM messenger_block_list%s WHERE account='%s' OR companion='%s'",
	get_table_postfix(), __account, __account);

	auto it1 = company.begin(), eit1 = company.end();
	for (; it1 != eit1; it1++) {
		RemoveFromList(account, *it1);
	}

	auto it2 = company.begin(), eit2 = company.end();
	for (; it2 != eit2;) {
		company.erase(it2++);
	}

	company.clear();
}
#endif

#if defined(ENABLE_MESSENGER_TEAM)
void MessengerManager::ClearTeamList()
{
	m_vecTeamList.clear();
}

void MessengerManager::UpdateTeamList()
{
	std::for_each(m_set_loginAccount.begin(), m_set_loginAccount.end(), [&](const std::string& name) { SendTeamList(name, true); });
}

void MessengerManager::PushTeamList(const char* c_szName)
{
	if (GetTeamMember(c_szName) != nullptr)
	{
		return;
	}

	TTeamMessenger TeamMessenger = {};
	TeamMessenger.IsConnected = m_set_loginAccount.find(c_szName) != m_set_loginAccount.end();
	strlcpy(TeamMessenger.szName, c_szName, sizeof(TeamMessenger.szName));
	m_vecTeamList.emplace_back(TeamMessenger);
}

void MessengerManager::SendTeamList(keyA account, bool isReload)
{
	if (account.empty() || (!isReload && m_vecTeamList.empty()))
	{
		return;
	}

	const LPCHARACTER ch = CHARACTER_MANAGER::instance().FindPC(account.c_str());
	const LPDESC desc = ch != nullptr ? ch->GetDesc() : nullptr;

	if (desc != nullptr)
	{
		std::for_each(m_vecTeamList.begin(), m_vecTeamList.end(), [&](TTeamMessenger& member) { member.IsConnected = m_set_loginAccount.find(member.szName) != m_set_loginAccount.end(); });

		TPacketGCMessenger pack{};
		pack.header = HEADER_GC_MESSENGER;
		pack.subheader = MESSENGER_SUBHEADER_GC_TEAM;
		pack.size = static_cast<uint16_t>(sizeof(TPacketGCMessenger) + (sizeof(TTeamMessenger) * m_vecTeamList.size()));

		TEMP_BUFFER buf;
		buf.write(&pack, sizeof(pack));
		if (!m_vecTeamList.empty())
			buf.write(&m_vecTeamList[0], static_cast<int>(sizeof(TTeamMessenger) * m_vecTeamList.size()));
		desc->Packet(buf.read_peek(), buf.size());
	}

	if (!isReload)
	{
		UpdateTeam(account);
	}
}

void MessengerManager::UpdateTeam(keyA account)
{
	TTeamMessenger* member = GetTeamMember(account);
	if (!member)
	{
		return;
	}

	member->IsConnected = m_set_loginAccount.find(account) != m_set_loginAccount.end();

	TPacketGCMessenger pack{};
	pack.header = HEADER_GC_MESSENGER;
	pack.subheader = MESSENGER_SUBHEADER_GC_TEAM_UPDATE;
	pack.size = sizeof(TPacketGCMessenger) + sizeof(TTeamMessenger);

	for (const auto & i : m_set_loginAccount)
	{
		if (strcmp(i.c_str(), account.c_str()) == 0)
		{
			continue;
		}

		LPCHARACTER ch = CHARACTER_MANAGER::instance().FindPC(i.c_str());
		LPDESC desc = ch != nullptr ? ch->GetDesc() : nullptr;
		if (desc != nullptr)
		{
			desc->BufferedPacket(&pack, sizeof(TPacketGCMessenger));
			desc->Packet(member, sizeof(TTeamMessenger));
		}
	}
}

TTeamMessenger* MessengerManager::GetTeamMember(keyA account)
{
	if (account.empty())
	{
		return nullptr;
	}

	auto iterator = m_vecTeamList.begin();
	while (iterator != m_vecTeamList.end())
	{
		auto& t = *iterator++;
		if (strcmp(t.szName, account.c_str()) == 0)
			return &t;
	}

	return nullptr;
}
#endif

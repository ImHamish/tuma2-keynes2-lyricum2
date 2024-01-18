#include "stdafx.h"
#include "constants.h"
#include "config.h"
#include "utils.h"
#include "input.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "char.h"
#include "item.h"
#include "char_manager.h"
#include "cmd.h"
#include "buffer_manager.h"
#include "protocol.h"
#include "pvp.h"
#include "start_position.h"
#include "messenger_manager.h"
#include "guild_manager.h"
#include "party.h"
#include "dungeon.h"
#include "war_map.h"
#include "questmanager.h"
#include "building.h"
#include "wedding.h"
#include "affect.h"
#include "arena.h"
#include "OXEvent.h"
#include "priv_manager.h"
#include "log.h"
#include "horsename_manager.h"
#include "MarkManager.h"
#include "../common/CommonDefines.h"
#ifdef __ENABLE_NEW_OFFLINESHOP__
#include "new_offlineshop.h"
#include "new_offlineshop_manager.h"
#endif
#include "PulseManager.h"

static void _send_bonus_info(LPCHARACTER ch)
{
	int	item_drop_bonus = 0;
	int gold_drop_bonus = 0;
	int gold10_drop_bonus	= 0;
	int exp_bonus		= 0;

	item_drop_bonus = CPrivManager::instance().GetPriv(ch, PRIV_ITEM_DROP);
	gold_drop_bonus = CPrivManager::instance().GetPriv(ch, PRIV_GOLD_DROP);
	gold10_drop_bonus = CPrivManager::instance().GetPriv(ch, PRIV_GOLD10_DROP);
	exp_bonus = CPrivManager::instance().GetPriv(ch, PRIV_EXP_PCT);
	if (item_drop_bonus) {
		ch->ChatPacketNew(CHAT_TYPE_INFO, 243, "%d", item_drop_bonus);
	}
	if (gold_drop_bonus) {
		ch->ChatPacketNew(CHAT_TYPE_INFO, 244, "%d", item_drop_bonus);
	}
	if (gold10_drop_bonus) {
		ch->ChatPacketNew(CHAT_TYPE_INFO, 245, "%d", item_drop_bonus);
	}
	if (exp_bonus) {
		ch->ChatPacketNew(CHAT_TYPE_INFO, 246, "%d", item_drop_bonus);
	}
}

static bool FN_is_battle_zone(LPCHARACTER ch)
{
	switch (ch->GetMapIndex())
	{
		case 1:         // 신수 1차 마을
		case 2:         // 신수 2차 마을
		case 21:        // 천조 1차 마을
		case 23:        // 천조 2차 마을
		case 41:        // 진노 1차 마을
		case 43:        // 진노 2차 마을
		case 113:       // OX 맵
			return false;
	}

	return true;
}

void CInputLogin::LoginByKey(LPDESC d, const char * data)
{
	TPacketCGLogin2 * pinfo = (TPacketCGLogin2 *) data;

	char login[LOGIN_MAX_LEN + 1];
	trim_and_lower(pinfo->login, login, sizeof(login));

	if (g_bNoMoreClient)
	{
		TPacketGCLoginFailure failurePacket;

		failurePacket.header = HEADER_GC_LOGIN_FAILURE;
		strlcpy(failurePacket.szStatus, "SHUTDOWN", sizeof(failurePacket.szStatus));
		d->Packet(&failurePacket, sizeof(TPacketGCLoginFailure));
		return;
	}

	if (g_iUserLimit > 0)
	{
		int iTotal;
		int * paiEmpireUserCount;
		int iLocal;

		DESC_MANAGER::instance().GetUserCount(iTotal, &paiEmpireUserCount, iLocal);

		if (g_iUserLimit <= iTotal)
		{
			TPacketGCLoginFailure failurePacket;

			failurePacket.header = HEADER_GC_LOGIN_FAILURE;
			strlcpy(failurePacket.szStatus, "FULL", sizeof(failurePacket.szStatus));

			d->Packet(&failurePacket, sizeof(TPacketGCLoginFailure));
			return;
		}
	}

	sys_log(0, "LOGIN_BY_KEY: %s key %u", login, pinfo->dwLoginKey);

	d->SetLoginKey(pinfo->dwLoginKey);
#ifndef _IMPROVED_PACKET_ENCRYPTION_
	d->SetSecurityKey(pinfo->adwClientKey);
#endif

	TPacketGDLoginByKey ptod;

	strlcpy(ptod.szLogin, login, sizeof(ptod.szLogin));
	ptod.dwLoginKey = pinfo->dwLoginKey;
	memcpy(ptod.adwClientKey, pinfo->adwClientKey, sizeof(DWORD) * 4);
	strlcpy(ptod.szIP, d->GetHostName(), sizeof(ptod.szIP));

	db_clientdesc->DBPacket(HEADER_GD_LOGIN_BY_KEY, d->GetHandle(), &ptod, sizeof(TPacketGDLoginByKey));
}
void CInputLogin::CharacterSelect(LPDESC d, const char * data)
{
	struct command_player_select * pinfo = (struct command_player_select *) data;
	TAccountTable & c_r = d->GetAccountTable();

	sys_log(0, "player_select: login: %s index: %d", c_r.login, pinfo->index);

	if (!c_r.id)
	{
		sys_err("no account table");
		d->SetPhase(PHASE_CLOSE);
		return;
	}

	if (pinfo->index >= PLAYER_PER_ACCOUNT || pinfo->index < 0)
	{
		sys_err("index overflow %d, login: %s", pinfo->index, c_r.login);
		d->SetPhase(PHASE_CLOSE);
		return;
	}

	if (!c_r.players[pinfo->index].dwID)
	{
		sys_err("no player id, login %s", c_r.login);
		d->SetPhase(PHASE_CLOSE);
		return;
	}

	for (const auto& player : c_r.players) // FIXME
	{
		if (player.is_selected)
		{
			sys_err("%s: is trying to select the player again, wtf?", c_r.login);
			d->SetPhase(PHASE_CLOSE);
			return;
		}
	}

	if (c_r.players[pinfo->index].bChangeName)
	{
		sys_err("name must be changed idx %d, login %s, name %s",
				pinfo->index, c_r.login, c_r.players[pinfo->index].szName);
		return;
	}

	c_r.players[pinfo->index].is_selected = true;
	TPlayerLoadPacket player_load_packet;

	player_load_packet.account_id	= c_r.id;
	player_load_packet.player_id	= c_r.players[pinfo->index].dwID;
	player_load_packet.account_index	= pinfo->index;

	db_clientdesc->DBPacket(HEADER_GD_PLAYER_LOAD, d->GetHandle(), &player_load_packet, sizeof(TPlayerLoadPacket));
}

bool NewPlayerTable(TPlayerTable * table,
		const char * name,
		BYTE job,
		BYTE shape,
		BYTE bEmpire,
		BYTE bCon,
		BYTE bInt,
		BYTE bStr,
		BYTE bDex)
{
	if (job >= JOB_MAX_NUM)
		return false;

	memset(table, 0, sizeof(TPlayerTable));

	strlcpy(table->name, name, sizeof(table->name));

	table->job = job;
	table->voice = 0;
	table->part_base = shape;

	table->st = JobInitialPoints[job].st;
	table->dx = JobInitialPoints[job].dx;
	table->ht = JobInitialPoints[job].ht;
	table->iq = JobInitialPoints[job].iq;

	table->hp = JobInitialPoints[job].max_hp + table->ht * JobInitialPoints[job].hp_per_ht;
	table->sp = JobInitialPoints[job].max_sp + table->iq * JobInitialPoints[job].sp_per_iq;
	table->stamina = JobInitialPoints[job].max_stamina;

	table->x 	= CREATE_START_X(bEmpire) + number(-300, 300);
	table->y 	= CREATE_START_Y(bEmpire) + number(-300, 300);

	table->z	= 0;
	table->dir	= 0;
	table->playtime = 0;
#ifdef USE_REWARD_AT_FIRST_LOGIN
    table->level = 5;
    table->gold = 500000;

    for (uint8_t i = 1; i < table->level; ++i)
    {
        int32_t iHP = number(JobInitialPoints[job].hp_per_lv_begin, JobInitialPoints[job].hp_per_lv_end);
        int32_t iSP = number(JobInitialPoints[job].sp_per_lv_begin, JobInitialPoints[job].sp_per_lv_end);
        table->sRandomHP += iHP;
        table->sRandomSP += iSP;
        table->stat_point += 3;
    }

    table->hp += table->sRandomHP;
    table->sp += table->sRandomSP;
#else
    table->level = 1;
    table->gold = 0;
#endif
#if defined(ENABLE_WORLDBOSS)
	table->wbpoints = PLAYER_CREATE_WB_POINTS;
#endif
#if defined(ENABLE_HALLOWEEN_EVENT_2022)
	table->skull = PLAYER_CREATE_SKULL;
#endif
#if defined(ENABLE_GAYA_RENEWAL)
	table->gem = 0;
#endif
	table->skill_group = 0;

	return true;
}

bool RaceToJob(unsigned race, unsigned* ret_job)
{
	*ret_job = 0;

	if (race >= MAIN_RACE_MAX_NUM)
		return false;

	switch (race)
	{
		case MAIN_RACE_WARRIOR_M:
			*ret_job = JOB_WARRIOR;
			break;

		case MAIN_RACE_WARRIOR_W:
			*ret_job = JOB_WARRIOR;
			break;

		case MAIN_RACE_ASSASSIN_M:
			*ret_job = JOB_ASSASSIN;
			break;

		case MAIN_RACE_ASSASSIN_W:
			*ret_job = JOB_ASSASSIN;
			break;

		case MAIN_RACE_SURA_M:
			*ret_job = JOB_SURA;
			break;

		case MAIN_RACE_SURA_W:
			*ret_job = JOB_SURA;
			break;

		case MAIN_RACE_SHAMAN_M:
			*ret_job = JOB_SHAMAN;
			break;

		case MAIN_RACE_SHAMAN_W:
			*ret_job = JOB_SHAMAN;
			break;
		default:
			return false;
			break;
	}
	return true;
}

// 신규 캐릭터 지원
bool NewPlayerTable2(TPlayerTable * table, const char * name, BYTE race, BYTE shape, BYTE bEmpire)
{
	if (race >= MAIN_RACE_MAX_NUM)
	{
		sys_err("NewPlayerTable2.OUT_OF_RACE_RANGE(%d >= max(%d))\n", race, MAIN_RACE_MAX_NUM);
		return false;
	}

	unsigned job;

	if (!RaceToJob(race, &job))
	{
		sys_err("NewPlayerTable2.RACE_TO_JOB_ERROR(%d)\n", race);
		return false;
	}

	sys_log(0, "NewPlayerTable2(name=%s, race=%d, job=%d)", name, race, job);

	memset(table, 0, sizeof(TPlayerTable));

	strlcpy(table->name, name, sizeof(table->name));

	table->level = 1;
	table->job = race; // 직업대신 종족을 넣는다
	table->voice = 0;
	table->part_base = shape;

	table->st = JobInitialPoints[job].st;
	table->dx = JobInitialPoints[job].dx;
	table->ht = JobInitialPoints[job].ht;
	table->iq = JobInitialPoints[job].iq;

	table->hp = JobInitialPoints[job].max_hp + table->ht * JobInitialPoints[job].hp_per_ht;
	table->sp = JobInitialPoints[job].max_sp + table->iq * JobInitialPoints[job].sp_per_iq;
	table->stamina = JobInitialPoints[job].max_stamina;

	table->x = CREATE_START_X(bEmpire) + number(-300, 300);
	table->y = CREATE_START_Y(bEmpire) + number(-300, 300);

	table->z = 0;
	table->dir = 0;
	table->playtime = 0;
#ifdef USE_REWARD_AT_FIRST_LOGIN
    table->level = 5;
    table->gold = 500000;

    for (uint8_t i = 1; i < table->level; ++i)
    {
        int32_t iHP = number(JobInitialPoints[job].hp_per_lv_begin, JobInitialPoints[job].hp_per_lv_end);
        int32_t iSP = number(JobInitialPoints[job].sp_per_lv_begin, JobInitialPoints[job].sp_per_lv_end);
        table->sRandomHP += iHP;
        table->sRandomSP += iSP;
        table->stat_point += 3;
    }

    table->hp += table->sRandomHP;
    table->sp += table->sRandomSP;
#else
    table->level = 1;
    table->gold = 0;
#endif
#if defined(ENABLE_WORLDBOSS)
	table->wbpoints = PLAYER_CREATE_WB_POINTS;
#endif
#if defined(ENABLE_HALLOWEEN_EVENT_2022)
	table->skull = PLAYER_CREATE_SKULL;
#endif
#if defined(ENABLE_GAYA_RENEWAL)
	table->gem = 0;
#endif
	table->stat_point = 0;
	table->skill_group = 0;

	return true;
}

void CInputLogin::CharacterCreate(LPDESC d, const char * data)
{
	struct command_player_create * pinfo = (struct command_player_create *) data;
	TPlayerCreatePacket player_create_packet;

	const TAccountTable& c_rAccountTable = d->GetAccountTable();
	if (!c_rAccountTable.id)
	{
		sys_err("no account table");
		d->SetPhase(PHASE_CLOSE);
		return;
	}

	sys_log(0, "PlayerCreate: name %s pos %d job %d shape %d",
			pinfo->name,
			pinfo->index,
			pinfo->job,
			pinfo->shape);

	TPacketGCLoginFailure packFailure;
	memset(&packFailure, 0, sizeof(packFailure));
	packFailure.header = HEADER_GC_CHARACTER_CREATE_FAILURE;

	if (true == g_BlockCharCreation)
	{
		d->Packet(&packFailure, sizeof(packFailure));
		return;
	}

	if (strlen(pinfo->name) > 12 || strlen(pinfo->name) < 3)
	{
		d->Packet(&packFailure, sizeof(packFailure));
		return;
	}

	if (pinfo->index >= PLAYER_PER_ACCOUNT)
	{
		sys_err("index overflow %d, login: %s", pinfo->index, c_rAccountTable.login);
		d->SetPhase(PHASE_CLOSE);
		return;
	}

	if (c_rAccountTable.players[pinfo->index].dwID != 0) // FIXME
	{
		sys_err("login(%s) no empty character slot(%u)", c_rAccountTable.login, pinfo->index);
		d->SetPhase(PHASE_CLOSE);
		return;
	}

	if (g_country_flags.find(pinfo->country_flag) == g_country_flags.end())
	{
		d->Packet(&packFailure, sizeof(packFailure));
		return;
	}

	// 사용할 수 없는 이름이거나, 잘못된 평상복이면 생설 실패
	if (!check_name(pinfo->name) || pinfo->shape > 1)
	{
		d->Packet(&packFailure, sizeof(packFailure));
		return;
	}

	if (0 == strcmp(c_rAccountTable.login, pinfo->name))
	{
		TPacketGCCreateFailure pack;
		pack.header = HEADER_GC_CHARACTER_CREATE_FAILURE;
		pack.bType = 1;

		d->Packet(&pack, sizeof(pack));
		return;
	}

	memset(&player_create_packet, 0, sizeof(TPlayerCreatePacket));

	if (!NewPlayerTable2(&player_create_packet.player_table, pinfo->name, pinfo->job, pinfo->shape, d->GetEmpire()))
	{
		sys_err("player_prototype error: job %d face %d ", pinfo->job);
		d->Packet(&packFailure, sizeof(packFailure));
		return;
	}

	strlcpy(player_create_packet.player_table.country_flag, pinfo->country_flag, sizeof(player_create_packet.player_table.country_flag));

	trim_and_lower(c_rAccountTable.login, player_create_packet.login, sizeof(player_create_packet.login));
	strlcpy(player_create_packet.passwd, c_rAccountTable.passwd, sizeof(player_create_packet.passwd));

	player_create_packet.account_id	= c_rAccountTable.id;
	player_create_packet.account_index	= pinfo->index;

	sys_log(0, "PlayerCreate: name %s account_id %d, TPlayerCreatePacketSize(%d), Packet->Gold %d",
			pinfo->name,
			pinfo->index,
			sizeof(TPlayerCreatePacket),
			player_create_packet.player_table.gold);

	db_clientdesc->DBPacket(HEADER_GD_PLAYER_CREATE, d->GetHandle(), &player_create_packet, sizeof(TPlayerCreatePacket));
}

void CInputLogin::CharacterDelete(LPDESC d, const char * data)
{
	struct command_player_delete * pinfo = (struct command_player_delete *) data;
	const TAccountTable & c_rAccountTable = d->GetAccountTable();

	if (!c_rAccountTable.id)
	{
		sys_err("PlayerDelete: no login data");
		d->SetPhase(PHASE_CLOSE);
		return;
	}

	sys_log(0, "PlayerDelete: login: %s index: %d, social_id %s", c_rAccountTable.login, pinfo->index, pinfo->private_code);

	if (pinfo->index >= PLAYER_PER_ACCOUNT || pinfo->index < 0)
	{
		sys_err("PlayerDelete: index overflow %d, login: %s", pinfo->index, c_rAccountTable.login);
		d->SetPhase(PHASE_CLOSE);
		return;
	}

	if (!c_rAccountTable.players[pinfo->index].dwID)
	{
		sys_err("PlayerDelete: Wrong Social ID index %d, login: %s", pinfo->index, c_rAccountTable.login);
		d->Packet(encode_byte(HEADER_GC_CHARACTER_DELETE_WRONG_SOCIAL_ID), 1);
		return;
	}

	// @fixme143 BEGIN
	static char __private_code[8 * 2 + 1];
	DBManager::instance().EscapeString(__private_code, sizeof(__private_code), pinfo->private_code, strnlen(pinfo->private_code, sizeof(pinfo->private_code)));
	if (strncmp(__private_code, pinfo->private_code, strnlen(pinfo->private_code, sizeof(pinfo->private_code))))
	{
		return;
	}
	// @fixme143 END

	TPlayerDeletePacket	player_delete_packet;
	trim_and_lower(c_rAccountTable.login, player_delete_packet.login, sizeof(player_delete_packet.login));
	player_delete_packet.player_id = c_rAccountTable.players[pinfo->index].dwID;
	player_delete_packet.account_index = pinfo->index;
	strlcpy(player_delete_packet.private_code, __private_code, sizeof(player_delete_packet.private_code));
	db_clientdesc->DBPacket(HEADER_GD_PLAYER_DELETE, d->GetHandle(), &player_delete_packet, sizeof(TPlayerDeletePacket));
}

#pragma pack(1)
typedef struct SPacketGTLogin
{
	BYTE header;
	WORD empty;
	DWORD id;
} TPacketGTLogin;
#pragma pack()

void CInputLogin::Entergame(LPDESC d, const char * data)
{
    const TAccountTable& c_rAccountTable = d->GetAccountTable();
    if (!c_rAccountTable.id)
    {
        d->SetPhase(PHASE_CLOSE);
        return;
    }

    LPCHARACTER ch = d ? d->GetCharacter() : nullptr;
    if (!ch)
    {
        d->SetPhase(PHASE_CLOSE);
        return;
    }

    std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT `status` FROM `account`.`account` WHERE `id`=%u LIMIT 1", ch->GetAID()));
    if (pMsg->Get()->uiNumRows == 0)
    {
        d->SetPhase(PHASE_CLOSE);
        return;
    }
    else
    {
        uint8_t bCol = 0;

        MYSQL_ROW mRow = mRow = mysql_fetch_row(pMsg->Get()->pSQLResult);
        if (!mRow[bCol]) 
        {
            sys_err("Error column %u.", bCol);
            d->SetPhase(PHASE_CLOSE);
            return;
        }

        char szStatus[ACCOUNT_STATUS_MAX_LEN + 1];
        strlcpy(szStatus, mRow[bCol++], sizeof(szStatus));

        if (strcmp(szStatus, "OK"))
        {
            sys_err("%s entered the game with %s account status.", ch->GetName(), szStatus);
            d->SetPhase(PHASE_CLOSE);
            return;
        }
    }

	if (ch->IsGM())
	{
		if (const auto qc = quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID()); !qc || (qc && !qc->IsLoaded()) || ch->GetQuestFlag("aff.observer") != 0)
			ch->SetObserverMode(true, false);
	}

	PIXEL_POSITION pos = ch->GetXYZ();

	if (!SECTREE_MANAGER::instance().GetMovablePosition(ch->GetMapIndex(), pos.x, pos.y, pos))
	{
		PIXEL_POSITION pos2;
		SECTREE_MANAGER::instance().GetRecallPositionByEmpire(ch->GetMapIndex(), ch->GetEmpire(), pos2);

		sys_err("!GetMovablePosition (name %s %dx%d map %d changed to %dx%d)",
				ch->GetName(),
				pos.x, pos.y,
				ch->GetMapIndex(),
				pos2.x, pos2.y);
		pos = pos2;
	}

	d->SetPhase(PHASE_GAME);
	sys_log(0, "ENTERGAME: %s %dx%dx%d %s map_index %d",
			ch->GetName(), ch->GetX(), ch->GetY(), ch->GetZ(), d->GetHostName(), ch->GetMapIndex());

	CGuildManager::instance().LoginMember(ch);

	ch->Show(ch->GetMapIndex(), pos.x, pos.y, pos.z);
	ch->ReviveInvisible(5);
	SECTREE_MANAGER::instance().SendNPCPosition(ch);
#ifdef ENABLE_ATLAS_BOSS
	SECTREE_MANAGER::instance().SendBossPosition(ch);
#endif

	if (ch->IsGM() && ch->IsAffectFlag(AFF_INVISIBILITY) && !test_server)
		ch->SetObserverMode(true);

	if(ch->GetItemAward_cmd())
		quest::CQuestManager::instance().ItemInformer(ch->GetPlayerID(),ch->GetItemAward_vnum());

	if (ch->GetHorseLevel() > 0)
	{
		ch->EnterHorse();
	}

#if defined(USE_MOUNT_COSTUME_SYSTEM)
	const LPITEM pCostumeMount = ch->GetWear(WEAR_COSTUME_MOUNT);
	if (pCostumeMount)
	{
#if defined(ENABLE_MOUNTSKIN)
		uint32_t mountVnum = 0;
		const LPITEM pSkinMount = ch->GetWear(WEAR_MOUNTSKIN);
		if (pSkinMount)
		{
#if defined(ENABLE_HIDE_COSTUME_SYSTEM)
			mountVnum = ch->GetPartStatus(HIDE_MOUNTSKIN) == 1 ? pCostumeMount->GetMountVnum() : pSkinMount->GetMountVnum();
#else
			mountVnum = pSkinMount->GetMountVnum();
#endif
		}
		else
		{
			mountVnum = pCostumeMount->GetMountVnum();
		}
#else
		uint32_t mountVnum = pCostumeMount->GetMountVnum();
#endif

		if (mountVnum != 0)
		{
			ch->MountVnum(mountVnum);
		}
	}
#endif

	// 플레이시간 레코딩 시작
	ch->ResetPlayTime();

	// 자동 저장 이벤트 추가
	ch->StartSaveEvent();
	ch->StartRecoveryEvent();

	CPVPManager::instance().Connect(ch);
	CPVPManager::instance().SendList(d);

	MessengerManager::instance().Login(ch->GetName());

	CPartyManager::instance().SetParty(ch);
	CGuildManager::instance().SendGuildWar(ch);

	building::CManager::instance().SendLandList(d, ch->GetMapIndex());

	marriage::CManager::instance().Login(ch);
#ifdef ENABLE_EVENT_MANAGER
	CHARACTER_MANAGER::Instance().SendDataPlayer(ch);
#endif
#ifdef ENABLE_REWARD_SYSTEM
	CHARACTER_MANAGER::Instance().SendRewardInfo(ch);
#endif
	TPacketGCTime p;
	p.bHeader = HEADER_GC_TIME;
	p.time = get_global_time();
	d->Packet(&p, sizeof(p));

	TPacketGCChannel p2;
	p2.header = HEADER_GC_CHANNEL;
	p2.channel = g_bChannel;
	d->Packet(&p2, sizeof(p2));
	ch->SendGreetMessage();

#ifdef ENABLE_PVP_ADVANCED // If something is wrong and server is crashed or stopping when you was in duel.
	int isDuel = ch->GetQuestFlag(CHECK_IS_FIGHT);
	if (isDuel)
		ch->SetQuestFlag(CHECK_IS_FIGHT, 0);
#endif
	
	_send_bonus_info(ch);
#ifdef __ENABLE_PREMIUM_PLAYERS__
	// ch->CheckPremiumPlayersAffects();
	if (ch->IsPremiumPlayer())
		ch->StartPremiumPlayersUpdateEvent();
#endif
#if defined(BL_OFFLINE_MESSAGE)
	ch->ReadOfflineMessages();
#endif

    for (uint8_t i = 0; i < PREMIUM_MAX_NUM; ++i)
    {
#ifdef __ENABLE_PREMIUM_PLAYERS__
        if (i == PREMIUM_AUTO_USE)
        {
            continue;
        }
#endif

        const auto remain = ch->GetPremiumRemainSeconds(i);
        if (remain <= 0)
        {
            continue;
        }

        ch->AddAffect(AFFECT_PREMIUM_START + i, POINT_NONE, 0, 0, remain, 0, true);
        sys_log(0, "PREMIUM: %s type %d %dmin", ch->GetName(), i, remain);
    }

	if (g_bCheckClientVersion)
	{
		sys_log(0, "VERSION CHECK %s %s", g_stClientVersion.c_str(), d->GetClientVersion());

		if (!d->GetClientVersion()) {
			d->DelayedDisconnect(10);
		}
		else {
			if (0 != g_stClientVersion.compare(d->GetClientVersion())) {
				d->DelayedDisconnect(0);
				LogManager::instance().HackLog("VERSION_CONFLICT", ch);
			}
		}
	}
	else
	{
		sys_log(0, "VERSION : NO CHECK");
	}

	if (ch->IsGM() == true)
		ch->ChatPacket(CHAT_TYPE_COMMAND, "ConsoleEnable");

	if (ch->GetMapIndex() >= 10000)
	{
		if (CWarMapManager::instance().IsWarMap(ch->GetMapIndex()))
			ch->SetWarMap(CWarMapManager::instance().Find(ch->GetMapIndex()));
		else if (marriage::WeddingManager::instance().IsWeddingMap(ch->GetMapIndex()))
			ch->SetWeddingMap(marriage::WeddingManager::instance().Find(ch->GetMapIndex()));
		else {
			ch->SetDungeon(CDungeonManager::instance().FindByMapIndex(ch->GetMapIndex()));
		}
	}
	else if (CArenaManager::instance().IsArenaMap(ch->GetMapIndex()) == true)
	{
		int memberFlag = CArenaManager::instance().IsMember(ch->GetMapIndex(), ch->GetPlayerID());
		if (memberFlag == MEMBER_OBSERVER)
		{
			ch->SetObserverMode(true);
			ch->SetArenaObserverMode(true);
			if (CArenaManager::instance().RegisterObserverPtr(ch, ch->GetMapIndex(), ch->GetX()/100, ch->GetY()/100))
			{
				sys_log(0, "ARENA : Observer add failed");
			}

			if (ch->IsHorseRiding() == true)
			{
				ch->StopRiding();
				ch->HorseSummon(false);
			}
		}
		else if (memberFlag == MEMBER_DUELIST)
		{
			TPacketGCDuelStart duelStart;
			duelStart.header = HEADER_GC_DUEL_START;
			duelStart.wSize = sizeof(TPacketGCDuelStart);

			ch->GetDesc()->Packet(&duelStart, sizeof(TPacketGCDuelStart));

			if (ch->IsHorseRiding() == true)
			{
				ch->StopRiding();
				ch->HorseSummon(false);
			}

			LPPARTY pParty = ch->GetParty();
			if (pParty != NULL)
			{
				if (pParty->GetMemberCount() == 2)
				{
					CPartyManager::instance().DeleteParty(pParty);
				}
				else
				{
					pParty->Quit(ch->GetPlayerID());
				}
			}
		}
		else if (memberFlag == MEMBER_NO)
		{
			if (ch->GetGMLevel() == GM_PLAYER)
				ch->WarpSet(EMPIRE_START_X(ch->GetEmpire()), EMPIRE_START_Y(ch->GetEmpire()));
		}
		else
		{
			// wtf
		}
	}
	else if (ch->GetMapIndex() == 113)
	{
		// ox 이벤트 맵
		if (COXEventManager::instance().Enter(ch) == false)
		{
			// ox 맵 진입 허가가 나지 않음. 플레이어면 마을로 보내자
			if (ch->GetGMLevel() == GM_PLAYER)
				ch->WarpSet(EMPIRE_START_X(ch->GetEmpire()), EMPIRE_START_Y(ch->GetEmpire()));
		}
	}
	else
	{
		if (CWarMapManager::instance().IsWarMap(ch->GetMapIndex()) ||
				marriage::WeddingManager::instance().IsWeddingMap(ch->GetMapIndex()))
		{
			if (!test_server)
				ch->WarpSet(EMPIRE_START_X(ch->GetEmpire()), EMPIRE_START_Y(ch->GetEmpire()));
		}
	}

	if (ch->GetHorseLevel() > 0)
	{
		DWORD pid = ch->GetPlayerID();
		if (pid != 0 && CHorseNameManager::instance().GetHorseName(pid) == NULL)
			db_clientdesc->DBPacket(HEADER_GD_REQ_HORSE_NAME, 0, &pid, sizeof(DWORD));

		ch->SetHorseLevel(ch->GetHorseLevel());
		ch->SkillLevelPacket();
	}

	if (g_noticeBattleZone) {
		if (FN_is_battle_zone(ch)) {
			ch->ChatPacketNew(CHAT_TYPE_INFO, 637, "");
			ch->ChatPacketNew(CHAT_TYPE_INFO, 638, "");
		}
	}

#if defined(ENABLE_NEW_FISH_EVENT)
	ch->FishEventGeneralInfo();
#endif
#if defined(ENABLE_LETTERS_EVENT)
	ch->SendLettersEventInfo();
#endif

#ifdef __ENABLE_NEW_OFFLINESHOP__
	if (ch->IsPC())
	{
		offlineshop::CShop* pkShop= offlineshop::GetManager().GetShopByOwnerID(ch->GetPlayerID());
		if(pkShop)
			ch->SetOfflineShop(pkShop);

		offlineshop::CAuction* auction = offlineshop::GetManager().GetAuctionByOwnerID(ch->GetPlayerID());
		if(auction)
			ch->SetAuction(auction);
	}
#endif

#ifdef __ENABLE_BLOCK_EXP__
	int expret = ch->GetQuestFlag("exp.stat");
	ch->Block_Exp = expret == 1 ? true : false;
	ch->ChatPacket(CHAT_TYPE_COMMAND, "manage_exp_status %d", ch->GetQuestFlag("exp.stat"));
#endif

#ifdef ENABLE_PVP_ADVANCED
	ch->ChatPacket(CHAT_TYPE_COMMAND, "equipview %d", ch->GetQuestFlag(BLOCK_EQUIPMENT_));
#endif
#ifdef BLOCK_RIDING_INSIDE_WAR
	if (ch->GetWarMap()) {
		if (ch->IsHorseRiding()) {
			ch->StopRiding();
			ch->HorseSummon(false);
		}
	}
#endif

#ifdef ENABLE_BIOLOGIST_UI
	if (ch->GetQuestFlag("biologist.stat") <= 13)
	{
		int biologisttime = ch->GetQuestFlag("biologist.time");
		biologisttime = biologisttime > 0 ? biologisttime : 1;
		ch->ChatPacket(CHAT_TYPE_COMMAND, "biologist_time %d", biologisttime);
	}
#endif

#ifdef USE_CAPTCHA_SYSTEM
    ch->LoadCaptchaTimer();
#endif
#if defined(ENABLE_HIDE_COSTUME_SYSTEM)
	ch->ChatPacket(CHAT_TYPE_COMMAND, "hiddenstatus "
"%d#%d"
#if defined(ENABLE_ACCE_SYSTEM)
"#%d"
#endif
#if defined(ENABLE_WEAPON_COSTUME_SYSTEM)
"#%d"
#endif
#if defined(ENABLE_AURA_SYSTEM)
"#%d"
#endif
#if defined(ENABLE_PETSKIN)
"#%d"
#endif
#if defined(ENABLE_MOUNTSKIN)
"#%d"
#endif
#if defined(ENABLE_ACCE_SYSTEM)
"#%d"
#endif
, ch->GetPartStatus(HIDE_PART_BODY), ch->GetPartStatus(HIDE_PART_HAIR)
#if defined(ENABLE_ACCE_SYSTEM)
, ch->GetPartStatus(HIDE_PART_SASH)
#endif
#if defined(ENABLE_WEAPON_COSTUME_SYSTEM)
, ch->GetPartStatus(HIDE_PART_WEAPON)
#endif
#if defined(ENABLE_AURA_SYSTEM)
, ch->GetPartStatus(HIDE_PART_AURA)
#endif
#if defined(ENABLE_PETSKIN)
, ch->GetPartStatus(HIDE_PETSKIN)
#endif
#if defined(ENABLE_MOUNTSKIN)
, ch->GetPartStatus(HIDE_MOUNTSKIN)
#endif
#if defined(ENABLE_ACCE_SYSTEM)
, ch->GetPartStatus(HIDE_STOLE_PART)
#endif
	);
#endif

/*
#if defined(ENABLE_HALLOWEEN_EVENT_2022) && defined(ENABLE_EVENT_MANAGER)
	ch->SendHalloweenEventInfo();

	if (CHARACTER_MANAGER::Instance().CheckEventIsActive(HALLOWEEN_EVENT) != NULL) {
		ch->ChatPacketNew(CHAT_TYPE_INFO, 1555, "");
	}
#endif
*/

#if defined(ENABLE_OKEY_CARD_GAME) && defined(ENABLE_EVENT_MANAGER)
	ch->SendOkeyGameEventInfo();
#endif

#ifdef USE_PICKUP_FILTER
    ch->ChatPacket(CHAT_TYPE_COMMAND, "sendftrset");
#endif
}

void CInputLogin::Empire(LPDESC d, const char * c_pData)
{
	const TPacketCGEmpire* p = reinterpret_cast<const TPacketCGEmpire*>(c_pData);

	if (p->bEmpire == 0 || p->bEmpire >= EMPIRE_MAX_NUM)
	{
		d->SetPhase(PHASE_CLOSE);
		return;
	}

	const TAccountTable& r = d->GetAccountTable();

	if (r.bEmpire != 0)
	{
		for (int i = 0; i < PLAYER_PER_ACCOUNT; ++i)
		{
			if (0 != r.players[i].dwID)
			{
				sys_err("EmpireSelectFailed %d", r.players[i].dwID);
				return;
			}
		}
	}

	TEmpireSelectPacket pd;

	pd.dwAccountID = r.id;
	pd.bEmpire = p->bEmpire;

	db_clientdesc->DBPacket(HEADER_GD_EMPIRE_SELECT, d->GetHandle(), &pd, sizeof(pd));
}

void CInputLogin::ChangeCountryFlag(LPDESC d, const char* c_pData)
{
	const auto packet = reinterpret_cast<const TPacketCGChangeCountryFlag*>(c_pData);
	const TAccountTable& account = d->GetAccountTable();

	if (!account.id)
	{
		sys_err("no account table");
		d->SetPhase(PHASE_CLOSE);
		return;
	}

	if (packet->index >= PLAYER_PER_ACCOUNT || packet->index < 0)
	{
		sys_err("index overflow %d, login: %s", packet->index, account.login);
		d->SetPhase(PHASE_CLOSE);
		return;
	}

	if (!account.players[packet->index].dwID)
	{
		sys_err("no player id, login %s", account.login);
		d->SetPhase(PHASE_CLOSE);
		return;
	}

	if (strcasecmp(packet->country_flag, account.players[packet->index].country_flag) == 0)
		return;

	if (g_country_flags.find(packet->country_flag) == g_country_flags.end())
	{
		sys_err("login %s unknown country flag %s", account.login, packet->country_flag);
		d->SetPhase(PHASE_CLOSE);
		return;
	}

	if (!PulseManager::Instance().IncreaseClock(d->GetHandle(), ePulse::ChangeCountryFlag, std::chrono::milliseconds(1500)))
		return;

	TPacketGDChangeCountryFlag pgd;
	trim_and_lower(account.login, pgd.login, sizeof(pgd.login));
	pgd.pid = account.players[packet->index].dwID;
	strlcpy(pgd.country_flag, packet->country_flag, sizeof(pgd.country_flag));

	sys_log(0, "pid %u change country flag from %s to %s", pgd.pid, account.players[packet->index].country_flag, pgd.country_flag);

	db_clientdesc->DBPacket(HEADER_GD_CHANGE_COUNTRY_FLAG, d->GetHandle(), &pgd, sizeof(pgd));
}

int CInputLogin::Analyze(LPDESC d, BYTE bHeader, const char * c_pData)
{
	int iExtraLen = 0;
	switch (bHeader)
	{
		case HEADER_CG_PONG:
			Pong(d);
			break;

		case HEADER_CG_TIME_SYNC:
			Handshake(d, c_pData);
			break;
		case HEADER_CG_LOGIN2:
			LoginByKey(d, c_pData);
			break;
		case HEADER_CG_CHARACTER_SELECT:
			CharacterSelect(d, c_pData);
			break;

		case HEADER_CG_CHARACTER_CREATE:
			CharacterCreate(d, c_pData);
			break;

		case HEADER_CG_CHARACTER_DELETE:
			CharacterDelete(d, c_pData);
			break;

		case HEADER_CG_ENTERGAME:
			Entergame(d, c_pData);
			break;

		case HEADER_CG_EMPIRE:
			Empire(d, c_pData);
			break;

		case HEADER_CG_CHANGE_COUNTRY_FLAG:
			ChangeCountryFlag(d, c_pData);
			break;

		case HEADER_CG_CLIENT_VERSION:
			Version(d->GetCharacter(), c_pData);
			break;

		case HEADER_CG_CLIENT_VERSION2:
			Version(d->GetCharacter(), c_pData);
			break;

		case HEADER_CG_CHAT:
			if ((iExtraLen = Chat(d, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		// @fixme120
		case HEADER_CG_ITEM_USE:
		case HEADER_CG_TARGET:
		case HEADER_CG_MOVE:
		case HEADER_CG_HACK:
			break;

		case HEADER_CG_SCRIPT_BUTTON:
#if defined(ENABLE_CHANGELOOK)
		case HEADER_CG_CHANGE_LOOK:
#endif
			break;
		default:
		{
			sys_err("login phase does not handle this packet! header %d", bHeader);
			d->SetPhase(PHASE_CLOSE);
			return 0;
		}
	}

	return iExtraLen;
}


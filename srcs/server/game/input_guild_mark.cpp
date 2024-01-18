#include "stdafx.h"
#include "input.h"
#include "buffer_manager.h"
#include "desc.h"
#include "guild.h"
#include "guild_manager.h"
#include "MarkImage.h"
#include "MarkManager.h"

#include <sodium.h>

int CInputGuildMark::Analyze(LPDESC d, BYTE bHeader, const char* c_pData)
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

	case HEADER_CG_SYMBOL_CRC:
		GuildSymbolCRC(d, c_pData);
		break;

	case HEADER_CG_MARK_UPLOAD:
		if ((iExtraLen = GuildMarkUpload(d, c_pData, m_iBufferLeft)) < 0)
			return -1;
		break;

	case HEADER_CG_MARK_IDXLIST:
		GuildMarkIDXList(d, c_pData);
		break;

	case HEADER_CG_MARK_CRCLIST:
		GuildMarkCRCList(d, c_pData);
		break;
	
	case HEADER_CG_MARK_LOGIN:
		break;

	default:
	{
		sys_err("guild mark phase does not handle this packet! header %d host(%s) socket(%d)", bHeader, d->GetHostName(), d->GetSocket());
		d->SetPhase(PHASE_CLOSE);
		return 0;
	}
	}
	return iExtraLen;
}

void CInputGuildMark::GuildMarkCRCList(LPDESC d, const char* c_pData)
{
	const auto pCG = reinterpret_cast<const TPacketCGMarkCRCList*>(c_pData);

	std::map<BYTE, const SGuildMarkBlock*> mapDiffBlocks;
	CGuildMarkManager::Instance().GetDiffBlocks(pCG->imgIdx, pCG->crclist, mapDiffBlocks);

	DWORD blockCount = 0;
	TEMP_BUFFER buf(1024 * 1024);

	for (const auto& [k, v] : mapDiffBlocks)
	{
		BYTE posBlock = k;
		const auto& [m_apxBuf, m_abCompBuf, m_sizeCompBuf, m_crc] = *v;

		buf.write(&posBlock, sizeof(BYTE));
		buf.write(&m_sizeCompBuf, sizeof(DWORD));
		buf.write(m_abCompBuf, m_sizeCompBuf);

		++blockCount;
	}

	TPacketGCMarkBlock pGC = {};

	pGC.header = HEADER_GC_MARK_BLOCK;
	pGC.imgIdx = pCG->imgIdx;
	pGC.bufSize = buf.size() + sizeof(TPacketGCMarkBlock);
	pGC.count = blockCount;

	sys_log(0, "MARK_SERVER: Sending blocks. (imgIdx %u diff %u size %u)", pCG->imgIdx, mapDiffBlocks.size(), pGC.bufSize);

	if (buf.size() > 0)
	{
		d->BufferedPacket(&pGC, sizeof(TPacketGCMarkBlock));
		d->LargePacket(buf.read_peek(), buf.size());
	}
	else
	{
		d->Packet(&pGC, sizeof(TPacketGCMarkBlock));
	}
}

void CInputGuildMark::GuildMarkIDXList(LPDESC d, [[maybe_unused]] const char* c_pData)
{
	const CGuildMarkManager& rkMarkMgr = CGuildMarkManager::Instance();

	const DWORD bufSize = sizeof(WORD) * 2 * rkMarkMgr.GetMarkCount();
	char* buf = nullptr;

	if (bufSize > 0)
	{
		buf = static_cast<char*>(malloc(bufSize));
		rkMarkMgr.CopyMarkIdx(buf);
	}

	TPacketGCMarkIDXList p;
	p.header = HEADER_GC_MARK_IDXLIST;
	p.bufSize = sizeof(p) + bufSize;
	p.count = rkMarkMgr.GetMarkCount();

	if (buf)
	{
		d->BufferedPacket(&p, sizeof(p));
		d->LargePacket(buf, bufSize);
		free(buf);
	}
	else
	{
		d->Packet(&p, sizeof(p));
	}

	sys_log(0, "MARK_SERVER: GuildMarkIDXList %d bytes sent.", p.bufSize);
}

int CInputGuildMark::GuildMarkUpload(LPDESC d, const char* c_pData, size_t uiBytes)
{
	const auto p = reinterpret_cast<const TPacketCGMarkUpload*>(c_pData);

	if (uiBytes < sizeof(TPacketCGMarkUpload))
		return -1;

	uiBytes -= sizeof(TPacketCGMarkUpload);

	if (uiBytes < sizeof(TPacketGuildSymbolAuth))
		return -1;

	c_pData += sizeof(TPacketCGMarkUpload);

	const auto guild = CGuildManager::Instance().FindGuild(p->gid);
	if (!guild)
	{
		d->SetPhase(PHASE_CLOSE);
		return 0;
	}

	const auto member = guild->GetMember(p->pid);
	if (!member)
	{
		sys_err("host(%s) guild_id(%u) no player by id(%u)", d->GetHostName(), p->gid, p->pid);
		d->SetPhase(PHASE_CLOSE);
		return 0;
	}

	if (!guild->HasGradeAuth(member->grade, GUILD_AUTH_NOTICE))
	{
		sys_err("[HACKER] host(%s) guild_id(%u) player_id(%u) has no permision to change the symbol", d->GetHostName(), p->gid, p->pid);
		d->SetPhase(PHASE_CLOSE);
		return 0;
	}

	const auto auth = reinterpret_cast<const TPacketGuildSymbolAuth*>(c_pData);

	uint8_t dummy[16];
	if (crypto_secretbox_open_easy(dummy, auth->ciphertext, sizeof(auth->ciphertext), auth->nonce, member->symbol_auth_key) != 0) {
		sys_err("[HACKER] pid %u gid %u guild_name(%s) host(%s)", p->pid, p->gid, guild->GetName(), d->GetHostName());
		d->SetPhase(PHASE_CLOSE);
		return 0;
	}

	guild->ClearSymbolAuthKey(p->pid);

	CGuildMarkManager& rkMarkMgr = CGuildMarkManager::Instance();

	bool isEmpty = true;

	for (DWORD iPixel = 0; iPixel < SGuildMark::SIZE; ++iPixel)
	{
		if (*((DWORD*)p->image + iPixel) != 0x00000000)
			isEmpty = false;
	}

	if (isEmpty)
		rkMarkMgr.DeleteMark(p->gid);
	else
		rkMarkMgr.SaveMark(p->gid, const_cast<uint8_t*>(p->image));

	sys_log(0, "MARK_SERVER: GuildMarkUpload: gid %u pid %u empty(%s)", p->gid, p->pid, isEmpty ? "yes" : "no");

	return sizeof(TPacketGuildSymbolAuth);
}

void CInputGuildMark::GuildSymbolCRC(LPDESC d, const char* c_pData)
{
	const auto* packet = reinterpret_cast<const TPacketCGSymbolCRC*>(c_pData);

	sys_log(0, "GuildSymbolCRC gid(%u) crc(%u) size(%u)", packet->guild_id, packet->crc, packet->size);

	const CGuildMarkManager::TGuildSymbol* symbol = CGuildMarkManager::Instance().GetGuildSymbol(packet->guild_id);

	if (!symbol)
		return;

	if (symbol->raw.size() != packet->size || symbol->crc != packet->crc)
	{
		TPacketGCGuildSymbolData GCPacket{};

		GCPacket.header = HEADER_GC_SYMBOL_DATA;
		GCPacket.size = sizeof(GCPacket) + symbol->raw.size();
		GCPacket.guild_id = packet->guild_id;

		d->BufferedPacket(&GCPacket, sizeof(GCPacket));
		d->Packet(symbol->raw.data(), symbol->raw.size());
	}
}


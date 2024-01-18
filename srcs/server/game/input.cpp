#include "stdafx.h"
#include <sstream>

#include "desc.h"
#include "desc_manager.h"
#include "char.h"
#include "buffer_manager.h"
#include "config.h"
#include "profiler.h"
#include "p2p.h"
#include "log.h"
#include "db.h"
#include "questmanager.h"
#include "login_sim.h"
#include "fishing.h"
#include "priv_manager.h"
#include "../common/CommonDefines.h"

#include "utils.h"

CInputProcessor::CInputProcessor() : m_pPacketInfo(nullptr), m_iBufferLeft(0)
{
	if (!m_pPacketInfo)
		BindPacketInfo(&m_packetInfoCG);
}

void CInputProcessor::BindPacketInfo(CPacketInfo * pPacketInfo)
{
	m_pPacketInfo = pPacketInfo;
}

bool CInputProcessor::Process(LPDESC lpDesc, const void * c_pvOrig, int iBytes, int & r_iBytesProceed)
{
	const char * c_pData = static_cast<const char*>(c_pvOrig);

	BYTE bLastHeader = 0;
	int iLastPacketLen = 0;
	int		iPacketLen;

#if defined(__IMPROVED_HANDSHAKE_PROCESS__)
	// Ignore input process if the host is an intruder
	if (lpDesc && DESC_MANAGER::instance().IsIntruder(lpDesc->GetHostName()))
	{
		// Set host close phase
		lpDesc->SetPhase(PHASE_CLOSE);
		return true;
	}
#endif

	if (!m_pPacketInfo)
	{
		sys_err("No packet info has been binded to");
		return true;
	}

	for (m_iBufferLeft = iBytes; m_iBufferLeft > 0;)
	{
		if (lpDesc->IsPhase(PHASE_CLOSE))
			return true;

		BYTE bHeader = (BYTE) *(c_pData);
		const char * c_pszName;

		if (bHeader == 0)
		{
			iPacketLen = 1;
		}
		else if (!m_pPacketInfo->Get(bHeader, &iPacketLen, &c_pszName))
		{
			sys_err("UNKNOWN HEADER: %d, LAST HEADER: %d(%d), REMAIN BYTES: %d, fd: %d",
				bHeader, bLastHeader, iLastPacketLen, m_iBufferLeft, lpDesc->GetSocket());

			//printdata((BYTE *) c_pvOrig, m_iBufferLeft);
			lpDesc->SetPhase(PHASE_CLOSE);
			return true;
		}

		if (m_iBufferLeft < iPacketLen)
			return true;


//		sys_err("header: %d ip: %s",bHeader, lpDesc->GetHostName());

		if (bHeader)
		{
			if ((test_server || packet_analyze) && bHeader != HEADER_CG_MOVE)
				sys_log(0, "Packet Analyze [Header %d][bufferLeft %d] fd (%d) ip (%s)", bHeader, m_iBufferLeft, lpDesc->GetSocket(), lpDesc->GetHostName());

			const int iExtraPacketSize = Analyze(lpDesc, bHeader, c_pData);
			if (iExtraPacketSize < 0) {
				return true;
			}

			iPacketLen += iExtraPacketSize;
		}

		if (bHeader == HEADER_CG_PONG)
		{
#if defined(ENABLE_SEQUENCE)
			sys_log(0, "PONG! %u %u", m_pPacketInfo->IsSequence(bHeader), *(BYTE*)(c_pData + iPacketLen - sizeof(BYTE)));
#else
			sys_log(0, "PONG! %u", *(BYTE*)(c_pData + iPacketLen - sizeof(BYTE)));
#endif
		}

#if defined(ENABLE_SEQUENCE)
		if (m_pPacketInfo->IsSequence(bHeader))
		{
			BYTE bSeq = lpDesc->GetSequence();
			BYTE bSeqReceived = *(BYTE *) (c_pData + iPacketLen - sizeof(BYTE));

			if (bSeq != bSeqReceived)
			{
				sys_err("SEQUENCE %x mismatch 0x%x != 0x%x header %u", get_pointer(lpDesc), bSeq, bSeqReceived, bHeader);

				LPCHARACTER	ch = lpDesc->GetCharacter();

				char buf[1024];
				int	offset, len;

				offset = snprintf(buf, sizeof(buf), "SEQUENCE_LOG [%s]-------------\n", ch ? ch->GetName() : "UNKNOWN");

				if (offset < 0 || offset >= (int) sizeof(buf))
					offset = sizeof(buf) - 1;

				for (size_t i = 0; i < lpDesc->m_seq_vector.size(); ++i)
				{
					len = snprintf(buf + offset, sizeof(buf) - offset, "\t[%03d : 0x%x]\n",
							lpDesc->m_seq_vector[i].hdr,
							lpDesc->m_seq_vector[i].seq);

					if (len < 0 || len >= (int) sizeof(buf) - offset)
						offset += (sizeof(buf) - offset) - 1;
					else
						offset += len;
				}

				snprintf(buf + offset, sizeof(buf) - offset, "\t[%03d : 0x%x]\n", bHeader, bSeq);
				sys_err("%s", buf);

				lpDesc->SetPhase(PHASE_CLOSE);
				return true;
			}
			else
			{
				lpDesc->push_seq(bHeader, bSeq);
				lpDesc->SetNextSequence();
				//sys_err("SEQUENCE %x match %u next %u header %u", lpDesc, bSeq, lpDesc->GetSequence(), bHeader);
			}
		}
#endif

		c_pData	+= iPacketLen;
		m_iBufferLeft -= iPacketLen;
		r_iBytesProceed += iPacketLen;

		iLastPacketLen = iPacketLen;
		bLastHeader = bHeader;

		if (GetType() != lpDesc->GetInputProcessor()->GetType())
			return false;
	}

	return true;
}

void CInputProcessor::Pong(LPDESC d)
{
	d->SetPong(true);
}

void CInputProcessor::Handshake(LPDESC d, const char * c_pData)
{
	TPacketCGHandshake * p = (TPacketCGHandshake *) c_pData;

	if (d->GetHandshake() != p->dwHandshake)
	{
		sys_err("Invalid Handshake on %d", d->GetSocket());
		d->SetPhase(PHASE_CLOSE);
	}
	else
	{
		if (d->IsPhase(PHASE_HANDSHAKE))
		{
			if (d->HandshakeProcess(p->dwTime, p->lDelta, false))
			{
#ifdef _IMPROVED_PACKET_ENCRYPTION_
				d->SendKeyAgreement();
#else
				// Handshaking succeeded
				if (g_bAuthServer) {
					d->SetPhase(PHASE_AUTH);
				} else {
					d->SetPhase(PHASE_LOGIN);
				}
#endif // #ifdef _IMPROVED_PACKET_ENCRYPTION_
			}
		}
		else
			d->HandshakeProcess(p->dwTime, p->lDelta, true);
	}
}

void CInputProcessor::Version(LPCHARACTER ch, const char* c_pData)
{
	if (!ch)
		return;

	TPacketCGClientVersion * p = (TPacketCGClientVersion *) c_pData;
	sys_log(0, "VERSION: %s %s %s", ch->GetName(), p->timestamp, p->filename);
	ch->GetDesc()->SetClientVersion(p->timestamp);
}

void LoginFailure(LPDESC d, const char * c_pszStatus)
{
	if (!d)
		return;

	TPacketGCLoginFailure failurePacket;

	failurePacket.header = HEADER_GC_LOGIN_FAILURE;
	strlcpy(failurePacket.szStatus, c_pszStatus, sizeof(failurePacket.szStatus));

	d->Packet(&failurePacket, sizeof(failurePacket));
}

void CInputHandshake::GuildMarkLogin(LPDESC d, const char* c_pData)
{
	if (!d)
		return;

	const auto* mark = reinterpret_cast<const TPacketCGMarkLogin*>(c_pData);
	if (d->GetHandshake() != mark->dwHandshake)
	{
		sys_err("(fd %d) host(%s) wrong guild mark login handshake", d->GetSocket(), d->GetHostName());
		d->SetPhase(PHASE_CLOSE);
		return;
	}

	d->SetPhase(PHASE_GUILD_MARK);
	sys_log(0, "GuildMarkLogin: mark id %u random_key %u handshake %u", mark->handle, mark->random_key, mark->dwHandshake);
}


int CInputProcessor::Chat(LPDESC d, const char* data, size_t uiBytes)
{
	const TPacketCGChat* pinfo = reinterpret_cast<const TPacketCGChat*>(data);

	if (uiBytes < pinfo->size)
		return -1;

	const int iExtraLen = pinfo->size - sizeof(TPacketCGChat);

	if (iExtraLen < 0 || iExtraLen > 1024)
	{
		sys_err("invalid packet length (len %d size %u buffer %u)", iExtraLen, pinfo->size, uiBytes);
		d->SetPhase(PHASE_CLOSE);
		return -1;
	}

	return iExtraLen;
}

CInputHandshake::CInputHandshake()
{
	CPacketInfoCG * pkPacketInfo = M2_NEW CPacketInfoCG;
#if defined(ENABLE_SEQUENCE)
	pkPacketInfo->SetSequence(HEADER_CG_PONG, false);
#endif

	m_pMainPacketInfo = m_pPacketInfo;
	BindPacketInfo(pkPacketInfo);
}

CInputHandshake::~CInputHandshake()
{
	if( nullptr != m_pPacketInfo )
	{
		M2_DELETE(m_pPacketInfo);
		m_pPacketInfo = nullptr;
	}
}


std::map<DWORD, CLoginSim *> g_sim;
std::map<DWORD, CLoginSim *> g_simByPID;
std::vector<TPlayerTable> g_vec_save;

int CInputHandshake::Analyze(LPDESC d, BYTE bHeader, const char * c_pData)
{
	int iExtraLen = 0;
	if (bHeader == HEADER_CG_TEXT) {
		return 0;
	}
	else if (bHeader == HEADER_CG_CHAT)
	{
		if ((iExtraLen = Chat(d, c_pData, m_iBufferLeft)) < 0)
			return -1;
		return iExtraLen;
	}
	else if (bHeader == HEADER_CG_MARK_LOGIN)
	{
		if (!guild_mark_server || g_bAuthServer)
		{
			sys_err("Guild Mark login requested but i'm not a mark server!");
			d->SetPhase(PHASE_CLOSE);
			return 0;
		}

		GuildMarkLogin(d, c_pData);
		return 0;
	}
	else if (bHeader == HEADER_CG_STATE_CHECKER)
	{
		if (d->IsChannelStatusRequested() || g_bAuthServer)
			return 0;

		d->SetChannelStatusRequested();
		db_clientdesc->DBPacket(HEADER_GD_REQUEST_CHANNELSTATUS, d->GetHandle(), nullptr, 0);
	}
	else if (bHeader == HEADER_CG_PONG)
	{
		Pong(d);
	}
	else if (bHeader == HEADER_CG_HANDSHAKE)
	{
		Handshake(d, c_pData);
	}
#ifdef _IMPROVED_PACKET_ENCRYPTION_
	else if (bHeader == HEADER_CG_KEY_AGREEMENT)
	{
		// Send out the key agreement completion packet first
		// to help client to enter encryption mode
		d->SendKeyAgreementCompleted();
		// Flush socket output before going encrypted
		d->ProcessOutput();

		TPacketKeyAgreement* p = (TPacketKeyAgreement*)c_pData;
		if (!d->IsCipherPrepared())
		{
			sys_err ("Cipher isn't prepared. %s maybe a Hacker.", inet_ntoa(d->GetAddr().sin_addr));
			d->DelayedDisconnect(5);
			return 0;
		}
		if (d->FinishHandshake(p->wAgreedLength, p->data, p->wDataLength)) {
			// Handshaking succeeded
			if (g_bAuthServer) {
				d->SetPhase(PHASE_AUTH);
			} else {
				d->SetPhase(PHASE_LOGIN);
			}
		} else {
			sys_log(0, "[CInputHandshake] Key agreement failed: al=%u dl=%u",
				p->wAgreedLength, p->wDataLength);
			d->SetPhase(PHASE_CLOSE);
		}
	}
#endif // _IMPROVED_PACKET_ENCRYPTION_
	else {
		sys_err("Handshake phase does not handle packet %d (fd %d)", bHeader, d->GetSocket());
		d->SetPhase(PHASE_CLOSE);
		return 0;
	}

	return iExtraLen;
}

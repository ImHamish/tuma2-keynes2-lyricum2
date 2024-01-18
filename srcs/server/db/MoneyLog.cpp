#include "stdafx.h"
#include "MoneyLog.h"
#include "ClientManager.h"
#include "Peer.h"


CMoneyLog::CMoneyLog()
{
}

CMoneyLog::~CMoneyLog()
{
}

void CMoneyLog::Save()
{
	CPeer* peer = CClientManager::instance().GetAnyPeer();
	if (!peer)
		return;
	for (BYTE bType = 0; bType < MONEY_LOG_TYPE_MAX_NUM; bType ++)
	{
		for (auto it = m_MoneyLogContainer[bType].begin(); it != m_MoneyLogContainer[bType].end(); ++it)
		{
			//bType;
			TPacketMoneyLog p;
			p.type = bType;
			p.vnum = it->first;
			p.gold = it->second;
			peer->EncodeHeader(HEADER_DG_MONEY_LOG, 0, sizeof(p));
			peer->Encode(&p, sizeof(p));
		}
		m_MoneyLogContainer[bType].clear();
	}
}

#if defined(ENABLE_NEW_GOLD_LIMIT)
void CMoneyLog::AddLog(BYTE bType, DWORD dwVnum, int64_t iGold)
#else
void CMoneyLog::AddLog(BYTE bType, DWORD dwVnum, signed long iGold)
#endif
{
	m_MoneyLogContainer[bType][dwVnum] += iGold;
}

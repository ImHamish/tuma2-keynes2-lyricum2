#include "stdafx.h"
#include "constants.h"
#include "config.h"
#include "input.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "protocol.h"
#include "locale_service.h"
#include "db.h"
#include "utils.h"

// #define ENABLE_ACCOUNT_W_SPECIALCHARS
bool FN_IS_VALID_LOGIN_STRING(const char *str)
{
	if (!str || !*str)
		return false;

	if (strlen(str) < 2)
		return false;

	for (const char* tmp = str; *tmp; ++tmp)
	{
		if (isdigit(*tmp) || isalpha(*tmp))
			continue;

#ifdef ENABLE_ACCOUNT_W_SPECIALCHARS
		switch (*tmp)
		{
			case ' ':
			case '_':
			case '-':
			case '.':
			case '!':
			case '@':
			case '#':
			case '$':
			case '%':
			case '^':
			case '&':
			case '*':
			case '(':
			case ')':
				continue;
		}
#endif
		return false;
	}

	return true;
}

bool Login_IsInChannelService(const char* c_login)
{
	if (c_login[0] == '[')
		return true;
	return false;
}

CInputAuth::CInputAuth()
{
}

void CInputAuth::Login(LPDESC d, const char * c_pData)
{
	TPacketCGLogin3 * pinfo = (TPacketCGLogin3 *) c_pData;
	if (!g_bAuthServer)
	{
		sys_err ("CInputAuth class is not for game server. IP %s might be a hacker.", inet_ntoa(d->GetAddr().sin_addr));
		d->DelayedDisconnect(5);
		return;
	}

#ifdef ENABLE_MULTI_LANGUAGE
    if (!pinfo->lang || pinfo->lang >= LANGUAGE_MAX_NUM)
    {
        LoginFailure(d, "SHUTDOWN");
        return;
    }
#endif

#if defined(ENABLE_NEW_ANTICHEAT_RULES)
	uint32_t dwHash = 1;
	for (size_t i = 0; i < strlen(pinfo->login); i++) {
		dwHash = (dwHash * pinfo->login[i]) % 0xFFFFFFFF;
	}

	uint32_t a = 519439225, b = 2658810181;

	uint32_t verify_hash = dwHash ^ a;
	verify_hash += b;
	verify_hash >>= 3;
	verify_hash |= 0xAA11;
	verify_hash ^= b;

	if(verify_hash != pinfo->loginHash) {
		sys_log(0, "InputAuth::Login : IS_NOT_VALID_LOGIN_HASH");
		LoginFailure(d, "SHUTDOWN");
		return;
	}
#endif

	char login[LOGIN_MAX_LEN + 1];
	trim_and_lower(pinfo->login, login, sizeof(login));

	char passwd[PASSWD_MAX_LEN + 1];
	strlcpy(passwd, pinfo->passwd, sizeof(passwd));

	sys_log(0, "InputAuth::Login : %s(%d) desc %p", login, strlen(login), get_pointer(d));

	// check login string
	if (false == FN_IS_VALID_LOGIN_STRING(login))
	{
		sys_log(0, "InputAuth::Login : IS_NOT_VALID_LOGIN_STRING(%s) desc %p", login, get_pointer(d));
		LoginFailure(d, "NOID");
		return;
	}

	if (g_bNoMoreClient)
	{
		TPacketGCLoginFailure failurePacket;

		failurePacket.header = HEADER_GC_LOGIN_FAILURE;
		strlcpy(failurePacket.szStatus, "SHUTDOWN", sizeof(failurePacket.szStatus));

		d->Packet(&failurePacket, sizeof(failurePacket));
		return;
	}

#ifdef ENABLE_ANTI_LOGIN_BRUTEFORCE
	if (!DESC_MANAGER::instance().LoginCheckByLogin(login))
	{
		LoginFailure(d, "BESAMEKEY");
		return;
	}

	if (!DESC_MANAGER::instance().LoginCheckByIP(d->GetHostName()))
	{
		LoginFailure(d, "BESAMEKEY");
		return;
	}
#endif

	if (DESC_MANAGER::instance().FindByLoginName(login))
	{
		LoginFailure(d, "ALREADY");
		return;
	}

	DWORD dwKey = DESC_MANAGER::instance().CreateLoginKey(d);
	sys_log(0, "InputAuth::Login: key (%u) login (%s)", dwKey, login);

	TPacketCGLogin3 * p = M2_NEW TPacketCGLogin3;
	memcpy(p, pinfo, sizeof(TPacketCGLogin3));

	char szPasswd[PASSWD_MAX_LEN * 2 + 1];
	DBManager::instance().EscapeString(szPasswd, sizeof(szPasswd), passwd, strlen(passwd));

	char szLogin[LOGIN_MAX_LEN * 2 + 1];
	DBManager::instance().EscapeString(szLogin, sizeof(szLogin), login, strlen(login));

	// CHANNEL_SERVICE_LOGIN
	if (Login_IsInChannelService(szLogin))
	{
		sys_log(0, "ChannelServiceLogin [%s]", szLogin);

		DBManager::instance().ReturnQuery(QID_AUTH_LOGIN, dwKey, p,
				"SELECT '%s',password,social_id,id,"
#ifdef ENABLE_MULTI_LANGUAGE
                "%u,"
#endif
                "status,availDt - NOW() > 0,"
				"UNIX_TIMESTAMP(silver_expire),"
				"UNIX_TIMESTAMP(gold_expire),"
				"UNIX_TIMESTAMP(safebox_expire),"
				"UNIX_TIMESTAMP(autoloot_expire),"
				"UNIX_TIMESTAMP(fish_mind_expire),"
				"UNIX_TIMESTAMP(marriage_fast_expire),"
				"UNIX_TIMESTAMP(money_drop_rate_expire),"
#ifdef ENABLE_VOTE4BONUS
                "UNIX_TIMESTAMP(bonus1_expire),"
                "UNIX_TIMESTAMP(bonus2_expire),"
                "UNIX_TIMESTAMP(bonus3_expire),"
#endif
#ifdef USE_AUTO_HUNT
                "UNIX_TIMESTAMP(auto_use_expire), "
#endif
				"UNIX_TIMESTAMP(create_time)"
				" FROM account WHERE login='%s'",

				szPasswd,
#ifdef ENABLE_MULTI_LANGUAGE
                pinfo->lang, 
#endif
                szLogin);
	}
	// END_OF_CHANNEL_SERVICE_LOGIN
	else
	{
		DBManager::instance().ReturnQuery(QID_AUTH_LOGIN, dwKey, p,
				"SELECT SHA1('%s'),password,social_id,id,"
#ifdef ENABLE_MULTI_LANGUAGE
                "%u,"
#endif
				"status,availDt - NOW() > 0,"
				"UNIX_TIMESTAMP(silver_expire),"
				"UNIX_TIMESTAMP(gold_expire),"
				"UNIX_TIMESTAMP(safebox_expire),"
				"UNIX_TIMESTAMP(autoloot_expire),"
				"UNIX_TIMESTAMP(fish_mind_expire),"
				"UNIX_TIMESTAMP(marriage_fast_expire),"
				"UNIX_TIMESTAMP(money_drop_rate_expire),"
#ifdef ENABLE_VOTE4BONUS
                "UNIX_TIMESTAMP(bonus1_expire), "
                "UNIX_TIMESTAMP(bonus2_expire), "
                "UNIX_TIMESTAMP(bonus3_expire), "
#endif
#ifdef USE_AUTO_HUNT
                "UNIX_TIMESTAMP(auto_use_expire), "
#endif
				"UNIX_TIMESTAMP(create_time)"
				" FROM account WHERE login='%s'",
				szPasswd,
#ifdef ENABLE_MULTI_LANGUAGE
                pinfo->lang,
#endif
                szLogin);
	}
}

extern void socket_timeout(socket_t s, long sec, long usec);

int CInputAuth::Analyze(LPDESC d, BYTE bHeader, const char * c_pData)
{
	if (!g_bAuthServer)
	{
		sys_err ("CInputAuth class is not for game server. IP %s might be a hacker.",
			inet_ntoa(d->GetAddr().sin_addr));
		d->DelayedDisconnect(5);
		return 0;
	}

	if (test_server)
		sys_log(0, " InputAuth Analyze Header[%d] ", bHeader);

	switch (bHeader) {
		case HEADER_CG_PONG:
		{
			Pong(d);
			break;
		}
		case HEADER_CG_LOGIN3:
		{
			Login(d, c_pData);
			break;
		}
		case HEADER_CG_HANDSHAKE:
		{
			break;
		}
		default:
		{
			sys_err("This phase does not handle this header %d (0x%x)(phase: AUTH)", bHeader, bHeader);
			d->SetPhase(PHASE_CLOSE);
			return 0;
		}
	}

	return 0;
}

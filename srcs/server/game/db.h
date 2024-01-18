#ifndef __INC_METIN_II_DB_MANAGER_H__
#define __INC_METIN_II_DB_MANAGER_H__

#include "../libsql/AsyncSQL.h"
#include <functional>

enum
{
	QUERY_TYPE_RETURN = 1,
	QUERY_TYPE_FUNCTION = 2,
	QUERY_TYPE_AFTER_FUNCTION = 3,
};

enum
{
	QID_SAFEBOX_SIZE,
	QID_DB_STRING,
	QID_AUTH_LOGIN,
	QID_LOTTO,
	QID_HIGHSCORE_REGISTER,
	QID_HIGHSCORE_SHOW,

	// BLOCK_CHAT
	QID_BLOCK_CHAT_LIST,
	// END_OF_BLOCK_CHAT

	// PROTECT_CHILD_FOR_NEWCIBN
	QID_PROTECT_CHILD,
	// END_PROTECT_CHILD_FOR_NEWCIBN
};

class CQueryInfo
{
	public:
		int	iQueryType;
};

class CReturnQueryInfo : public CQueryInfo
{
	public:
		int	iType;
		DWORD	dwIdent;
		void			*	pvData;
};

class CFuncQueryInfo : public CQueryInfo
{
	public:
		std::function<void(SQLMsg*)> f;
};

class CFuncAfterQueryInfo : public CQueryInfo
{
	public:
		std::function<void()> f;
};

class CLoginData;


class DBManager : public singleton<DBManager>
{
	public:
		DBManager();
		virtual ~DBManager();

		bool			IsConnected();

		bool			Connect(const char * host, const int port, const char * user, const char * pwd, const char * db);
		void			Query(const char * c_pszFormat, ...);

		SQLMsg *		DirectQuery(const char * c_pszFormat, ...);
		void			ReturnQuery(int iType, DWORD dwIdent, void* pvData, const char * c_pszFormat, ...);

		void			Process();
		void			AnalyzeReturnQuery(SQLMsg * pmsg);
#if defined(ENABLE_NEW_GOLD_LIMIT)
		void SendMoneyLog(uint8_t, uint32_t, uint64_t);
#else
		void SendMoneyLog(uint8_t, uint32_t, uint32_t);
#endif

		void			LoginPrepare(LPDESC d, DWORD * pdwClientKey, int * paiPremiumTimes = NULL);
		void			SendAuthLogin(LPDESC d);
		void			SendLoginPing(const char * c_pszLogin);

		void			InsertLoginData(CLoginData * pkLD);
		void			DeleteLoginData(CLoginData * pkLD);
		CLoginData *		GetLoginData(DWORD dwKey);

		DWORD			CountQuery()		{ return m_sql.CountQuery(); }
		DWORD			CountQueryResult()	{ return m_sql.CountResult(); }
		void			ResetQueryResult()	{ m_sql.ResetQueryFinished(); }

		void			LoadDBString();
		const std::string &	GetDBString(const std::string& key);
		const std::vector<std::string> & GetGreetMessage();

		void FuncQuery(std::function<void(SQLMsg*)> f, const char* c_pszFormat, ...);
		void FuncAfterQuery(std::function<void()> f, const char* c_pszFormat, ...);

		size_t EscapeString(char* dst, size_t dstSize, const char *src, size_t srcSize);

	private:
		SQLMsg *				PopResult();

		CAsyncSQL				m_sql;
		CAsyncSQL				m_sql_direct;
		bool					m_bIsConnect;

		std::map<std::string, std::string>	m_map_dbstring;
		std::vector<std::string>		m_vec_GreetMessage;
		std::map<DWORD, CLoginData *>		m_map_pkLoginData;

#if defined(ENABLE_VOTE4BONUS) || defined(USE_AUTO_HUNT)
    public:
        void SetPremiumByType(LPDESC pDesc, uint8_t type, int32_t time);
#endif
};

////////////////////////////////////////////////////////////////
typedef struct SHighscoreRegisterQueryInfo
{
	char    szBoard[20+1];
	DWORD   dwPID;
	int     iValue;
	bool    bOrder;
} THighscoreRegisterQueryInfo;

// ACCOUNT_DB
class AccountDB : public singleton<AccountDB>
{
	public:
		AccountDB();

		bool IsConnected();
		bool Connect(const char * host, const int port, const char * user, const char * pwd, const char * db);
		bool ConnectAsync(const char * host, const int port, const char * user, const char * pwd, const char * db, const char * locale);

		SQLMsg* DirectQuery(const char * query);
		void ReturnQuery(int iType, DWORD dwIdent, void * pvData, const char * c_pszFormat, ...);
		void AsyncQuery(const char* query);

		void SetLocale(const std::string & stLocale);

		void Process();

	private:
		SQLMsg * PopResult();
		void AnalyzeReturnQuery(SQLMsg * pMsg);

		CAsyncSQL2	m_sql_direct;
		CAsyncSQL2	m_sql;
		bool		m_IsConnect;

};
#endif

#ifndef __INC_MONEY_LOG
#define __INC_MONEY_LOG

#include <map>

class CMoneyLog : public singleton<CMoneyLog>
{
	public:
		CMoneyLog();
		virtual ~CMoneyLog();

		void Save();
#if defined(ENABLE_NEW_GOLD_LIMIT)
		void AddLog(BYTE utype, DWORD dwVnum, int64_t iGold);
#else
		void AddLog(BYTE bType, DWORD dwVnum, signed long iGold);
#endif

	private:
#if defined(ENABLE_NEW_GOLD_LIMIT)
		std::map<DWORD, int64_t> m_MoneyLogContainer[MONEY_LOG_TYPE_MAX_NUM];
#else
		std::map<DWORD, signed long> m_MoneyLogContainer[MONEY_LOG_TYPE_MAX_NUM];
#endif
};
#endif

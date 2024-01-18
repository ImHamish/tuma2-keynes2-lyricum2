// vim:ts=8 sw=4
#ifndef __INC_METIN_II_DB_LOGINDATA_H__
#define __INC_METIN_II_DB_LOGINDATA_H__

class CLoginData
{
    public:
	CLoginData();

	TAccountTable & GetAccountRef();
	void            SetClientKey(const DWORD * c_pdwClientKey);

	const DWORD *   GetClientKey();
	void            SetKey(DWORD dwKey);
	DWORD           GetKey();

	void            SetConnectedPeerHandle(DWORD dwHandle);
	DWORD		GetConnectedPeerHandle();

	void            SetLogonTime();
	DWORD		GetLogonTime();

	void		SetIP(const char * c_pszIP);
	const char *	GetIP();

	void		SetPlay(bool bOn);
	bool		IsPlay();

	void		SetDeleted(bool bSet);
	bool		IsDeleted();

	time_t		Getlastplay() { return m_lastplay; }

	void            SetPremium(int * paiPremiumTimes);
    int32_t GetPremium(uint8_t t);
	int *           GetPremiumPtr();

	DWORD		GetLastPlayerID() const { return m_dwLastPlayerID; }
	void		SetLastPlayerID(DWORD id) { m_dwLastPlayerID = id; }
	void SetLoginKeyExpired(bool bLoginKeyExpired) { m_bLoginKeyExpired = bLoginKeyExpired; }
	bool IsLoginKeyExpired() const { return m_bLoginKeyExpired; }

    private:
	bool m_bLoginKeyExpired;
	DWORD           m_dwKey;
	DWORD           m_adwClientKey[4];
	DWORD           m_dwConnectedPeerHandle;
	DWORD           m_dwLogonTime;
	char		m_szIP[MAX_HOST_LENGTH+1];
	bool		m_bPlay;
	bool		m_bDeleted;

	time_t		m_lastplay;
    int32_t m_aiPremiumTimes[PREMIUM_MAX_NUM];

	DWORD		m_dwLastPlayerID;

	TAccountTable   m_data;

#if defined(ENABLE_VOTE4BONUS) || defined(USE_AUTO_HUNT)
    public:
        void SetPremiumByType(const uint8_t type, const int32_t time);
#endif
};
#endif

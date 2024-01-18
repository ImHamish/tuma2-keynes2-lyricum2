#ifndef __INC_MESSENGER_MANAGER_H
#define __INC_MESSENGER_MANAGER_H

#include "db.h"
#if defined(ENABLE_MESSENGER_TEAM)
#include "packet.h"
#endif

class MessengerManager : public singleton<MessengerManager>
{
	public:
		typedef std::string keyT;
#if defined(ENABLE_MESSENGER_BLOCK)
		typedef std::string keyBL;
#endif
		typedef const std::string & keyA;

		MessengerManager();
		virtual ~MessengerManager();

	public:
		void	P2PLogin(keyA account);
		void	P2PLogout(keyA account);

		void	Login(keyA account);
		void	Logout(keyA account);
		void	RequestToAdd(LPCHARACTER ch, LPCHARACTER target);
		bool	AuthToAdd(keyA account, keyA companion, bool bDeny);

		void	__AddToList(keyA account, keyA companion);
		void	AddToList(keyA account, keyA companion);

		void	__RemoveFromList(keyA account, keyA companion);
		void	RemoveFromList(keyA account, keyA companion);
#if defined(ENABLE_MESSENGER_BLOCK)
		void __AddToBlockList(keyA, keyA);
		void AddToBlockList(keyA, keyA);
		bool CheckMessengerList(const std::string& ch, const std::string& tch, uint8_t type);
		void __RemoveFromBlockList(keyA, keyA);
		void RemoveFromBlockList(keyA, keyA);
		void RemoveAllBlockList(keyA);
#endif

		void	RemoveAllList(keyA account);

		void	Initialize();

	private:
		void	SendList(keyA account);
		void	SendLogin(keyA account, keyA companion);
		void	SendLogout(keyA account, keyA companion);

		void	LoadList(SQLMsg * pmsg);
#if defined(ENABLE_MESSENGER_BLOCK)
		void SendBlockLogin(keyA, keyA);
		void SendBlockLogout(keyA, keyA);
		void LoadBlockList(SQLMsg*);
		void SendBlockList(keyA);
#endif

		void	Destroy();

		std::set<keyT>			m_set_loginAccount;
		std::map<keyT, std::set<keyT> >	m_Relation;
		std::map<keyT, std::set<keyT> >	m_InverseRelation;
		std::set<DWORD>			m_set_requestToAdd;
#if defined(ENABLE_MESSENGER_BLOCK)
		std::map<keyT, std::set<keyT> > m_BlockRelation;
		std::map<keyT, std::set<keyT> > m_InverseBlockRelation;
		std::set<DWORD> m_set_requestToBlockAdd;
#endif

#if defined(ENABLE_MESSENGER_TEAM)
	public:
		void ClearTeamList();
		void PushTeamList(const char*);
		void UpdateTeamList();

	private:
		void UpdateTeam(keyA);
		void SendTeamList(keyA, bool);
		TTeamMessenger* GetTeamMember(keyA);
		std::vector<TTeamMessenger> m_vecTeamList;
#endif 
};

#endif

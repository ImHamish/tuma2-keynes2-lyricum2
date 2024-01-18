#pragma once
#if defined(ENABLE_MESSENGER_TEAM)
#include "Packet.h"
#endif

class CPythonMessenger : public CSingleton<CPythonMessenger>
{
	public:
		typedef std::set<std::string> TFriendNameMap;
		typedef std::map<std::string, BYTE> TGuildMemberStateMap;

		enum EMessengerGroupIndex
		{
			MESSENGER_GRUOP_INDEX_FRIEND,
			MESSENGER_GRUOP_INDEX_GUILD,
#if defined(ENABLE_MESSENGER_TEAM)
			MESSENGER_GROUP_INDEX_TEAM,
#endif
#if defined(ENABLE_MESSENGER_HELPER)
			MESSENGER_GROUP_INDEX_HELPER,
#endif
#if defined(ENABLE_MESSENGER_BLOCK)
			MESSENGER_GROUP_INDEX_BLOCK,
#endif
		};

	public:
		CPythonMessenger();
		virtual ~CPythonMessenger();

		void Destroy();

		// Friend
		void RemoveFriend(const char * c_szKey);
		void OnFriendLogin(const char * c_szKey);
		void OnFriendLogout(const char * c_szKey);
#if defined(ENABLE_MESSENGER_TEAM)
		void LoadTeamList(const TTeamMessenger& TeamMessenger);
		void UpdateTeam(const TTeamMessenger& TeamMessenger);
		void ClearTeamList();
#endif
#if defined(ENABLE_MESSENGER_HELPER)
		void LoadHelperList(const TTeamMessenger& TeamMessenger);
		void UpdateHelper(const TTeamMessenger& TeamMessenger);
		void ClearHelperList();
#endif
#if defined(ENABLE_MESSENGER_BLOCK)
		void RemoveBlock(const char*);
		void OnBlockLogin(const char*);
		void OnBlockLogout(const char*);
		bool IsBlockByKey(const char*);
		bool IsBlockByName(const char*);
#endif
		void SetMobile(const char * c_szKey, BYTE byState);
		BOOL IsFriendByKey(const char * c_szKey);
		BOOL IsFriendByName(const char * c_szName);

		// Guild
		void AppendGuildMember(const char * c_szName);
		void RemoveGuildMember(const char * c_szName);
		void RemoveAllGuildMember();
		void LoginGuildMember(const char * c_szName);
		void LogoutGuildMember(const char * c_szName);
		void RefreshGuildMember();
		void SetMessengerHandler(PyObject* poHandler);

	protected:
		TFriendNameMap m_FriendNameMap;
		TGuildMemberStateMap m_GuildMemberStateMap;
#if defined(ENABLE_MESSENGER_TEAM)
		std::map<std::string, bool> m_TeamListMap;
#endif
#if defined(ENABLE_MESSENGER_HELPER)
		std::map<std::string, bool> m_HelperListMap;
#endif
#if defined(ENABLE_MESSENGER_BLOCK)
		TFriendNameMap m_BlockNameMap;
#endif

	private:
		PyObject * m_poMessengerHandler;
};

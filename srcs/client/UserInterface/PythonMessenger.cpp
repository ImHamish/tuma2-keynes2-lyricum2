#include "stdafx.h"
#include "PythonMessenger.h"
#if defined(ENABLE_MESSENGER_TEAM)
#include "PythonPlayer.h"
#endif

void CPythonMessenger::RemoveFriend(const char * c_szKey)
{
	m_FriendNameMap.erase(c_szKey);
}

void CPythonMessenger::OnFriendLogin(const char * c_szKey/*, const char * c_szName*/)
{
	m_FriendNameMap.insert(c_szKey);

	if (m_poMessengerHandler)
		PyCallClassMemberFunc(m_poMessengerHandler, "OnLogin", Py_BuildValue("(is)", MESSENGER_GRUOP_INDEX_FRIEND, c_szKey));
}

void CPythonMessenger::OnFriendLogout(const char * c_szKey)
{
	m_FriendNameMap.insert(c_szKey);

	if (m_poMessengerHandler)
		PyCallClassMemberFunc(m_poMessengerHandler, "OnLogout", Py_BuildValue("(is)", MESSENGER_GRUOP_INDEX_FRIEND, c_szKey));
}

#if defined(ENABLE_MESSENGER_TEAM)
void CPythonMessenger::LoadTeamList(const TTeamMessenger& TeamMessenger)
{
#if defined(ENABLE_MESSENGER_HELPER)
	if (strstr(TeamMessenger.szName, "[H]"))
	{
		LoadHelperList(TeamMessenger);
		return;
	}
#endif

	CPythonPlayer& pyPlayer = CPythonPlayer::Instance();
	if (m_poMessengerHandler && strcmp(TeamMessenger.szName, pyPlayer.GetName()) != 0)
	{
		PyCallClassMemberFunc(m_poMessengerHandler, TeamMessenger.isConnected ?
		 "OnLogin" : "OnLogout", Py_BuildValue("(is)", MESSENGER_GROUP_INDEX_TEAM, TeamMessenger.szName));
	}

	if (m_TeamListMap.find(TeamMessenger.szName) != m_TeamListMap.end())
	{
		m_TeamListMap[TeamMessenger.szName] = TeamMessenger.isConnected;
	}
	else
	{
		m_TeamListMap.emplace(TeamMessenger.szName, TeamMessenger.isConnected);
	}
}

void CPythonMessenger::UpdateTeam(const TTeamMessenger& TeamMessenger)
{
#if defined(ENABLE_MESSENGER_HELPER)
	if (strstr(TeamMessenger.szName, "[H]"))
	{
		LoadHelperList(TeamMessenger);
		return;
	}
#endif

	if (m_poMessengerHandler)
	{
		PyCallClassMemberFunc(m_poMessengerHandler, TeamMessenger.isConnected ?
		 "OnLogin" : "OnLogout", Py_BuildValue("(is)", MESSENGER_GROUP_INDEX_TEAM, TeamMessenger.szName));
	}

	if (m_TeamListMap.find(TeamMessenger.szName) != m_TeamListMap.end())
	{
		m_TeamListMap[TeamMessenger.szName] = TeamMessenger.isConnected;
	}
	else
	{
		m_TeamListMap.emplace(TeamMessenger.szName, TeamMessenger.isConnected);
	}
}

void CPythonMessenger::ClearTeamList()
{
#if defined(ENABLE_MESSENGER_HELPER)
	ClearHelperList();
#endif

	if (m_poMessengerHandler)
	{
		PyCallClassMemberFunc(m_poMessengerHandler, "OnRemoveAllList", Py_BuildValue("(i)", MESSENGER_GROUP_INDEX_TEAM));
	}

	m_TeamListMap.clear();
}
#endif

#if defined(ENABLE_MESSENGER_HELPER)
void CPythonMessenger::LoadHelperList(const TTeamMessenger& HelperMessenger)
{
	CPythonPlayer& pyPlayer = CPythonPlayer::Instance();

	if (m_poMessengerHandler && strcmp(HelperMessenger.szName, pyPlayer.GetName()) != 0)
	{
		PyCallClassMemberFunc(m_poMessengerHandler, HelperMessenger.isConnected ?
		"OnLogin" : "OnLogout", Py_BuildValue("(is)", MESSENGER_GROUP_INDEX_HELPER, HelperMessenger.szName));
	}

	if (m_HelperListMap.find(HelperMessenger.szName) != m_HelperListMap.end())
	{
		m_HelperListMap[HelperMessenger.szName] = HelperMessenger.isConnected;
	}
	else
	{
		m_HelperListMap.emplace(HelperMessenger.szName, HelperMessenger.isConnected);
	}
}

void CPythonMessenger::UpdateHelper(const TTeamMessenger& HelperMessenger)
{
	if (m_poMessengerHandler)
	{
		PyCallClassMemberFunc(m_poMessengerHandler, HelperMessenger.isConnected ?
		 "OnLogin" : "OnLogout", Py_BuildValue("(is)", MESSENGER_GROUP_INDEX_HELPER, HelperMessenger.szName));
	}

	if (m_HelperListMap.find(HelperMessenger.szName) != m_HelperListMap.end())
	{
		m_HelperListMap[HelperMessenger.szName] = HelperMessenger.isConnected;
	}
	else
	{
		m_HelperListMap.emplace(HelperMessenger.szName, HelperMessenger.isConnected);
	}
}

void CPythonMessenger::ClearHelperList()
{
	if (m_poMessengerHandler)
	{
		PyCallClassMemberFunc(m_poMessengerHandler, "OnRemoveAllList", Py_BuildValue("(i)", MESSENGER_GROUP_INDEX_HELPER));
	}

	m_HelperListMap.clear();
}
#endif

void CPythonMessenger::SetMobile(const char * c_szKey, BYTE byState)
{
	m_FriendNameMap.insert(c_szKey);

	if (m_poMessengerHandler)
		PyCallClassMemberFunc(m_poMessengerHandler, "OnMobile", Py_BuildValue("(isi)", MESSENGER_GRUOP_INDEX_FRIEND, c_szKey, byState));
}

BOOL CPythonMessenger::IsFriendByKey(const char * c_szKey)
{
	return m_FriendNameMap.end() != m_FriendNameMap.find(c_szKey);
}

BOOL CPythonMessenger::IsFriendByName(const char * c_szName)
{
	return IsFriendByKey(c_szName);
}

void CPythonMessenger::AppendGuildMember(const char * c_szName)
{
	if (m_GuildMemberStateMap.end() != m_GuildMemberStateMap.find(c_szName))
		return;

	LogoutGuildMember(c_szName);
}

void CPythonMessenger::RemoveGuildMember(const char * c_szName)
{
	m_GuildMemberStateMap.erase(c_szName);

	if (m_poMessengerHandler)
		PyCallClassMemberFunc(m_poMessengerHandler, "OnRemoveList", Py_BuildValue("(is)", MESSENGER_GRUOP_INDEX_GUILD, c_szName));
}

void CPythonMessenger::RemoveAllGuildMember()
{
	m_GuildMemberStateMap.clear();

	if (m_poMessengerHandler)
		PyCallClassMemberFunc(m_poMessengerHandler, "OnRemoveAllList", Py_BuildValue("(i)", MESSENGER_GRUOP_INDEX_GUILD));
}

void CPythonMessenger::LoginGuildMember(const char * c_szName)
{
	m_GuildMemberStateMap[c_szName] = 1;
	if (m_poMessengerHandler)
		PyCallClassMemberFunc(m_poMessengerHandler, "OnLogin", Py_BuildValue("(is)", MESSENGER_GRUOP_INDEX_GUILD, c_szName));
}

void CPythonMessenger::LogoutGuildMember(const char * c_szName)
{
	m_GuildMemberStateMap[c_szName] = 0;
	if (m_poMessengerHandler)
		PyCallClassMemberFunc(m_poMessengerHandler, "OnLogout", Py_BuildValue("(is)", MESSENGER_GRUOP_INDEX_GUILD, c_szName));
}

void CPythonMessenger::RefreshGuildMember()
{
	for (TGuildMemberStateMap::iterator itor = m_GuildMemberStateMap.begin(); itor != m_GuildMemberStateMap.end(); ++itor)
	{
		if (itor->second)
			PyCallClassMemberFunc(m_poMessengerHandler, "OnLogin", Py_BuildValue("(is)", MESSENGER_GRUOP_INDEX_GUILD, (itor->first).c_str()));
		else
			PyCallClassMemberFunc(m_poMessengerHandler, "OnLogout", Py_BuildValue("(is)", MESSENGER_GRUOP_INDEX_GUILD, (itor->first).c_str()));
	}
}

#if defined(ENABLE_MESSENGER_BLOCK)
bool CPythonMessenger::IsBlockByKey(const char* name) {
	return (m_BlockNameMap.end() != m_BlockNameMap.find(name));
}

bool CPythonMessenger::IsBlockByName(const char* name) {
	return IsBlockByKey(name);
}

void CPythonMessenger::RemoveBlock(const char* name) {
	m_BlockNameMap.erase(name);
	PyCallClassMemberFunc(m_poMessengerHandler, "OnRemoveList", Py_BuildValue("(is)", MESSENGER_GROUP_INDEX_BLOCK, name));
}

void CPythonMessenger::OnBlockLogin(const char* name) {
	m_BlockNameMap.insert(name);
	PyCallClassMemberFunc(m_poMessengerHandler, "OnLogout", Py_BuildValue("(is)", MESSENGER_GROUP_INDEX_BLOCK, name));
}

void CPythonMessenger::OnBlockLogout(const char* name) {
	m_BlockNameMap.insert(name);
	if (m_poMessengerHandler) {
		PyCallClassMemberFunc(m_poMessengerHandler, "OnLogout", Py_BuildValue("(is)", MESSENGER_GROUP_INDEX_BLOCK, name));
	}
}
#endif

void CPythonMessenger::Destroy()
{
	m_FriendNameMap.clear();
	m_GuildMemberStateMap.clear();
#if defined(ENABLE_MESSENGER_TEAM)
	m_TeamListMap.clear();
#endif
#if defined(ENABLE_MESSENGER_HELPER)
	m_HelperListMap.clear();
#endif
#if defined(ENABLE_MESSENGER_BLOCK)
	m_BlockNameMap.clear();
#endif
}

void CPythonMessenger::SetMessengerHandler(PyObject* poHandler)
{
	m_poMessengerHandler = poHandler;
}

CPythonMessenger::CPythonMessenger()
	: m_poMessengerHandler(NULL)
{
}

CPythonMessenger::~CPythonMessenger()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////

PyObject * messengerRemoveFriend(PyObject* poSelf, PyObject* poArgs)
{
	char * szKey;
	if (!PyTuple_GetString(poArgs, 0, &szKey))
		return Py_BuildException();

	CPythonMessenger::Instance().RemoveFriend(szKey);
	return Py_BuildNone();
}

PyObject * messengerIsFriendByName(PyObject* poSelf, PyObject* poArgs)
{
	char * szName;
	if (!PyTuple_GetString(poArgs, 0, &szName))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonMessenger::Instance().IsFriendByName(szName));
}

PyObject * messengerDestroy(PyObject* poSelf, PyObject* poArgs)
{
	CPythonMessenger::Instance().Destroy();
	return Py_BuildNone();
}

PyObject * messengerRefreshGuildMember(PyObject* poSelf, PyObject* poArgs)
{
	CPythonMessenger::Instance().RefreshGuildMember();
	return Py_BuildNone();
}

PyObject * messengerSetMessengerHandler(PyObject* poSelf, PyObject* poArgs)
{
	PyObject * poEventHandler;
	if (!PyTuple_GetObject(poArgs, 0, &poEventHandler))
		return Py_BuildException();

	CPythonMessenger::Instance().SetMessengerHandler(poEventHandler);
	return Py_BuildNone();
}

#if defined(ENABLE_MESSENGER_BLOCK)
PyObject* messengerIsBlockByName(PyObject* poSelf, PyObject* poArgs) {
	char* szName;

	if (!PyTuple_GetString(poArgs, 0, &szName)) {
		return Py_BuildException();
	}

	return Py_BuildValue("i", CPythonMessenger::Instance().IsBlockByName(szName));
}

PyObject* messengerRemoveBlock(PyObject* poSelf, PyObject* poArgs) {
	char* szKey;

	if (!PyTuple_GetString(poArgs, 0, &szKey)) {
		return Py_BuildException();
	}

	CPythonMessenger::Instance().RemoveBlock(szKey);

	return Py_BuildNone();
}
#endif

void initMessenger()
{
	static PyMethodDef s_methods[] =
	{
#if defined(ENABLE_MESSENGER_BLOCK)
		{"IsBlockByName", messengerIsBlockByName, METH_VARARGS},
		{"RemoveBlock", messengerRemoveBlock, METH_VARARGS},
#endif
		{ "RemoveFriend",				messengerRemoveFriend,				METH_VARARGS },
		{ "IsFriendByName",				messengerIsFriendByName,			METH_VARARGS } ,
		{ "Destroy",					messengerDestroy,					METH_VARARGS },
		{ "RefreshGuildMember",			messengerRefreshGuildMember,		METH_VARARGS },
		{ "SetMessengerHandler",		messengerSetMessengerHandler,		METH_VARARGS },
		{ NULL,							NULL,								NULL         },
	};

	PyObject* poModule = Py_InitModule("messenger", s_methods);
	PyModule_AddIntConstant(poModule, "MESSENGER_GRUOP_INDEX_FRIEND", CPythonMessenger::MESSENGER_GRUOP_INDEX_FRIEND);
	PyModule_AddIntConstant(poModule, "MESSENGER_GRUOP_INDEX_GUILD", CPythonMessenger::MESSENGER_GRUOP_INDEX_GUILD);
#if defined(ENABLE_MESSENGER_TEAM)
	PyModule_AddIntConstant(poModule, "MESSENGER_GROUP_INDEX_TEAM", CPythonMessenger::MESSENGER_GROUP_INDEX_TEAM);
#endif
#if defined(ENABLE_MESSENGER_BLOCK)
	PyModule_AddIntConstant(poModule, "MESSENGER_GROUP_INDEX_BLOCK", CPythonMessenger::MESSENGER_GROUP_INDEX_BLOCK);
#endif
}

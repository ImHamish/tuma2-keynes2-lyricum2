#include "StdAfx.h"

#ifdef INGAME_WIKI

#include "PythonApplication.h"

CPythonWikiRenderTarget::~CPythonWikiRenderTarget() { Destroy(); }
CPythonWikiRenderTarget::CPythonWikiRenderTarget() { Destroy(); }

/*----------------------------
--------PUBLIC CLASS FUNCTIONS
-----------------------------*/

void CPythonWikiRenderTarget::Destroy() {
	_bCanRenderModules = false;
	_RenderWikiModules.clear();
}

int CPythonWikiRenderTarget::GetFreeID()
{
	const size_t render_wiki_size = _RenderWikiModules.size();
	int new_module = START_MODULE;
	
	if (!render_wiki_size)
		return new_module;
	
	for (const auto& it : _RenderWikiModules)
	{
		if (it != new_module)
			break;
		++new_module;
	}
	
	return new_module;
}

void CPythonWikiRenderTarget::RegisterRenderModule(int module_id, int module_wnd)
{
	if (module_id == DELETE_PARM || !module_wnd)
		return;

	if (!_InitializeWindow(module_id, (UI::CUiWikiRenderTarget*)module_wnd)) {
		TraceError("RegisterRenderModule Cant Regist Module ID : %d", module_id);
		return;
	}

	_RenderWikiModules.emplace_back(module_id);
}

void CPythonWikiRenderTarget::UnregisterRenderModule(int module_id, int module_wnd)
{
	if (module_id == DELETE_PARM || !module_wnd)
		return;


	CWikiRenderTargetManager::Instance().DeleteRenderTarget(module_id);
	_RenderWikiModules.erase(std::remove(_RenderWikiModules.begin(), _RenderWikiModules.end(), module_id), _RenderWikiModules.end());
}

void CPythonWikiRenderTarget::ManageModelViewVisibility(int module_id, bool flag)
{
	auto _Wrt = _GetRenderTargetHandle(module_id);
	if (!_Wrt)
		return;
	
	_Wrt->SetVisibility(flag);
}

bool CPythonWikiRenderTarget::CanRenderWikiModules() const
{
	return true;
//	bool _canRender = false;
//
//	if (_bCanRenderModules) {
//		const auto hWnd = CPythonApplication::Instance().GetWindowHandle();
//		const auto isMinimized = static_cast<bool>(IsIconic(hWnd));
//		_canRender = isMinimized ? false : true;
//	}
//
//	return _canRender;
}

void CPythonWikiRenderTarget::SetModelViewModel(int module_id, int module_vnum)
{
	auto _Wrt = _GetRenderTargetHandle(module_id);
	if (!_Wrt)
		return;
	
	_Wrt->SelectModel(module_vnum);
}

void CPythonWikiRenderTarget::SetWeaponModel(int module_id, int weapon_vnum)
{
	auto _Wrt = _GetRenderTargetHandle(module_id);
	if (!_Wrt)
		return;
	
	_Wrt->SetWeapon(weapon_vnum);
}

void CPythonWikiRenderTarget::SetModelForm(int module_id, int main_vnum)
{
	auto _Wrt = _GetRenderTargetHandle(module_id);
	if (!_Wrt)
		return;
	
	_Wrt->SetArmor(main_vnum);
}

void CPythonWikiRenderTarget::SetModelHair(int module_id, int hair_vnum)
{
	auto _Wrt = _GetRenderTargetHandle(module_id);
	if (!_Wrt)
		return;
	
	_Wrt->SetHair(hair_vnum);
}

void CPythonWikiRenderTarget::SetModelSash(int module_id, int sashVnum)
{
	auto _Wrt = _GetRenderTargetHandle(module_id);
	if (!_Wrt)
		return;
	
	_Wrt->SetSash(sashVnum);
}

#if defined(ENABLE_AURA_SYSTEM)
void CPythonWikiRenderTarget::SetModelAura(int32_t module_id, int32_t auraVnum) {
	auto _Wrt = _GetRenderTargetHandle(module_id);
	if (!_Wrt) {
		return;
	}

	_Wrt->SetAura(auraVnum);
}
#endif

void CPythonWikiRenderTarget::SetModelWeaponEffect(int module_id, int weaponeffectVnum)
{
	auto _Wrt = _GetRenderTargetHandle(module_id);
	if (!_Wrt)
		return;
	
	_Wrt->SetWeaponEffect(weaponeffectVnum);
}

void CPythonWikiRenderTarget::SetModelArmorEffect(int module_id, int armoreffectVnum)
{
	auto _Wrt = _GetRenderTargetHandle(module_id);
	if (!_Wrt)
		return;
	
	_Wrt->SetArmorEffect(armoreffectVnum);
}

void CPythonWikiRenderTarget::SetModelV3Eye(int module_id, float x, float y, float z)
{
	auto _Wrt = _GetRenderTargetHandle(module_id);
	if (!_Wrt)
		return;
	
	_Wrt->SetModelV3Eye(x, y, z);
}

void CPythonWikiRenderTarget::SetModelV3Target(int module_id, float x, float y, float z)
{
	auto _Wrt = _GetRenderTargetHandle(module_id);
	if (!_Wrt)
		return;
	
	_Wrt->SetModelV3Target(x, y, z);
}

void CPythonWikiRenderTarget::CreateBackground(int moduleID, const char* szPathName, int iWidth, int iHeight) {
	auto _Wrt = _GetRenderTargetHandle(moduleID);
	if (_Wrt)
		_Wrt->CreateBackground(szPathName, iWidth, iHeight);
}

/*----------------------------
------PROTECTED CLASS FUNCTIONS
-----------------------------*/

bool CPythonWikiRenderTarget::_InitializeWindow(int module_id, UI::CUiWikiRenderTarget* handle_window)
{
	if (!handle_window)
		return false;
	
	return handle_window->SetWikiRenderTargetModule(module_id);
}

std::shared_ptr<CWikiRenderTarget> CPythonWikiRenderTarget::_GetRenderTargetHandle(int module_id)
{
	auto it = std::find(_RenderWikiModules.begin(), _RenderWikiModules.end(), module_id);
	if (it == _RenderWikiModules.end())
		return nullptr;
	
	return CWikiRenderTargetManager::Instance().GetRenderTarget(module_id);
}
#endif

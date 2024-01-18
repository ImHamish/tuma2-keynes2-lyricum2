#pragma once
#include <functional>
#include <map>
#include <vector>

using TSkillData = struct { uint8_t bJobIndex; std::vector<const char*> vecFileName; std::vector<const char*> vecExtraFileName; };
using THorse = struct { uint8_t bJobIndex; std::vector<uint8_t> vecMotions; };
using TCacheEffect = struct { uint32_t eEffectType; const char* c_szEftAttachBone; std::string c_szEftName; bool bCache; };
using TEmoticon = struct { uint32_t eEffectType; const char* c_szEftAttachBone; std::string c_szEftName; std::string stEmoticon; };
using TSoundFileName = struct { uint8_t eItemType; const char* c_szFileName; };
using TRaceInfo = struct { std::string stFileName; std::string stFolderName; };
using TFlyEffects = struct { uint8_t dwIndex; uint8_t byType; const char* c_szFileName; };
using TSkillIndex = struct { uint8_t bJobIndex; std::map < uint8_t, std::vector<uint8_t>> mapSkillIndex; std::vector<uint8_t> vecSupportSkills; };
using TEmotion = struct { uint16_t motion_id; const char* file_name; };

extern "C" std::string _declspec(dllexport) GetHorsePathNameByMode(const uint16_t);
extern "C" void _declspec(dllexport) RegisterSkill(const DWORD, const uint8_t, const uint8_t);
extern "C" void _declspec(dllexport) LoadPythonPlayerSettingsModuleLibrary(const std::string&);
extern "C" void _declspec(dllexport) LoadGameNPC(std::vector<std::vector<std::string>>&);

extern "C" void __declspec(dllexport) PythonPlayerSettingsModuleLibRegisterVariables(const std::vector<uint8_t>&, const std::vector<uint8_t>&, const std::vector<TSkillIndex>&, const std::vector<THorse>&, const std::vector<TSkillData>&, const std::vector<TCacheEffect>&, const std::vector<TEmoticon>&, const std::vector<TRaceInfo>&, const std::vector<TSoundFileName>&, const std::vector<TSoundFileName>&, const std::vector<TFlyEffects>&, const DWORD);
extern "C" void __declspec(dllexport) PythonPlayerSettingsModuleLibRegisterFunctions(const std::function<void(DWORD, const std::string&)>&, const std::function<void(DWORD, const std::string&)>&, const std::function<void(DWORD, BYTE, const char*)>&, const std::function<void(const char*)>&, const std::function<void(DWORD, DWORD)>&, const std::function<void(DWORD, const char*, bool)>&, const std::function<void(const std::string&)>&, const std::function<void(DWORD, const char*)>&, const std::function<void(const char*, const char*)>&, const std::function<void(float)>&, const std::function<void(float)>&, const std::function<bool(UINT, const char*, const char*, bool)>&, const std::function<void(const char*)>&);

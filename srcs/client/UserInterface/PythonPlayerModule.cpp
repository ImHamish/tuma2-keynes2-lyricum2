#include "StdAfx.h"
#include "PythonPlayer.h"
#include "PythonApplication.h"
#include "../GameLib/GameLibDefines.h"

extern const DWORD c_iSkillIndex_Tongsol	= 121;
extern const DWORD c_iSkillIndex_Combo		= 122;
extern const DWORD c_iSkillIndex_Fishing	= 123;
extern const DWORD c_iSkillIndex_Mining		= 124;
extern const DWORD c_iSkillIndex_Making		= 125;
extern const DWORD c_iSkillIndex_Language1	= 126;
extern const DWORD c_iSkillIndex_Language2	= 127;
extern const DWORD c_iSkillIndex_Language3	= 128;
extern const DWORD c_iSkillIndex_Polymorph	= 129;
extern const DWORD c_iSkillIndex_Riding		= 130;
extern const DWORD c_iSkillIndex_Summon		= 131;

enum
{
	EMOTION_CLAP		= 1,
	EMOTION_CHEERS_1,
	EMOTION_CHEERS_2,
	EMOTION_DANCE_1,
	EMOTION_DANCE_2,
	EMOTION_DANCE_3,
	EMOTION_DANCE_4,
	EMOTION_DANCE_5,
	EMOTION_DANCE_6,		// 강남스타일
	EMOTION_CONGRATULATION,
	EMOTION_FORGIVE,
	EMOTION_ANGRY,

	EMOTION_ATTRACTIVE,
	EMOTION_SAD,
	EMOTION_SHY,
	EMOTION_CHEERUP,
	EMOTION_BANTER,
	EMOTION_JOY,

	EMOTION_KISS		= 51,
	EMOTION_FRENCH_KISS,
	EMOTION_SLAP,

	EMOTION_SELFIE		= 54,
	EMOTION_DANCE_7,
	EMOTION_DOZE,
	EMOTION_EXERCISE,
	EMOTION_PUSHUP,
};

std::map<int, CGraphicImage *> m_kMap_iEmotionIndex_pkIconImage;
extern int TWOHANDED_WEWAPON_ATT_SPEED_DECREASE_VALUE;


#ifdef ENABLE_FEATURES_REFINE_SYSTEM
enum
{
	REFINE_PERCENTAGE_LOW = 5,
	REFINE_PERCENTAGE_MEDIUM = 10,
	REFINE_PERCENTAGE_EXTRA = 15,

	REFINE_VNUM_POTION_LOW = 56001,
	REFINE_VNUM_POTION_MEDIUM = 56002,
	REFINE_VNUM_POTION_EXTRA = 56003,
};
#endif


#ifdef ENABLE_NEW_EQUIPMENT_SYSTEM
class CBeltInventoryHelper
{
public:
	typedef BYTE	TGradeUnit;

	static TGradeUnit GetBeltGradeByRefineLevel(int refineLevel)
	{
		static TGradeUnit beltGradeByLevelTable[] =
		{
			0,			// 벨트+0
			1,			// +1
			1,			// +2
			2,			// +3
			2,			// +4,
			3,			// +5
			4,			// +6,
			5,			// +7,
			6,			// +8,
			7,			// +9
		};

		return beltGradeByLevelTable[refineLevel];
	}

	// 현재 벨트 레벨을 기준으로, 어떤 셀들을 이용할 수 있는지 리턴
	static const TGradeUnit* GetAvailableRuleTableByGrade()
	{
		/**
			벨트는 총 +0 ~ +9 레벨을 가질 수 있으며, 레벨에 따라 7단계 등급으로 구분되어 인벤토리가 활성 화 된다.
			벨트 레벨에 따른 사용 가능한 셀은 아래 그림과 같음. 현재 등급 >= 활성가능 등급이면 사용 가능.
			(단, 현재 레벨이 0이면 무조건 사용 불가, 괄호 안의 숫자는 등급)

				2(1)  4(2)  6(4)  8(6)
				5(3)  5(3)  6(4)  8(6)
				7(5)  7(5)  7(5)  8(6)
				9(7)  9(7)  9(7)  9(7)

			벨트 인벤토리의 크기는 4x4 (16칸)
		*/

		static TGradeUnit availableRuleByGrade[c_Belt_Inventory_Slot_Count] = {
			1, 2, 4, 6,
			3, 3, 4, 6,
			5, 5, 5, 6,
			7, 7, 7, 7
		};

		return availableRuleByGrade;
	}

	static bool IsAvailableCell(WORD cell, int beltGrade /*int beltLevel*/)
	{
		// 기획 또 바뀜.. 아놔...
		//const TGradeUnit beltGrade = GetBeltGradeByRefineLevel(beltLevel);
		const TGradeUnit* ruleTable = GetAvailableRuleTableByGrade();

		return ruleTable[cell] <= beltGrade;
	}

};
#endif

PyObject* playerPickCloseItem(PyObject* poSelf, PyObject* poArgs)
{
#ifdef USE_QUICK_PICKUP
    CPythonPlayer::Instance().PickAllCloseItems();
#else
    CPythonPlayer::Instance().PickCloseItem();
#endif
    return Py_BuildNone();
}

//PyObject* playerPickCloseMoney(PyObject* poSelf, PyObject* poArgs)
//{
	//CPythonPlayer::Instance().PickCloseMoney();
	//return Py_BuildNone();
//}

PyObject* playerSetGameWindow(PyObject* poSelf, PyObject* poArgs)
{
	PyObject* pyHandle;
	if (!PyTuple_GetObject(poArgs, 0, &pyHandle))
		return Py_BadArgument();

	CPythonPlayer & rkPlayer = CPythonPlayer::Instance();
	rkPlayer.SetGameWindow(pyHandle);
	return Py_BuildNone();
}


PyObject* playerSetQuickCameraMode(PyObject* poSelf, PyObject* poArgs)
{
	int nIsEnable;
	if (!PyTuple_GetInteger(poArgs, 0, &nIsEnable))
		return Py_BadArgument();

	CPythonPlayer & rkPlayer = CPythonPlayer::Instance();
	rkPlayer.SetQuickCameraMode(nIsEnable ? true : false);

	return Py_BuildNone();
}

// Test Code
PyObject* playerSetMainCharacterIndex(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	CPythonPlayer::Instance().SetMainCharacterIndex(iVID);
	CPythonCharacterManager::Instance().SetMainInstance(iVID);

	return Py_BuildNone();
}
// Test Code

PyObject* playerGetMainCharacterIndex(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().GetMainCharacterIndex());
}

PyObject* playerGetMainCharacterName(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("s", CPythonPlayer::Instance().GetName());
}

PyObject* playerGetMainCharacterPosition(PyObject* poSelf, PyObject* poArgs)
{
	TPixelPosition kPPosMainActor;
	CPythonPlayer & rkPlayer = CPythonPlayer::Instance();
	rkPlayer.NEW_GetMainActorPosition(&kPPosMainActor);
	return Py_BuildValue("fff", kPPosMainActor.x, kPPosMainActor.y, kPPosMainActor.z);
}

PyObject* playerIsMainCharacterIndex(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonPlayer::Instance().IsMainCharacterIndex(iVID));
}

PyObject* playerCanAttackInstance(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	CInstanceBase * pMainInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();
	CInstanceBase * pTargetInstance = CPythonCharacterManager::Instance().GetInstancePtr(iVID);
	if (!pMainInstance)
		return Py_BuildValue("i", 0);
	if (!pTargetInstance)
		return Py_BuildValue("i", 0);

	return Py_BuildValue("i", pMainInstance->IsAttackableInstance(*pTargetInstance));
}

PyObject* playerIsActingEmotion(PyObject* poSelf, PyObject* poArgs)
{
	CInstanceBase * pMainInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();
	if (!pMainInstance)
		return Py_BuildValue("i", 0);

	return Py_BuildValue("i", pMainInstance->IsActingEmotion());
}

PyObject* playerIsPVPInstance(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	CInstanceBase * pMainInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();
	CInstanceBase * pTargetInstance = CPythonCharacterManager::Instance().GetInstancePtr(iVID);
	if (!pMainInstance)
		return Py_BuildValue("i", 0);
	if (!pTargetInstance)
		return Py_BuildValue("i", 0);

	return Py_BuildValue("i", pMainInstance->IsPVPInstance(*pTargetInstance));
}

PyObject* playerIsSameEmpire(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	CInstanceBase * pMainInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();
	CInstanceBase * pTargetInstance = CPythonCharacterManager::Instance().GetInstancePtr(iVID);
	if (!pMainInstance)
		return Py_BuildValue("i", FALSE);
	if (!pTargetInstance)
		return Py_BuildValue("i", FALSE);

	return Py_BuildValue("i", pMainInstance->IsSameEmpire(*pTargetInstance));
}

PyObject* playerIsChallengeInstance(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonPlayer::Instance().IsChallengeInstance(iVID));
}

PyObject* playerIsRevengeInstance(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonPlayer::Instance().IsRevengeInstance(iVID));
}

PyObject* playerIsCantFightInstance(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonPlayer::Instance().IsCantFightInstance(iVID));
}

PyObject* playerGetCharacterDistance(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	CInstanceBase * pMainInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();
	CInstanceBase * pTargetInstance = CPythonCharacterManager::Instance().GetInstancePtr(iVID);
	if (!pMainInstance)
		return Py_BuildValue("f", -1.0f);
	if (!pTargetInstance)
		return Py_BuildValue("f", -1.0f);

	return Py_BuildValue("f", pMainInstance->GetDistance(pTargetInstance));
}

PyObject* playerIsInSafeArea(PyObject* poSelf, PyObject* poArgs)
{
	CInstanceBase * pMainInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();
	if (!pMainInstance)
		return Py_BuildValue("i", FALSE);

	return Py_BuildValue("i", pMainInstance->IsInSafe());
}

PyObject* playerIsMountingHorse(PyObject* poSelf, PyObject* poArgs)
{
	CInstanceBase * pMainInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();
	if (!pMainInstance)
		return Py_BuildValue("i", FALSE);

	return Py_BuildValue("i", pMainInstance->IsMountingHorse());
}

PyObject* playerIsObserverMode(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();
	return Py_BuildValue("i", rkPlayer.IsObserverMode());
}

PyObject* playerActEmotion(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();
	rkPlayer.ActEmotion(iVID);
	return Py_BuildNone();
}

PyObject* playerShowPlayer(PyObject* poSelf, PyObject* poArgs)
{
	CInstanceBase * pMainInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();
	if (pMainInstance)
		pMainInstance->GetGraphicThingInstanceRef().Show();
	return Py_BuildNone();
}

PyObject* playerHidePlayer(PyObject* poSelf, PyObject* poArgs)
{
	CInstanceBase * pMainInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();
	if (pMainInstance)
		pMainInstance->GetGraphicThingInstanceRef().Hide();
	return Py_BuildNone();
}


PyObject* playerComboAttack(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPlayer::Instance().NEW_Attack();
	return Py_BuildNone();
}


PyObject* playerRegisterEffect(PyObject* poSelf, PyObject* poArgs)
{
	int iEft;
	if (!PyTuple_GetInteger(poArgs, 0, &iEft))
		return Py_BadArgument();

	char* szFileName;
	if (!PyTuple_GetString(poArgs, 1, &szFileName))
		return Py_BadArgument();

	CPythonPlayer & rkPlayer = CPythonPlayer::Instance();
	if (!rkPlayer.RegisterEffect(iEft, szFileName, false))
		return Py_BuildException("CPythonPlayer::RegisterEffect(eEft=%d, szFileName=%s", iEft, szFileName);

	return Py_BuildNone();
}

PyObject* playerRegisterCacheEffect(PyObject* poSelf, PyObject* poArgs)
{
	int iEft;
	if (!PyTuple_GetInteger(poArgs, 0, &iEft))
		return Py_BadArgument();

	char* szFileName;
	if (!PyTuple_GetString(poArgs, 1, &szFileName))
		return Py_BadArgument();

	CPythonPlayer & rkPlayer = CPythonPlayer::Instance();
	if (!rkPlayer.RegisterEffect(iEft, szFileName, true))
		return Py_BuildException("CPythonPlayer::RegisterEffect(eEft=%d, szFileName=%s", iEft, szFileName);

	return Py_BuildNone();
}

PyObject* playerSetAttackKeyState(PyObject* poSelf, PyObject* poArgs)
{
	int isPressed;
	if (!PyTuple_GetInteger(poArgs, 0, &isPressed))
		return Py_BuildException("playerSetAttackKeyState(isPressed) - There is no first argument");

	CPythonPlayer & rkPlayer = CPythonPlayer::Instance();
	rkPlayer.SetAttackKeyState(isPressed ? true : false);
	return Py_BuildNone();
}

PyObject* playerSetSingleDIKKeyState(PyObject* poSelf, PyObject* poArgs)
{
	int eDIK;
	if (!PyTuple_GetInteger(poArgs, 0, &eDIK))
		return Py_BuildException("playerSetSingleDIKKeyState(eDIK, isPressed) - There is no first argument");

	int isPressed;
	if (!PyTuple_GetInteger(poArgs, 1, &isPressed))
		return Py_BuildException("playerSetSingleDIKKeyState(eDIK, isPressed) - There is no second argument");

	CPythonPlayer & rkPlayer = CPythonPlayer::Instance();
	rkPlayer.NEW_SetSingleDIKKeyState(eDIK, isPressed ? true : false);
	return Py_BuildNone();
}

PyObject* playerEndKeyWalkingImmediately(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPlayer::Instance().NEW_Stop();
	return Py_BuildNone();
}


PyObject* playerStartMouseWalking(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildNone();
}

PyObject* playerEndMouseWalking(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildNone();
}

PyObject* playerResetCameraRotation(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPlayer::Instance().NEW_ResetCameraRotation();
	return Py_BuildNone();
}

PyObject* playerSetAutoCameraRotationSpeed(PyObject* poSelf, PyObject* poArgs)
{
	float fCmrRotSpd;
	if (!PyTuple_GetFloat(poArgs, 0, &fCmrRotSpd))
		return Py_BuildException();

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();
	rkPlayer.NEW_SetAutoCameraRotationSpeed(fCmrRotSpd);
	return Py_BuildNone();
}

PyObject* playerSetMouseState(PyObject* poSelf, PyObject* poArgs)
{
	int eMBT;
	if (!PyTuple_GetInteger(poArgs, 0, &eMBT))
		return Py_BuildException();

	int eMBS;
	if (!PyTuple_GetInteger(poArgs, 1, &eMBS))
		return Py_BuildException();

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();
	rkPlayer.NEW_SetMouseState(eMBT, eMBS);

	return Py_BuildNone();
}

PyObject* playerSetMouseFunc(PyObject* poSelf, PyObject* poArgs)
{
	int eMBT;
	if (!PyTuple_GetInteger(poArgs, 0, &eMBT))
		return Py_BuildException();

	int eMBS;
	if (!PyTuple_GetInteger(poArgs, 1, &eMBS))
		return Py_BuildException();

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();
	rkPlayer.NEW_SetMouseFunc(eMBT, eMBS);

	return Py_BuildNone();
}

PyObject* playerGetMouseFunc(PyObject* poSelf, PyObject* poArgs)
{
	int eMBT;
	if (!PyTuple_GetInteger(poArgs, 0, &eMBT))
		return Py_BuildException();

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();
	return Py_BuildValue("i", rkPlayer.NEW_GetMouseFunc(eMBT));
}

PyObject* playerSetMouseMiddleButtonState(PyObject* poSelf, PyObject* poArgs)
{
	int eMBS;
	if (!PyTuple_GetInteger(poArgs, 0, &eMBS))
		return Py_BuildException();

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();
	rkPlayer.NEW_SetMouseMiddleButtonState(eMBS);

	return Py_BuildNone();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

PyObject* playerGetName(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("s", CPythonPlayer::Instance().GetName());
}

PyObject* playerGetRace(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().GetRace());
}

PyObject* playerGetJob(PyObject* poSelf, PyObject* poArgs)
{
	int race = CPythonPlayer::Instance().GetRace();
	int job = RaceToJob(race);
	return Py_BuildValue("i", job);
}

PyObject* playerGetPlayTime(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().GetPlayTime());
}

PyObject* playerSetPlayTime(PyObject* poSelf, PyObject* poArgs)
{
	int iTime;
	if (!PyTuple_GetInteger(poArgs, 0, &iTime))
		return Py_BuildException();

	CPythonPlayer::Instance().SetPlayTime(iTime);
	return Py_BuildNone();
}

PyObject* playerIsSkillCoolTime(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonPlayer::Instance().IsSkillCoolTime(iSlotIndex));
}

PyObject* playerGetSkillCoolTime(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	float fCoolTime = CPythonPlayer::Instance().GetSkillCoolTime(iSlotIndex);
	float fElapsedCoolTime = CPythonPlayer::Instance().GetSkillElapsedCoolTime(iSlotIndex);
	return Py_BuildValue("ff", fCoolTime, fElapsedCoolTime);
}

PyObject* playerIsSkillActive(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonPlayer::Instance().IsSkillActive(iSlotIndex));
}

PyObject* playerUseGuildSkill(PyObject* poSelf, PyObject* poArgs)
{
	int iSkillSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSkillSlotIndex))
		return Py_BuildException();

	CPythonPlayer::Instance().UseGuildSkill(iSkillSlotIndex);
	return Py_BuildNone();
}

PyObject* playerAffectIndexToSkillIndex(PyObject* poSelf, PyObject* poArgs)
{
	int iAffectIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iAffectIndex))
		return Py_BuildException();

	DWORD dwSkillIndex;
	if (!CPythonPlayer::Instance().AffectIndexToSkillIndex(iAffectIndex, &dwSkillIndex))
		return Py_BuildValue("i", 0);

	return Py_BuildValue("i", dwSkillIndex);
}

PyObject* playerGetEXP(PyObject* poSelf, PyObject* poArgs)
{
	DWORD dwEXP = CPythonPlayer::Instance().GetStatus(POINT_EXP);
	return Py_BuildValue("l", dwEXP);
}

PyObject* playerGetStatus(PyObject* poSelf, PyObject* poArgs)
{
	int iType;
	if (!PyTuple_GetInteger(poArgs, 0, &iType))
		return Py_BuildException();

	long iValue = CPythonPlayer::Instance().GetStatus(iType);

	if (POINT_ATT_SPEED == iType)
	{
		CInstanceBase * pInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();
		if (pInstance && (CItemData::WEAPON_TWO_HANDED == pInstance->GetWeaponType()))
		{
			iValue -= TWOHANDED_WEWAPON_ATT_SPEED_DECREASE_VALUE;
		}
	}

	return Py_BuildValue("i", iValue);
}

PyObject* playerSetStatus(PyObject* poSelf, PyObject* poArgs)
{
	int iType;
	if (!PyTuple_GetInteger(poArgs, 0, &iType))
		return Py_BuildException();

	int iValue;
	if (!PyTuple_GetInteger(poArgs, 1, &iValue))
		return Py_BuildException();

	CPythonPlayer::Instance().SetStatus(iType, iValue);
	return Py_BuildNone();
}
#if defined(ENABLE_NEW_GOLD_LIMIT)
PyObject* playerGetElk(PyObject* poSelf, PyObject* poArgs)
{
	return PyLong_FromLongLong(CPythonPlayer::Instance().GetGold());
}
#else
PyObject* playerGetElk(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().GetStatus(POINT_GOLD));
}
#endif

PyObject* playerGetGuildID(PyObject* poSelf, PyObject* poArgs)
{
	CInstanceBase * pInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();
	if (!pInstance)
		return Py_BuildValue("i", 0);
	return Py_BuildValue("i", pInstance->GetGuildID());
}

PyObject* playerGetGuildName(PyObject* poSelf, PyObject* poArgs)
{
	CInstanceBase * pInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();
	if (pInstance)
	{
		DWORD dwID = pInstance->GetGuildID();
		std::string strName;
		if (CPythonGuild::Instance().GetGuildName(dwID, &strName))
			return Py_BuildValue("s", strName.c_str());
	}
	return Py_BuildValue("s", "");
}

PyObject* playerGetAlignmentData(PyObject* poSelf, PyObject* poArgs)
{
	CInstanceBase * pInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();
	int iAlignmentPoint = 0;
	int iAlignmentGrade = 4;
	if (pInstance)
	{
		iAlignmentPoint = pInstance->GetAlignment();
		iAlignmentGrade = pInstance->GetAlignmentGrade();
	}
	return Py_BuildValue("ii", iAlignmentPoint, iAlignmentGrade);
}

PyObject* playerSetSkill(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	int iSkillIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSkillIndex))
		return Py_BuildException();

	CPythonPlayer::Instance().SetSkill(iSlotIndex, iSkillIndex);
	return Py_BuildNone();
}

PyObject* playerGetSkillIndex(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonPlayer::Instance().GetSkillIndex(iSlotIndex));
}

PyObject* playerGetSkillSlotIndex(PyObject* poSelf, PyObject* poArgs)
{
	int iSkillIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSkillIndex))
		return Py_BuildException();

	DWORD dwSlotIndex;
	if (!CPythonPlayer::Instance().GetSkillSlotIndex(iSkillIndex, &dwSlotIndex))
		return Py_BuildException();

	return Py_BuildValue("i", dwSlotIndex);
}

PyObject* playerGetSkillGrade(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonPlayer::Instance().GetSkillGrade(iSlotIndex));
}

PyObject* playerGetSkillLevel(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonPlayer::Instance().GetSkillLevel(iSlotIndex));
}

PyObject* playerGetSkillCurrentEfficientPercentage(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	return Py_BuildValue("f", CPythonPlayer::Instance().GetSkillCurrentEfficientPercentage(iSlotIndex));
}
PyObject* playerGetSkillNextEfficientPercentage(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	return Py_BuildValue("f", CPythonPlayer::Instance().GetSkillNextEfficientPercentage(iSlotIndex));
}


PyObject* playerClickSkillSlot(PyObject* poSelf, PyObject* poArgs)
{
	int iSkillSlot;
	if (!PyTuple_GetInteger(poArgs, 0, &iSkillSlot))
		return Py_BadArgument();

	CPythonPlayer::Instance().ClickSkillSlot(iSkillSlot);

	return Py_BuildNone();
}

PyObject* playerChangeCurrentSkillNumberOnly(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BadArgument();

	CPythonPlayer::Instance().ChangeCurrentSkillNumberOnly(iSlotIndex);

	return Py_BuildNone();
}

PyObject* playerClearSkillDict(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPlayer::Instance().ClearSkillDict();
	return Py_BuildNone();
}

PyObject* playerMoveItem(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos srcCell;
	TItemPos dstCell;
	switch (PyTuple_Size(poArgs))
	{
	case 2:
		// int iSourceSlotIndex;
		if (!PyTuple_GetInteger(poArgs, 0, &srcCell.cell))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &dstCell.cell))
			return Py_BuildException();
		break;
	case 4:
		if (!PyTuple_GetByte(poArgs, 0, &srcCell.window_type))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &srcCell.cell))
			return Py_BuildException();
		if (!PyTuple_GetByte(poArgs, 2, &dstCell.window_type))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 3, &dstCell.cell))
			return Py_BuildException();
	default:
		return Py_BuildException();
	}

	CPythonPlayer::Instance().MoveItemData(srcCell, dstCell);
	return Py_BuildNone();
}

PyObject* playerSendClickItemPacket(PyObject* poSelf, PyObject* poArgs)
{
	int ivid;
	if (!PyTuple_GetInteger(poArgs, 0, &ivid))
		return Py_BuildException();

	CPythonPlayer::Instance().SendClickItemPacket(ivid);
	return Py_BuildNone();
}

PyObject* playerGetItemIndex(PyObject* poSelf, PyObject* poArgs)
{
	switch (PyTuple_Size(poArgs))
	{
	case 1:
		{
			int iSlotIndex;
			if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
				return Py_BuildException();

			int ItemIndex = CPythonPlayer::Instance().GetItemIndex(TItemPos (INVENTORY, iSlotIndex));
			return Py_BuildValue("i", ItemIndex);
		}
	case 2:
		{
			TItemPos Cell;
			if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
				return Py_BuildException();
			if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
				return Py_BuildException();

			int ItemIndex = CPythonPlayer::Instance().GetItemIndex(Cell);
			return Py_BuildValue("i", ItemIndex);
		}
	default:
		return Py_BuildException();

	}
}

PyObject* playerGetItemFlags(PyObject* poSelf, PyObject* poArgs)
{
	switch (PyTuple_Size(poArgs))
	{
	case 1:
		{
			int iSlotIndex;
			if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
				return Py_BuildException();

			DWORD flags = CPythonPlayer::Instance().GetItemFlags(TItemPos(INVENTORY, iSlotIndex));
			return Py_BuildValue("i", flags);
		}
	case 2:
		{
			TItemPos Cell;
			if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
				return Py_BuildException();

			if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
				return Py_BuildException();

			DWORD flags = CPythonPlayer::Instance().GetItemFlags(Cell);
			return Py_BuildValue("i", flags);
		}
	default:
		return Py_BuildException();
	}
}


PyObject* playerGetItemCount(PyObject* poSelf, PyObject* poArgs)
{
	switch (PyTuple_Size(poArgs))
	{
	case 1:
		{
			int iSlotIndex;
			if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
				return Py_BuildException();

			int ItemNum = CPythonPlayer::Instance().GetItemCount(TItemPos (INVENTORY, iSlotIndex));
			return Py_BuildValue("i", ItemNum);
		}
	case 2:
		{
			TItemPos Cell;
			if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
				return Py_BuildException();

			if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
				return Py_BuildException();

			int ItemNum = CPythonPlayer::Instance().GetItemCount(Cell);

			return Py_BuildValue("i", ItemNum);
		}
	default:
		return Py_BuildException();

	}
}

PyObject* playerSetItemCount(PyObject* poSelf, PyObject* poArgs)
{
	switch (PyTuple_Size(poArgs))
	{
	case 2:
		{
			int iSlotIndex;
			if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
				return Py_BuildException();
			
			WORD bCount;

			if (!PyTuple_GetInteger(poArgs, 1, &bCount))
				return Py_BuildException();

			if (0 == bCount)
				return Py_BuildException();

			CPythonPlayer::Instance().SetItemCount(TItemPos (INVENTORY, iSlotIndex), bCount);
			return Py_BuildNone();
		}
	case 3:
		{
			TItemPos Cell;
			if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
				return Py_BuildException();

			if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
				return Py_BuildException();

			WORD bCount;

			if (!PyTuple_GetInteger(poArgs, 2, &bCount))
				return Py_BuildException();

			CPythonPlayer::Instance().SetItemCount(Cell, bCount);

			return Py_BuildNone();
		}
	default:
		return Py_BuildException();

	}
}

PyObject* playerGetItemCountByVnum(PyObject* poSelf, PyObject* poArgs)
{
	int ivnum;
	if (!PyTuple_GetInteger(poArgs, 0, &ivnum))
		return Py_BuildException();

	int ItemNum = CPythonPlayer::Instance().GetItemCountByVnum(ivnum);
	return Py_BuildValue("i", ItemNum);
}

#ifdef ENABLE_EXTRA_INVENTORY
//To fix search item in refine (0)
PyObject* playerGetItemCountByVnumExtra(PyObject* poSelf, PyObject* poArgs)
{
	int ivnum;
	if (!PyTuple_GetInteger(poArgs, 0, &ivnum))
		return Py_BuildException();

	int ItemNum = CPythonPlayer::Instance().GetItemCountbyVnumExtraInventory(ivnum);
	return Py_BuildValue("i", ItemNum);
}
#endif


PyObject* playerGetItemMetinSocket(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos Cell;
	int iMetinSocketIndex;

	switch (PyTuple_Size(poArgs))
	{
	case 2:
		if (!PyTuple_GetInteger(poArgs, 0, &Cell.cell))
			return Py_BuildException();

		if (!PyTuple_GetInteger(poArgs, 1, &iMetinSocketIndex))
			return Py_BuildException();

		break;
	case 3:
		if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 2, &iMetinSocketIndex))
			return Py_BuildException();

		break;

	default:
		return Py_BuildException();

	}
	int nMetinSocketValue = CPythonPlayer::Instance().GetItemMetinSocket(Cell, iMetinSocketIndex);
	return Py_BuildValue("i", nMetinSocketValue);
}

PyObject* playerGetItemAttribute(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos Cell;
	// int iSlotPos;
	int iAttributeSlotIndex;
	switch (PyTuple_Size(poArgs))
	{
	case 2:
		if (!PyTuple_GetInteger(poArgs, 0, &Cell.cell))
			return Py_BuildException();

		if (!PyTuple_GetInteger(poArgs, 1, &iAttributeSlotIndex))
			return Py_BuildException();

		break;
	case 3:
		if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
			return Py_BuildException();

		if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
			return Py_BuildException();

		if (!PyTuple_GetInteger(poArgs, 2, &iAttributeSlotIndex))
			return Py_BuildException();
		break;
	default:
		return Py_BuildException();
	}
	BYTE byType;
	short sValue;
	CPythonPlayer::Instance().GetItemAttribute(Cell, iAttributeSlotIndex, &byType, &sValue);

	return Py_BuildValue("ii", byType, sValue);
}

PyObject* playerGetItemLink(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos Cell;

	switch (PyTuple_Size(poArgs))
	{
	case 1:
		if (!PyTuple_GetInteger(poArgs, 0, &Cell.cell))
			return Py_BuildException();
		break;
	case 2:
		if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
			return Py_BuildException();
		break;
	default:
		return Py_BuildException();
	}
	const TItemData * pPlayerItem = CPythonPlayer::Instance().GetItemData(Cell);
	CItemData * pItemData = NULL;
	char buf[1024];

	if (pPlayerItem && CItemManager::Instance().GetItemDataPointer(pPlayerItem->vnum, &pItemData))
	{
		char itemlink[256];
		bool isAttr = false;

		int32_t len = snprintf(itemlink, sizeof(itemlink), "item:%x:%x", pPlayerItem->vnum, pPlayerItem->flags);

		for (uint8_t i = 0; i <= ITEM_SOCKET_SLOT_MAX_NUM; i++)
		{
			len += snprintf(itemlink + len, sizeof(itemlink) - len, ":%x", pPlayerItem->alSockets[i]);
		}

#if defined(ENABLE_MULTI_LANGUAGE)
		len += snprintf(itemlink + len, sizeof(itemlink) - len, ":[{%d}]", pPlayerItem->vnum);
#endif

#if defined(ENABLE_CHANGELOOK)
		len += snprintf(itemlink + len, sizeof(itemlink) - len, ":%x", pPlayerItem->transmutation);
#endif

		for (uint8_t i = 0; i < ITEM_ATTRIBUTE_SLOT_MAX_NUM; i++) {
			len += snprintf(itemlink + len, sizeof(itemlink) - len, ":%x:%d", pPlayerItem->aAttr[i].bType, pPlayerItem->aAttr[i].sValue);
			if (!isAttr && pPlayerItem->aAttr[i].bType != 0) {
				isAttr = true;
			}
		}

#ifdef ENABLE_MULTI_LANGUAGE
		if (GetDefaultCodePage() == CP_ARABIC) {
			if (isAttr) {
				snprintf(buf, sizeof(buf), " |h|r[%s]|cffffc700|H%s|h", pItemData->GetName(), itemlink);
			} else {
				snprintf(buf, sizeof(buf), " |h|r[%s]|cfff1e6c0|H%s|h", pItemData->GetName(), itemlink);
			}
		} else {
			if (isAttr) {
				snprintf(buf, sizeof(buf), "|cffffc700|H%s|h[- %s -]|h|r", itemlink, pItemData->GetName());
			} else {
				snprintf(buf, sizeof(buf), "|cfff1e6c0|H%s|h[- %s -]|h|r", itemlink, pItemData->GetName());
			}
		}
#else
		if (GetDefaultCodePage() == CP_ARABIC) {
			if (isAttr)
				snprintf(buf, sizeof(buf), " |h|r[%s]|cffffc700|H%s|h", pItemData->GetName(), itemlink);
			else
				snprintf(buf, sizeof(buf), " |h|r[%s]|cfff1e6c0|H%s|h", pItemData->GetName(), itemlink);
		}
		else {
			if (isAttr)
				snprintf(buf, sizeof(buf), "|cffffc700|H%s|h[%s]|h|r", itemlink, pItemData->GetName());
			else
				snprintf(buf, sizeof(buf), "|cfff1e6c0|H%s|h[%s]|h|r", itemlink, pItemData->GetName());
		}
#endif
	}
	else
		buf[0] = '\0';

	return Py_BuildValue("s", buf);
}

PyObject* playerGetISellItemPrice(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos Cell;

	switch (PyTuple_Size(poArgs))
	{
	case 1:
		if (!PyTuple_GetInteger(poArgs, 0, &Cell.cell))
			return Py_BuildException();
		break;
	case 2:
		if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
			return Py_BuildException();
		break;
	default:
		return Py_BuildException();
	}
	CItemData * pItemData;

	if (!CItemManager::Instance().GetItemDataPointer(CPythonPlayer::Instance().GetItemIndex(Cell), &pItemData))
		return Py_BuildValue("i", 0);

	int iPrice;

	if (pItemData->IsFlag(CItemData::ITEM_FLAG_COUNT_PER_1GOLD))
		iPrice = CPythonPlayer::Instance().GetItemCount(Cell) / pItemData->GetISellItemPrice();
	else
		iPrice = pItemData->GetISellItemPrice() * CPythonPlayer::Instance().GetItemCount(Cell);

	iPrice /= 5;
	return Py_BuildValue("i", iPrice);
}

PyObject* playerGetQuickPage(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().GetQuickPage());
}

PyObject* playerSetQuickPage(PyObject* poSelf, PyObject* poArgs)
{
	int iPageIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iPageIndex))
		return Py_BuildException();

	CPythonPlayer::Instance().SetQuickPage(iPageIndex);
	return Py_BuildNone();
}

PyObject* playerLocalQuickSlotIndexToGlobalQuickSlotIndex(PyObject* poSelf, PyObject* poArgs)
{
	int32_t iLocalSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iLocalSlotIndex)) {
		return Py_BuildException();
	}

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();
	return Py_BuildValue("i", rkPlayer.LocalQuickSlotIndexToGlobalQuickSlotIndex(iLocalSlotIndex));
}


PyObject* playerGetLocalQuickSlot(PyObject* poSelf, PyObject* poArgs)
{
	int32_t iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex)) {
		return Py_BuildException();
	}

	uint8_t dwWndType;
	uint16_t dwWndItemPos;

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();
	rkPlayer.GetLocalQuickSlotData(iSlotIndex, &dwWndType, &dwWndItemPos);

	return Py_BuildValue("ii", dwWndType, dwWndItemPos);
}

PyObject* playerGetGlobalQuickSlot(PyObject* poSelf, PyObject* poArgs)
{
	int32_t iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex)) {
		return Py_BuildException();
	}

	uint8_t dwWndType;
	uint16_t dwWndItemPos;

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();
	rkPlayer.GetGlobalQuickSlotData(iSlotIndex, &dwWndType, &dwWndItemPos);

	return Py_BuildValue("ii", dwWndType, dwWndItemPos);
}


PyObject* playerRequestAddLocalQuickSlot(PyObject* poSelf, PyObject* poArgs)
{
	int32_t nSlotIndex;

	if (!PyTuple_GetInteger(poArgs, 0, &nSlotIndex)) {
		return Py_BuildException();
	}

	int32_t nWndType;
	if (!PyTuple_GetInteger(poArgs, 1, &nWndType)) {
		return Py_BuildException();
	}

	int32_t nWndItemPos;
	if (!PyTuple_GetInteger(poArgs, 2, &nWndItemPos)) {
		return Py_BuildException();
	}

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();
	rkPlayer.RequestAddLocalQuickSlot(nSlotIndex, static_cast<uint8_t>(nWndType), static_cast<uint16_t>(nWndItemPos));

	return Py_BuildNone();
}

PyObject* playerRequestAddToEmptyLocalQuickSlot(PyObject* poSelf, PyObject* poArgs)
{
	int32_t nWndType;
	if (!PyTuple_GetInteger(poArgs, 0, &nWndType)) {
		return Py_BuildException();
	}

	int32_t nWndItemPos;
	if (!PyTuple_GetInteger(poArgs, 1, &nWndItemPos)) {
		return Py_BuildException();
	}

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();
	rkPlayer.RequestAddToEmptyLocalQuickSlot(static_cast<uint8_t>(nWndType), static_cast<uint16_t>(nWndItemPos));

	return Py_BuildNone();
}

PyObject* playerRequestDeleteGlobalQuickSlot(PyObject* poSelf, PyObject* poArgs)
{
	int32_t nGlobalSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &nGlobalSlotIndex))
	{
		return Py_BuildException();
	}

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();
	rkPlayer.RequestDeleteGlobalQuickSlot(nGlobalSlotIndex);
	return Py_BuildNone();
}

PyObject* playerRequestMoveGlobalQuickSlotToLocalQuickSlot(PyObject* poSelf, PyObject* poArgs)
{
	int32_t nGlobalSrcSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &nGlobalSrcSlotIndex)) {
		return Py_BuildException();
	}

	int32_t nLocalDstSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &nLocalDstSlotIndex)) {
		return Py_BuildException();
	}

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();
	rkPlayer.RequestMoveGlobalQuickSlotToLocalQuickSlot(nGlobalSrcSlotIndex, nLocalDstSlotIndex);
	return Py_BuildNone();
}

PyObject* playerRequestUseLocalQuickSlot(PyObject* poSelf, PyObject* poArgs)
{
	int32_t iLocalPosition;
	if (!PyTuple_GetInteger(poArgs, 0, &iLocalPosition)) {
		return Py_BuildException();
	}

	CPythonPlayer::Instance().RequestUseLocalQuickSlot(iLocalPosition);

	return Py_BuildNone();
}

PyObject* playerRemoveQuickSlotByValue(PyObject* poSelf, PyObject* poArgs)
{
	int32_t iType;
	if (!PyTuple_GetInteger(poArgs, 0, &iType)) {
		return Py_BuildException();
	}

	int32_t iPosition;
	if (!PyTuple_GetInteger(poArgs, 1, &iPosition)) {
		return Py_BuildException();
	}

	CPythonPlayer::Instance().RemoveQuickSlotByValue(iType, iPosition);

	return Py_BuildNone();
}

PyObject* playerisItem(PyObject* poSelf, PyObject* poArgs)
{
	int32_t iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
	{
		return Py_BuildException();
	}

	char Flag = CPythonPlayer::Instance().IsItem(TItemPos(INVENTORY, iSlotIndex));

	return Py_BuildValue("i", Flag);
}

#ifdef ENABLE_NEW_EQUIPMENT_SYSTEM
PyObject* playerIsBeltInventorySlot(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	char Flag = CPythonPlayer::Instance().IsBeltInventorySlot(TItemPos(INVENTORY, iSlotIndex));

	return Py_BuildValue("i", Flag);
}
#endif

PyObject* playerIsEquipmentSlot(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	if (iSlotIndex >= c_Equipment_Start)
	if (iSlotIndex <= c_DragonSoul_Equip_End)
		return Py_BuildValue("i", 1);

	return Py_BuildValue("i", 0);
}

PyObject* playerIsDSEquipmentSlot(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bWindowType;
	if (!PyTuple_GetInteger(poArgs, 0, &bWindowType))
		return Py_BuildException();
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	if (INVENTORY == bWindowType)
	if (iSlotIndex >= c_DragonSoul_Equip_Start)
	if (iSlotIndex <= c_DragonSoul_Equip_End)
		return Py_BuildValue("i", 1);

	return Py_BuildValue("i", 0);

}

PyObject* playerIsCostumeSlot(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

#ifdef ENABLE_COSTUME_SYSTEM
	if (iSlotIndex >= c_Costume_Slot_Start)
	if (iSlotIndex <= c_Costume_Slot_End)
		return Py_BuildValue("i", 1);
#endif

	return Py_BuildValue("i", 0);
}

PyObject* playerIsOpenPrivateShop(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().IsOpenPrivateShop());
}

PyObject* playerIsValuableItem(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos SlotIndex;

	switch (PyTuple_Size (poArgs))
	{
	case 1:
		if (!PyTuple_GetInteger(poArgs, 0, &SlotIndex.cell))
			return Py_BuildException();
		break;
	case 2:
		if (!PyTuple_GetInteger(poArgs, 0, &SlotIndex.window_type))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &SlotIndex.cell)) // @fixme013 arg idx from 0 to 1
			return Py_BuildException();
		break;
	default:
		return Py_BuildException();
	}

	DWORD dwItemIndex = CPythonPlayer::Instance().GetItemIndex(SlotIndex);
	CItemManager::Instance().SelectItemData(dwItemIndex);
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Can't find item data");

	BOOL hasMetinSocket = FALSE;
	BOOL isHighPrice = FALSE;

	for (int i = 0; i < METIN_SOCKET_COUNT; ++i)
		if (CPythonPlayer::METIN_SOCKET_TYPE_NONE != CPythonPlayer::Instance().GetItemMetinSocket(SlotIndex, i))
			hasMetinSocket = TRUE;

	DWORD dwValue = pItemData->GetISellItemPrice();
	if (dwValue > 0)
		isHighPrice = TRUE;

	return Py_BuildValue("i", hasMetinSocket || isHighPrice);
}

int GetItemGrade(const char * c_szItemName)
{
	std::string strName = c_szItemName;
	if (strName.empty())
		return 0;

	char chGrade = strName[strName.length() - 1];
	if (chGrade < '0' || chGrade > '9')
		chGrade = '0';

	int iGrade = chGrade - '0';
	return iGrade;
}

PyObject* playerGetItemGrade(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos SlotIndex;
	switch (PyTuple_Size(poArgs))
	{
	case 1:
		if (!PyTuple_GetInteger(poArgs, 0, &SlotIndex.cell))
			return Py_BuildException();
		break;
	case 2:
		if (!PyTuple_GetInteger(poArgs, 0, &SlotIndex.window_type))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &SlotIndex.cell))
			return Py_BuildException();
		break;
	default:
		return Py_BuildException();
	}

	int iItemIndex = CPythonPlayer::Instance().GetItemIndex(SlotIndex);
	CItemManager::Instance().SelectItemData(iItemIndex);
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Can't find item data");

	return Py_BuildValue("i", GetItemGrade(pItemData->GetName()));
}

#if defined(GAIDEN)
PyObject* playerGetItemUnbindTime(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	return Py_BuildValue("i", (int) CPythonPlayer::instance().GetItemUnbindTime(iSlotIndex));
}
#endif

enum
{
	REFINE_SCROLL_TYPE_MAKE_SOCKET = 1,
	REFINE_SCROLL_TYPE_UP_GRADE = 2,
};

enum
{
	REFINE_CANT,
	REFINE_OK,
	REFINE_ALREADY_MAX_SOCKET_COUNT,
	REFINE_NEED_MORE_GOOD_SCROLL,
	REFINE_CANT_MAKE_SOCKET_ITEM,
	REFINE_NOT_NEXT_GRADE_ITEM,
	REFINE_CANT_REFINE_METIN_TO_EQUIPMENT,
	REFINE_CANT_REFINE_ROD,
};

PyObject* playerCanRefine(PyObject* poSelf, PyObject* poArgs)
{
	int iScrollItemIndex;
	TItemPos TargetSlotIndex;

	switch (PyTuple_Size(poArgs))
	{
	case 2:
		if (!PyTuple_GetInteger(poArgs, 0, &iScrollItemIndex))
			return Py_BadArgument();
		if (!PyTuple_GetInteger(poArgs, 1, &TargetSlotIndex.cell))
			return Py_BadArgument();
		break;
	case 3:
		if (!PyTuple_GetInteger(poArgs, 0, &iScrollItemIndex))
			return Py_BadArgument();
		if (!PyTuple_GetInteger(poArgs, 1, &TargetSlotIndex.window_type))
			return Py_BadArgument();
		if (!PyTuple_GetInteger(poArgs, 2, &TargetSlotIndex.cell))
			return Py_BadArgument();
		break;
	default:
		return Py_BadArgument();
	}

	if (CPythonPlayer::Instance().IsEquipmentSlot(TargetSlotIndex))
	{
		return Py_BuildValue("i", REFINE_CANT_REFINE_METIN_TO_EQUIPMENT);
	}

	// Scroll
	CItemManager::Instance().SelectItemData(iScrollItemIndex);
	CItemData * pScrollItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pScrollItemData)
		return Py_BuildValue("i", REFINE_CANT);
	int iScrollType = pScrollItemData->GetType();
	int iScrollSubType = pScrollItemData->GetSubType();
	if (iScrollType != CItemData::ITEM_TYPE_USE)
		return Py_BuildValue("i", REFINE_CANT);
	if (iScrollSubType != CItemData::USE_TUNING)
		return Py_BuildValue("i", REFINE_CANT);

	// Target Item
	int iTargetItemIndex = CPythonPlayer::Instance().GetItemIndex(TargetSlotIndex);
	CItemManager::Instance().SelectItemData(iTargetItemIndex);
	CItemData * pTargetItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pTargetItemData)
		return Py_BuildValue("i", REFINE_CANT);

	int iTargetType = pTargetItemData->GetType();
	//int iTargetSubType = pTargetItemData->GetSubType();
	if (CItemData::ITEM_TYPE_ROD == iTargetType)
		return Py_BuildValue("i", REFINE_CANT_REFINE_ROD);

	if (pTargetItemData->HasNextGrade())
	{
		return Py_BuildValue("i", REFINE_OK);
	}
	else
	{
		return Py_BuildValue("i", REFINE_NOT_NEXT_GRADE_ITEM);
	}

	return Py_BuildValue("i", REFINE_CANT);
}

enum
{
	ATTACH_METIN_CANT,
	ATTACH_METIN_OK,
	ATTACH_METIN_NOT_MATCHABLE_ITEM,
	ATTACH_METIN_NO_MATCHABLE_SOCKET,
	ATTACH_METIN_NOT_EXIST_GOLD_SOCKET,
	ATTACH_METIN_CANT_ATTACH_TO_EQUIPMENT,
};

PyObject* playerCanAttachMetin(PyObject* poSelf, PyObject* poArgs)
{
	int iMetinItemID;
	TItemPos TargetSlotIndex;

	switch (PyTuple_Size(poArgs))
	{
	case 2:
		if (!PyTuple_GetInteger(poArgs, 0, &iMetinItemID))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &TargetSlotIndex.cell))
			return Py_BuildException();
		break;
	case 3:
		if (!PyTuple_GetInteger(poArgs, 0, &iMetinItemID))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &TargetSlotIndex.window_type))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 2, &TargetSlotIndex.cell))
			return Py_BuildException();
		break;
	default:
		return Py_BuildException();
	}
	if (CPythonPlayer::Instance().IsEquipmentSlot(TargetSlotIndex))
	{
		return Py_BuildValue("i", ATTACH_METIN_CANT_ATTACH_TO_EQUIPMENT);
	}

	CItemData * pMetinItemData;
	if (!CItemManager::Instance().GetItemDataPointer(iMetinItemID, &pMetinItemData))
		return Py_BuildException("can't find item data");

	DWORD dwTargetItemIndex = CPythonPlayer::Instance().GetItemIndex(TargetSlotIndex);
	CItemData * pTargetItemData;
	if (!CItemManager::Instance().GetItemDataPointer(dwTargetItemIndex, &pTargetItemData))
		return Py_BuildException("can't find item data");

	DWORD dwMetinWearFlags = pMetinItemData->GetWearFlags();
	DWORD dwTargetWearFlags = pTargetItemData->GetWearFlags();
	if (0 == (dwMetinWearFlags & dwTargetWearFlags))
		return Py_BuildValue("i", ATTACH_METIN_NOT_MATCHABLE_ITEM);
	if (CItemData::ITEM_TYPE_ROD == pTargetItemData->GetType())
		return Py_BuildValue("i", ATTACH_METIN_CANT);

	bool bNotExistGoldSocket = false;

	int iSubType = pMetinItemData->GetSubType();
	for (int i = 0; i < ITEM_SOCKET_SLOT_MAX_NUM; ++i)
	{
		DWORD dwSocketType = CPythonPlayer::Instance().GetItemMetinSocket(TargetSlotIndex, i);
		if (CItemData::METIN_NORMAL == iSubType)
		{
			if (CPythonPlayer::METIN_SOCKET_TYPE_SILVER == dwSocketType ||
				CPythonPlayer::METIN_SOCKET_TYPE_GOLD == dwSocketType)
			{
				return Py_BuildValue("i", ATTACH_METIN_OK);
			}
		}
		else if (CItemData::METIN_GOLD == iSubType)
		{
			if (CPythonPlayer::METIN_SOCKET_TYPE_GOLD == dwSocketType)
			{
				return Py_BuildValue("i", ATTACH_METIN_OK);
			}
			else if (CPythonPlayer::METIN_SOCKET_TYPE_SILVER == dwSocketType)
			{
				bNotExistGoldSocket = true;
			}
		}
	}

	if (bNotExistGoldSocket)
	{
		return Py_BuildValue("i", ATTACH_METIN_NOT_EXIST_GOLD_SOCKET);
	}

	return Py_BuildValue("i", ATTACH_METIN_NO_MATCHABLE_SOCKET);
}

enum
{
	DETACH_METIN_CANT,
	DETACH_METIN_OK,
};

PyObject* playerCanDetach(PyObject* poSelf, PyObject* poArgs)
{
	int iScrollItemIndex;
	TItemPos TargetSlotIndex;
	switch (PyTuple_Size (poArgs))
	{
	case 2:
		if (!PyTuple_GetInteger(poArgs, 0, &iScrollItemIndex))
			return Py_BadArgument();
		if (!PyTuple_GetInteger(poArgs, 1, &TargetSlotIndex.cell))
			return Py_BadArgument();
		break;
	case 3:
		if (!PyTuple_GetInteger(poArgs, 0, &iScrollItemIndex))
			return Py_BadArgument();
		if (!PyTuple_GetInteger(poArgs, 1, &TargetSlotIndex.window_type))
			return Py_BadArgument();
		if (!PyTuple_GetInteger(poArgs, 2, &TargetSlotIndex.cell))
			return Py_BadArgument();
		break;
	default:
		return Py_BadArgument();
	}

	// Scroll
	CItemManager::Instance().SelectItemData(iScrollItemIndex);
	CItemData * pScrollItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pScrollItemData)
		return Py_BuildException("Can't find item data");

#if defined(ENABLE_CHANGELOOK)
	if (CPythonChangeLook::Instance().IsCleanScroll(iScrollItemIndex) != FALSE)
	{
		if (CPythonPlayer::Instance().IsEquipmentSlot(TargetSlotIndex)) {
			return Py_BuildValue("i", DETACH_METIN_CANT);
		}

		const TItemData* pPlayerItem = CPythonPlayer::Instance().GetItemData(TargetSlotIndex);
		if (pPlayerItem) {
			if (pPlayerItem->transmutation == 0) {
				return Py_BuildValue("i", DETACH_METIN_CANT);
			} else {
				return Py_BuildValue("i", DETACH_METIN_OK);
			}
		} else {
			return Py_BuildValue("i", DETACH_METIN_OK);
		}
	}
#endif

	int iScrollType = pScrollItemData->GetType();
	int iScrollSubType = pScrollItemData->GetSubType();
	if (iScrollType != CItemData::ITEM_TYPE_USE)
		return Py_BuildValue("i", DETACH_METIN_CANT);
	if (iScrollSubType != CItemData::USE_DETACHMENT)
		return Py_BuildValue("i", DETACH_METIN_CANT);

	// Target Item
	int iTargetItemIndex = CPythonPlayer::Instance().GetItemIndex(TargetSlotIndex);
	CItemManager::Instance().SelectItemData(iTargetItemIndex);
	CItemData * pTargetItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pTargetItemData)
		return Py_BuildException("Can't find item data");


#ifdef ENABLE_ACCE_SYSTEM
	if (pScrollItemData->GetValue(0) == ACCE_CLEAN_ATTR_VALUE0)
	{
		if ((pTargetItemData->GetType() != CItemData::ITEM_TYPE_COSTUME) || (pTargetItemData->GetSubType() != CItemData::COSTUME_ACCE))
			return Py_BuildValue("i", DETACH_METIN_CANT);

		const TItemData * pPlayerItem = CPythonPlayer::Instance().GetItemData(TargetSlotIndex);
		if (pPlayerItem)
		{
			if (pPlayerItem->alSockets[ACCE_ABSORBED_SOCKET] > 0)
				return Py_BuildValue("i", DETACH_METIN_OK);
			else
				return Py_BuildValue("i", DETACH_METIN_CANT);
		}
		else
			return Py_BuildValue("i", DETACH_METIN_CANT);
	}
#endif

	//int iTargetType = pTargetItemData->GetType();
	//int iTargetSubType = pTargetItemData->GetSubType();

	if (pTargetItemData->IsFlag(CItemData::ITEM_FLAG_REFINEABLE))
	{
		for (int iSlotCount = 0; iSlotCount < METIN_SOCKET_COUNT; ++iSlotCount)
			if (CPythonPlayer::Instance().GetItemMetinSocket(TargetSlotIndex, iSlotCount) > 2)
			{
				return Py_BuildValue("i", DETACH_METIN_OK);
			}
	}

	return Py_BuildValue("i", DETACH_METIN_CANT);
}

PyObject* playerCanUnlock(PyObject* poSelf, PyObject* poArgs)
{
	int iKeyItemIndex;
	TItemPos TargetSlotIndex;
	switch (PyTuple_Size(poArgs))
	{
	case 2:
		if (!PyTuple_GetInteger(poArgs, 0, &iKeyItemIndex))
			return Py_BadArgument();
		if (!PyTuple_GetInteger(poArgs, 1, &TargetSlotIndex.cell))
			return Py_BadArgument();
		break;
	case 3:
		if (!PyTuple_GetInteger(poArgs, 0, &iKeyItemIndex))
			return Py_BadArgument();
		if (!PyTuple_GetInteger(poArgs, 1, &TargetSlotIndex.window_type))
			return Py_BadArgument();
		if (!PyTuple_GetInteger(poArgs, 2, &TargetSlotIndex.cell))
			return Py_BadArgument();
		break;
	default:
		return Py_BadArgument();
	}

	// Key
	CItemManager::Instance().SelectItemData(iKeyItemIndex);
	CItemData * pKeyItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pKeyItemData)
		return Py_BuildException("Can't find item data");
	int iKeyType = pKeyItemData->GetType();
	if (iKeyType != CItemData::ITEM_TYPE_TREASURE_KEY)
		return Py_BuildValue("i", FALSE);

	// Target Item
	int iTargetItemIndex = CPythonPlayer::Instance().GetItemIndex(TargetSlotIndex);
	CItemManager::Instance().SelectItemData(iTargetItemIndex);
	CItemData * pTargetItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pTargetItemData)
		return Py_BuildException("Can't find item data");
	int iTargetType = pTargetItemData->GetType();
	if (iTargetType != CItemData::ITEM_TYPE_TREASURE_BOX)
		return Py_BuildValue("i", FALSE);

	return Py_BuildValue("i", TRUE);
}

PyObject* playerIsRefineGradeScroll(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos ScrollSlotIndex;
	switch (PyTuple_Size(poArgs))
	{
	case 1:
		if (!PyTuple_GetInteger(poArgs, 0, &ScrollSlotIndex.cell))
			return Py_BuildException();
	case 2:
		if (!PyTuple_GetInteger(poArgs, 0, &ScrollSlotIndex.window_type))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &ScrollSlotIndex.cell))
			return Py_BuildException();
	default:
		return Py_BuildException();
	}

	int iScrollItemIndex = CPythonPlayer::Instance().GetItemIndex(ScrollSlotIndex);
	CItemManager::Instance().SelectItemData(iScrollItemIndex);
	CItemData * pScrollItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pScrollItemData)
		return Py_BuildException("Can't find item data");

	return Py_BuildValue("i", REFINE_SCROLL_TYPE_UP_GRADE == pScrollItemData->GetValue(0));
}

PyObject* playerUpdate(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPlayer::Instance().Update();
	return Py_BuildNone();
}

PyObject* playerRender(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildNone();
}

PyObject* playerClear(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPlayer::Instance().Clear();
	return Py_BuildNone();
}

PyObject* playerClearTarget(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPlayer::Instance().SetTarget(0);
	return Py_BuildNone();
}

PyObject* playerSetTarget(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	CPythonPlayer::Instance().SetTarget(iVID);
	return Py_BuildNone();
}

PyObject* playerOpenCharacterMenu(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	CPythonPlayer::Instance().OpenCharacterMenu(iVID);
	return Py_BuildNone();
}

PyObject* playerIsPartyMember(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonPlayer::Instance().IsPartyMemberByVID(iVID));
}

PyObject* playerIsPartyLeader(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	DWORD dwPID;
	if (!CPythonPlayer::Instance().PartyMemberVIDToPID(iVID, &dwPID))
		return Py_BuildValue("i", FALSE);

	CPythonPlayer::TPartyMemberInfo * pPartyMemberInfo;
	if (!CPythonPlayer::Instance().GetPartyMemberPtr(dwPID, &pPartyMemberInfo))
		return Py_BuildValue("i", FALSE);

	return Py_BuildValue("i", CPythonPlayer::PARTY_ROLE_LEADER == pPartyMemberInfo->byState);
}

PyObject* playerIsPartyLeaderByPID(PyObject* poSelf, PyObject* poArgs)
{
	int iPID;
	if (!PyTuple_GetInteger(poArgs, 0, &iPID))
		return Py_BuildException();

	CPythonPlayer::TPartyMemberInfo * pPartyMemberInfo;
	if (!CPythonPlayer::Instance().GetPartyMemberPtr(iPID, &pPartyMemberInfo))
		return Py_BuildValue("i", FALSE);

	return Py_BuildValue("i", CPythonPlayer::PARTY_ROLE_LEADER == pPartyMemberInfo->byState);
}

PyObject* playerGetPartyMemberHPPercentage(PyObject* poSelf, PyObject* poArgs)
{
	int iPID;
	if (!PyTuple_GetInteger(poArgs, 0, &iPID))
		return Py_BuildException();

	CPythonPlayer::TPartyMemberInfo * pPartyMemberInfo;
	if (!CPythonPlayer::Instance().GetPartyMemberPtr(iPID, &pPartyMemberInfo))
		return Py_BuildValue("i", FALSE);

	return Py_BuildValue("i", pPartyMemberInfo->byHPPercentage);
}

PyObject* playerGetPartyMemberState(PyObject* poSelf, PyObject* poArgs)
{
	int iPID;
	if (!PyTuple_GetInteger(poArgs, 0, &iPID))
		return Py_BuildException();

	CPythonPlayer::TPartyMemberInfo * pPartyMemberInfo;
	if (!CPythonPlayer::Instance().GetPartyMemberPtr(iPID, &pPartyMemberInfo))
		return Py_BuildValue("i", FALSE);

	return Py_BuildValue("i", pPartyMemberInfo->byState);
}

PyObject* playerGetPartyMemberAffects(PyObject* poSelf, PyObject* poArgs)
{
	int iPID;
	if (!PyTuple_GetInteger(poArgs, 0, &iPID))
		return Py_BuildException();

	CPythonPlayer::TPartyMemberInfo * pPartyMemberInfo;
	if (!CPythonPlayer::Instance().GetPartyMemberPtr(iPID, &pPartyMemberInfo))
		return Py_BuildValue("i", FALSE);

	return Py_BuildValue("iiiiiii",	pPartyMemberInfo->sAffects[0],
									pPartyMemberInfo->sAffects[1],
									pPartyMemberInfo->sAffects[2],
									pPartyMemberInfo->sAffects[3],
									pPartyMemberInfo->sAffects[4],
									pPartyMemberInfo->sAffects[5],
									pPartyMemberInfo->sAffects[6]);
}

PyObject* playerRemovePartyMember(PyObject* poSelf, PyObject* poArgs)
{
	int iPID;
	if (!PyTuple_GetInteger(poArgs, 0, &iPID))
		return Py_BuildException();

	CPythonPlayer::Instance().RemovePartyMember(iPID);
	return Py_BuildNone();
}

PyObject* playerExitParty(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPlayer::Instance().ExitParty();
	return Py_BuildNone();
}

PyObject* playerGetPKMode(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().GetPKMode());
}

PyObject* playerHasMobilePhoneNumber(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().HasMobilePhoneNumber());
}

PyObject* playerSetWeaponAttackBonusFlag(PyObject* poSelf, PyObject* poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	return Py_BuildNone();
}

PyObject* playerToggleCoolTime(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().__ToggleCoolTime());
}

PyObject* playerToggleLevelLimit(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().__ToggleLevelLimit());
}

PyObject* playerGetTargetVID(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().GetTargetVID());
}

PyObject* playerRegisterEmotionIcon(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();
	char * szFileName;
	if (!PyTuple_GetString(poArgs, 1, &szFileName))
		return Py_BuildException();

	CGraphicImage * pImage = (CGraphicImage *)CResourceManager::Instance().GetResourcePointer(szFileName);
	m_kMap_iEmotionIndex_pkIconImage.insert(std::make_pair(iIndex, pImage));

	return Py_BuildNone();
}

PyObject* playerGetEmotionIconImage(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();
	
	if (m_kMap_iEmotionIndex_pkIconImage.end() == m_kMap_iEmotionIndex_pkIconImage.find(iIndex))
		return Py_BuildValue("i", 0);

	return Py_BuildValue("i", m_kMap_iEmotionIndex_pkIconImage[iIndex]);
}

PyObject* playerSetItemData(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 1, &iVirtualID))
		return Py_BuildException();

	int iNum;
	if (!PyTuple_GetInteger(poArgs, 2, &iNum))
		return Py_BuildException();

	TItemData kItemInst;
	ZeroMemory(&kItemInst, sizeof(kItemInst));
	kItemInst.vnum=iVirtualID;
	kItemInst.count=iNum;
	CPythonPlayer::Instance().SetItemData(TItemPos(INVENTORY, iSlotIndex), kItemInst);
	return Py_BuildNone();
}

PyObject* playerSetItemMetinSocket(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos ItemPos;
	int iMetinSocketNumber;
	int iNum;

	switch (PyTuple_Size(poArgs))
	{
	case 3:
		if (!PyTuple_GetInteger(poArgs, 0, &ItemPos.cell))
			return Py_BuildException();

		if (!PyTuple_GetInteger(poArgs, 1, &iMetinSocketNumber))
			return Py_BuildException();

		if (!PyTuple_GetInteger(poArgs, 2, &iNum))
			return Py_BuildException();

		break;
	case 4:
		if (!PyTuple_GetInteger(poArgs, 0, &ItemPos.window_type))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &ItemPos.cell))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 2, &iMetinSocketNumber))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 3, &iNum))
			return Py_BuildException();

		break;
	default:
		return Py_BuildException();
	}

	CPythonPlayer::Instance().SetItemMetinSocket(ItemPos, iMetinSocketNumber, iNum);
	return Py_BuildNone();
}

PyObject* playerSetItemAttribute(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos ItemPos;
	int iAttributeSlotIndex;
	int iAttributeType;
	int iAttributeValue;

	switch (PyTuple_Size(poArgs))
	{
	case 4:
		if (!PyTuple_GetInteger(poArgs, 0, &ItemPos.cell))
			return Py_BuildException();

		if (!PyTuple_GetInteger(poArgs, 1, &iAttributeSlotIndex))
			return Py_BuildException();

		if (!PyTuple_GetInteger(poArgs, 2, &iAttributeType))
			return Py_BuildException();

		if (!PyTuple_GetInteger(poArgs, 3, &iAttributeValue))
			return Py_BuildException();
		break;
	case 5:
		if (!PyTuple_GetInteger(poArgs, 0, &ItemPos.window_type))
			return Py_BuildException();

		if (!PyTuple_GetInteger(poArgs, 1, &ItemPos.cell))
			return Py_BuildException();

		if (!PyTuple_GetInteger(poArgs, 2, &iAttributeSlotIndex))
			return Py_BuildException();

		if (!PyTuple_GetInteger(poArgs, 3, &iAttributeType))
			return Py_BuildException();

		if (!PyTuple_GetInteger(poArgs, 4, &iAttributeValue))
			return Py_BuildException();
		break;
	}
	CPythonPlayer::Instance().SetItemAttribute(ItemPos, iAttributeSlotIndex, iAttributeType, iAttributeValue);
	return Py_BuildNone();
}

PyObject* playerSetAutoPotionInfo(PyObject* poSelf, PyObject* poArgs)
{
	int potionType = 0;
	if (!PyTuple_GetInteger(poArgs, 0, &potionType))
		return Py_BadArgument();

	CPythonPlayer* player = CPythonPlayer::InstancePtr();

	CPythonPlayer::SAutoPotionInfo& potionInfo = player->GetAutoPotionInfo(potionType);

	if (!PyTuple_GetBoolean(poArgs, 1, &potionInfo.bActivated))
		return Py_BadArgument();

	if (!PyTuple_GetLong(poArgs, 2, &potionInfo.currentAmount))
		return Py_BadArgument();

	if (!PyTuple_GetLong(poArgs, 3, &potionInfo.totalAmount))
		return Py_BadArgument();

	if (!PyTuple_GetLong(poArgs, 4, &potionInfo.inventorySlotIndex))
		return Py_BadArgument();

	return Py_BuildNone();
}

PyObject* playerGetAutoPotionInfo(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPlayer* player = CPythonPlayer::InstancePtr();

	int potionType = 0;
	if (!PyTuple_GetInteger(poArgs, 0, &potionType))
		return Py_BadArgument();

	CPythonPlayer::SAutoPotionInfo& potionInfo = player->GetAutoPotionInfo(potionType);

	return Py_BuildValue("biii", potionInfo.bActivated, int(potionInfo.currentAmount), int(potionInfo.totalAmount), int(potionInfo.inventorySlotIndex));
}

PyObject* playerSlotTypeToInvenType(PyObject* poSelf, PyObject* poArgs)
{
	int slotType = 0;
	if (!PyTuple_GetInteger(poArgs, 0, &slotType))
		return Py_BadArgument();

	return Py_BuildValue("i", SlotTypeToInvenType((BYTE)slotType));
}

#ifdef ENABLE_NEW_EQUIPMENT_SYSTEM
// 플레이어가 벨트를 착용 중인지?
PyObject* playerIsEquippingBelt(PyObject* poSelf, PyObject* poArgs)
{
	const CPythonPlayer* player = CPythonPlayer::InstancePtr();
	bool bEquipping = false;

	const TItemData* data = player->GetItemData(TItemPos(EQUIPMENT, c_Equipment_Belt));

	if (NULL != data)
		bEquipping = 0 < data->count;

	return Py_BuildValue("b", bEquipping);

}

// 검사하려는 벨트 인벤토리 Cell이 사용 가능한 칸인지? (사용가능 여부는 착용 중인 벨트의 강화 정도에 따라 달라짐)
PyObject* playerIsAvailableBeltInventoryCell(PyObject* poSelf, PyObject* poArgs)
{
	const CPythonPlayer* player = CPythonPlayer::InstancePtr();
	const TItemData* pData = player->GetItemData(TItemPos(EQUIPMENT, c_Equipment_Belt));

	if (NULL == pData || 0 == pData->count)
		return Py_BuildValue("b", false);

	CItemManager::Instance().SelectItemData(pData->vnum);
	CItemData * pItem = CItemManager::Instance().GetSelectedItemDataPointer();

	long beltGrade = pItem->GetValue(0);

	int pos = 0;
	if (!PyTuple_GetInteger(poArgs, 0, &pos))
		return Py_BadArgument();

	//return Py_BuildValue("b", CBeltInventoryHelper::IsAvailableCell(pos - c_Belt_Inventory_Slot_Start, GetItemGrade(pItem->GetName())));
	return Py_BuildValue("b", CBeltInventoryHelper::IsAvailableCell(pos - c_Belt_Inventory_Slot_Start, beltGrade));
}
#endif


// 용혼석 강화
PyObject* playerSendDragonSoulRefine(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bSubHeader;
	PyObject* pDic;
	TItemPos RefineItemPoses[DS_REFINE_WINDOW_MAX_NUM];
	if (!PyTuple_GetByte(poArgs, 0, &bSubHeader))
		return Py_BuildException();
	switch (bSubHeader)
	{
	case DS_SUB_HEADER_CLOSE:
		break;
	case DS_SUB_HEADER_DO_UPGRADE:
	case DS_SUB_HEADER_DO_IMPROVEMENT:
	case DS_SUB_HEADER_DO_REFINE:
		{
			if (!PyTuple_GetObject(poArgs, 1, &pDic))
				return Py_BuildException();
			int pos = 0;
			PyObject* key, *value;
			int size = PyDict_Size(pDic);

			while (PyDict_Next(pDic, &pos, &key, &value))
			{
				int i = PyInt_AsLong(key);
				if (i > DS_REFINE_WINDOW_MAX_NUM)
					return Py_BuildException();

				if (!PyTuple_GetByte(value, 0, &RefineItemPoses[i].window_type)
					|| !PyTuple_GetInteger(value, 1, &RefineItemPoses[i].cell))
				{
					return Py_BuildException();
				}
			}
		}
		break;
	}

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendDragonSoulRefinePacket(bSubHeader, RefineItemPoses);

	return Py_BuildNone();
}

#ifdef ENABLE_DS_REFINE_ALL
PyObject* playerSendDragonSoulRefineAll(PyObject* poSelf, PyObject* poArgs) {
	uint8_t subheader;
	if (!PyTuple_GetByte(poArgs, 0, &subheader)) {
		return Py_BuildException();
	} else if (subheader != DS_SUB_HEADER_DO_UPGRADE && subheader != DS_SUB_HEADER_DO_IMPROVEMENT) {
		return Py_BuildException();
	}

	uint8_t type;
	if (!PyTuple_GetByte(poArgs, 1, &type)) {
		return Py_BuildException();
	} else if (!(type >= 0 && type <= 5)) {
		return Py_BuildNone();
	}

	uint8_t grade;
	if (!PyTuple_GetByte(poArgs, 2, &grade)) {
		return Py_BuildException();
	} else if (!(grade >= 0 && grade <= 5)) {
		return Py_BuildNone();
	}

	if (subheader == DS_SUB_HEADER_DO_UPGRADE && grade == 5) {
		return Py_BuildNone();
	}

	CPythonNetworkStream::Instance().SendDragonSoulRefineAllPacket(subheader, type, grade);
	return Py_BuildNone();
}
#endif

PyObject* playerGetItemTypeBySlot(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos Cell;
	switch (PyTuple_Size(poArgs))
	{
		case 1: {
			if (!PyTuple_GetInteger(poArgs, 0, &Cell.cell))
				return Py_BadArgument();
			}
			break;
		case 2: {
			if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
				return Py_BadArgument();
			
			if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
				return Py_BadArgument();
			}
			break;
		default:
			return Py_BuildException();
	}
	
	return Py_BuildValue("i", CPythonPlayer::Instance().GetItemTypeBySlot(Cell));
}

PyObject* playerGetItemSubTypeBySlot(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos Cell;
	switch (PyTuple_Size(poArgs))
	{
		case 1: {
			if (!PyTuple_GetInteger(poArgs, 0, &Cell.cell))
				return Py_BadArgument();
			}
			break;
		case 2: {
			if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
				return Py_BadArgument();
			
			if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
				return Py_BadArgument();
			}
			break;
		default:
			return Py_BuildException();
	}

	return Py_BuildValue("i", CPythonPlayer::Instance().GetItemSubTypeBySlot(Cell));
}

#ifdef ATTR_LOCK
PyObject* playerGetItemAttrLocked(PyObject* poSelf, PyObject* poArgs)
{
	switch (PyTuple_Size(poArgs))
	{
	case 1:
	{
		int iSlotIndex;
		if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
			return Py_BuildException();

		short sAttrLocked = CPythonPlayer::Instance().GetItemAttrLocked(TItemPos(INVENTORY, iSlotIndex));
		return Py_BuildValue("i", sAttrLocked);
	}
	case 2:
	{
		TItemPos Cell;
		if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
			return Py_BuildException();

		if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
			return Py_BuildException();

		short sAttrLocked = CPythonPlayer::Instance().GetItemAttrLocked(Cell);
		return Py_BuildValue("i", sAttrLocked);
	}
	default:
		return Py_BuildException();
	}
}
#endif

#ifdef ENABLE_SKILL_COLOR_SYSTEM
PyObject* playerSetSkillColor(PyObject* poSelf, PyObject* poArgs)
{
	int iSkillSlot;
	if (!PyTuple_GetInteger(poArgs, 0, &iSkillSlot))
		return Py_BadArgument();

	int iColor1;
	if (!PyTuple_GetInteger(poArgs, 1, &iColor1))
		return Py_BadArgument();

	int iColor2;
	if (!PyTuple_GetInteger(poArgs, 2, &iColor2))
		return Py_BadArgument();

	int iColor3;
	if (!PyTuple_GetInteger(poArgs, 3, &iColor3))
		return Py_BadArgument();

	int iColor4;
	if (!PyTuple_GetInteger(poArgs, 4, &iColor4))
		return Py_BadArgument();

	int iColor5;
	if (!PyTuple_GetInteger(poArgs, 5, &iColor5))
		return Py_BadArgument();

	if (iSkillSlot >= 255 || iSkillSlot < 0 || iColor1 < 0 || iColor2 < 0 || iColor3 < 0 || iColor4 < 0 || iColor5 < 0)
		return Py_BadArgument();

	CPythonNetworkStream& nstr = CPythonNetworkStream::Instance();
	nstr.SendSkillColorPacket(iSkillSlot, iColor1, iColor2, iColor3, iColor4, iColor5);

	return Py_BuildNone();
}

PyObject* playerGetSkillColor(PyObject* poSelf, PyObject* poArgs)
{
	int iSkillSlot;
	if (!PyTuple_GetInteger(poArgs, 0, &iSkillSlot))
		return Py_BadArgument();

	if (iSkillSlot >= 255 || iSkillSlot < 0)
		return Py_BadArgument();

	CInstanceBase* pMainInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();
	DWORD* dwSkillColor = pMainInstance->GetSkillColor(iSkillSlot);
	if (!dwSkillColor)
		return Py_BuildValue("iiiii", 0, 0, 0, 0, 0);

	return Py_BuildValue("iiiii", dwSkillColor[0], dwSkillColor[1], dwSkillColor[2], dwSkillColor[3], dwSkillColor[4]);
}
#endif

PyObject* playerGetLevel(PyObject* poSelf, PyObject* poArgs)
{
	DWORD dwLevel = CPythonPlayer::Instance().GetStatus(POINT_LEVEL);
	return Py_BuildValue("l", dwLevel);
}

PyObject* playerIsAntiFlagBySlot(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos Cell;
	int antiflag;
	switch (PyTuple_Size(poArgs))
	{
	case 2:
		if (!PyTuple_GetInteger(poArgs, 0, &Cell.cell))
			return Py_BadArgument();
		if (!PyTuple_GetInteger(poArgs, 1, &antiflag))
			return Py_BadArgument();
		break;

	case 3:
		if (!PyTuple_GetInteger(poArgs, 0, &Cell.window_type))
			return Py_BadArgument();
		if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
			return Py_BadArgument();
		if (!PyTuple_GetInteger(poArgs, 2, &antiflag))
			return Py_BadArgument();
		break;
	default:
		return Py_BuildException();
	}

	DWORD itemAntiFlags = CPythonPlayer::Instance().GetItemAntiFlags(Cell);
	return Py_BuildValue("i", IS_SET(itemAntiFlags, antiflag) != 0 ? TRUE : FALSE);
}

#if defined(ENABLE_GAYA_RENEWAL)
PyObject* playerGetGemShopItemVnum(PyObject* poSelf, PyObject* poArgs)
{
	int nPos;
	if (!PyTuple_GetInteger(poArgs, 0, &nPos))
		return Py_BuildException();

	const TGemShopItem * c_pItemData;
	if (CPythonPlayer::Instance().GetGemShopItemData(nPos, &c_pItemData))
		return Py_BuildValue("i", c_pItemData->dwVnum);

	return Py_BuildValue("i", 0);
}
PyObject* playerGetGemShopItemCount(PyObject* poSelf, PyObject* poArgs)
{
	int nPos;
	if (!PyTuple_GetInteger(poArgs, 0, &nPos))
		return Py_BuildException();

	const TGemShopItem * c_pItemData;
	if (CPythonPlayer::Instance().GetGemShopItemData(nPos, &c_pItemData))
		return Py_BuildValue("i", c_pItemData->bCount);

	return Py_BuildValue("i", 0);
}
PyObject* playerGetGemShopItemPrice(PyObject* poSelf, PyObject* poArgs)
{
	int nPos;
	if (!PyTuple_GetInteger(poArgs, 0, &nPos))
		return Py_BuildException();

	const TGemShopItem * c_pItemData;
	if (CPythonPlayer::Instance().GetGemShopItemData(nPos, &c_pItemData))
		return Py_BuildValue("i", c_pItemData->dwPrice);

	return Py_BuildValue("i", 0);
}
PyObject* playerGetGemShopItemStatus(PyObject* poSelf, PyObject* poArgs)
{
	int nPos;
	if (!PyTuple_GetInteger(poArgs, 0, &nPos))
		return Py_BuildException();

	const TGemShopItem * c_pItemData;
	if (CPythonPlayer::Instance().GetGemShopItemData(nPos, &c_pItemData))
		return Py_BuildValue("i", c_pItemData->status);

	return Py_BuildValue("i", 0);
}
PyObject* playerGetGemShopRefreshTime(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().GetGemShopRefreshTime());
}
PyObject* playerGetGem(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("b", CPythonPlayer::Instance().GetStatus(POINT_GEM));
}
#endif

#if defined(ENABLE_AURA_SYSTEM)
PyObject* playerGetAuraItemID(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotPos))
		return Py_BadArgument();

	TItemData* pAuraItemInstance;
	if (!CPythonPlayer::Instance().GetAuraItemDataPtr(iSlotPos, &pAuraItemInstance))
		return Py_BuildException();

	return Py_BuildValue("i", pAuraItemInstance->vnum);
}

PyObject* playerGetAuraItemCount(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotPos))
		return Py_BadArgument();

	TItemData* pAuraItemInstance;
	if (!CPythonPlayer::Instance().GetAuraItemDataPtr(iSlotPos, &pAuraItemInstance))
		return Py_BuildException();

	return Py_BuildValue("i", pAuraItemInstance->count);
}

PyObject* playerGetAuraItemMetinSocket(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotPos))
		return Py_BadArgument();
	int iMetinSocketIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iMetinSocketIndex))
		return Py_BadArgument();

	TItemData* pAuraItemInstance;
	if (!CPythonPlayer::Instance().GetAuraItemDataPtr(iSlotPos, &pAuraItemInstance))
		return Py_BuildException();

	if (iMetinSocketIndex >= ITEM_SOCKET_SLOT_MAX_NUM || iMetinSocketIndex < 0)
		return Py_BuildException();

	return Py_BuildValue("i", pAuraItemInstance->alSockets[iMetinSocketIndex]);
}

PyObject* playerGetAuraItemAttribute(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotPos))
		return Py_BadArgument();
	int iAttributeSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iAttributeSlotIndex))
		return Py_BadArgument();

	TItemData* pAuraItemInstance;
	if (!CPythonPlayer::Instance().GetAuraItemDataPtr(iSlotPos, &pAuraItemInstance) || (iAttributeSlotIndex >= ITEM_ATTRIBUTE_SLOT_MAX_NUM || iAttributeSlotIndex < 0))
		return Py_BuildValue("ii", 0, 0);

	TPlayerItemAttribute kAttr = pAuraItemInstance->aAttr[iAttributeSlotIndex];
	return Py_BuildValue("ii", kAttr.bType, kAttr.sValue);
}

PyObject* playerIsAuraRefineWindowEmpty(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().IsAuraRefineWindowEmpty());
}

PyObject* playerGetAuraCurrentItemSlotCount(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().GetAuraCurrentItemSlotCount());
}

PyObject* playerIsAuraRefineWindowOpen(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().IsAuraRefineWindowOpen());
}

PyObject* playerGetAuraRefineWindowType(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().GetAuraRefineWindowType());
}

PyObject* playerFineMoveAuraItemSlot(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().FineMoveAuraItemSlot());
}

PyObject* playerSetAuraActivatedItemSlot(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bAuraSlotPos;
	if (!PyTuple_GetByte(poArgs, 0, &bAuraSlotPos))
		return Py_BuildException();

	TItemPos ItemCell;
	if (!PyTuple_GetByte(poArgs, 1, &ItemCell.window_type))
		return Py_BuildException();
	if (!PyTuple_GetInteger(poArgs, 2, &ItemCell.cell))
		return Py_BuildException();

	CPythonPlayer::Instance().SetActivatedAuraSlot(bAuraSlotPos, ItemCell);
	return Py_BuildNone();
}

PyObject* playerFindActivatedAuraSlot(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos ItemCell;
	if (!PyTuple_GetByte(poArgs, 0, &ItemCell.window_type))
		return Py_BuildException();
	if (!PyTuple_GetInteger(poArgs, 1, &ItemCell.cell))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonPlayer::Instance().FindActivatedAuraSlot(ItemCell));
}

PyObject* playerFindUsingAuraSlot(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bAuraSlotPos;
	if (!PyTuple_GetInteger(poArgs, 0, &bAuraSlotPos))
		return Py_BuildException();

	TItemPos ItemCell = CPythonPlayer::Instance().FindUsingAuraSlot(bAuraSlotPos);
	return Py_BuildValue("(ii)", ItemCell.window_type, ItemCell.cell);
}

PyObject* playerGetAuraRefineInfo(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bLevel;
	if (!PyTuple_GetByte(poArgs, 0, &bLevel))
		return Py_BuildException();

	BYTE bRefineInfoIndex;
	if (!PyTuple_GetByte(poArgs, 1, &bRefineInfoIndex))
		return Py_BuildException();

#if defined(ENABLE_NEW_GOLD_LIMIT)
	const uint64_t* info = GetAuraRefineInfo(bLevel);
#else
	const uint32_t* info = GetAuraRefineInfo(bLevel);
#endif

	if (!info || bRefineInfoIndex < AURA_REFINE_INFO_STEP || bRefineInfoIndex >= AURA_REFINE_INFO_MAX) {
		return Py_BuildValue("i", 0);
	}

#if defined(ENABLE_NEW_GOLD_LIMIT)
	return PyLong_FromLongLong(info[bRefineInfoIndex]);
#else
	return Py_BuildValue("i", info[bRefineInfoIndex]);
#endif
}

PyObject* playerGetAuraRefineInfoExpPer(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bAuraRefineInfoSlot;
	if (!PyTuple_GetInteger(poArgs, 0, &bAuraRefineInfoSlot))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonPlayer::Instance().GetAuraRefineInfoExpPct(bAuraRefineInfoSlot));
}

PyObject* playerGetAuraRefineInfoLevel(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bAuraRefineInfoSlot;
	if (!PyTuple_GetInteger(poArgs, 0, &bAuraRefineInfoSlot))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonPlayer::Instance().GetAuraRefineInfoLevel(bAuraRefineInfoSlot));
}
#endif

PyObject* playerNPOS(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("(ii)", NPOS.window_type, NPOS.cell);
}

#if defined(ENABLE_CHANGELOOK)
PyObject* playerGetItemTransmutation(PyObject* poSelf, PyObject* poArgs) {
	switch (PyTuple_Size(poArgs)) {
		case 1: {
			int32_t iSlotIndex;
			if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex)) {
				return Py_BuildException();
			}

			return Py_BuildValue("i", CPythonPlayer::Instance().GetItemTransmutation(TItemPos(INVENTORY, iSlotIndex)));
		}
		case 2: {
			TItemPos Cell;
			if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type)) {
				return Py_BuildException();
			}

			if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell)) {
				return Py_BuildException();
			}

			return Py_BuildValue("i", CPythonPlayer::Instance().GetItemTransmutation(Cell));
		}
		default: {
			return Py_BuildException();
		}
	}
}

PyObject* playerSetChangeLookWindow(PyObject* poSelf, PyObject* poArgs) {
	int32_t iSet = 0;
	if (!PyTuple_GetInteger(poArgs, 0, &iSet)) {
		return Py_BadArgument();
	}

	CPythonPlayer::Instance().SetChangeLookWindow(iSet);
	return Py_BuildNone();
}

PyObject* playerGetChangeLookWindowOpen(PyObject* poSelf, PyObject* poArgs) {
	return Py_BuildValue("i", CPythonPlayer::Instance().GetChangeLookWindowOpen());
}
#endif

#if defined(ENABLE_WORLDBOSS)
PyObject* playerGetWbPoints(PyObject* poSelf, PyObject* poArgs) {
	return Py_BuildValue("i", CPythonPlayer::Instance().GetStatus(POINT_WB_POINTS));
}
#endif

#if defined(ENABLE_HALLOWEEN_EVENT_2022)
PyObject* playerGetSkull(PyObject* poSelf, PyObject* poArgs) {
	return Py_BuildValue("i", CPythonPlayer::Instance().GetStatus(POINT_SKULL));
}
#endif

#if defined(ENABLE_ULTIMATE_REGEN)
PyObject* playerLoadNewRegen(PyObject* poSelf, PyObject* poArgs) {
	CPythonPlayer::Instance().LoadNewRegen();
	return Py_BuildNone();
}
#endif

#if defined(USE_BATTLEPASS)
PyObject* playerGetBattlePassID(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("b", CPythonPlayer::Instance().GetStatus(POINT_BATTLE_PASS_ID));
}
#endif
#ifdef ENABLE_PREMIUM_PLAYERS
PyObject* playerSendPremiumPlayersOpenRequest(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendPremiumPlayersOpenRequest();
	return Py_BuildNone();
}

PyObject* playerSendPremiumPlayersListRequest(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendPremiumPlayersListRequest();
	return Py_BuildNone();
}

PyObject* playerSendPremiumPlayersActivateRequest(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendPremiumPlayersActivateRequest();
	return Py_BuildNone();
}

PyObject* playerIsPremium(PyObject* poSelf, PyObject* poArgs)
{
	CInstanceBase* pMainInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();

	if (!pMainInstance)
		return Py_BuildValue("i", 0);

	return Py_BuildValue("i", pMainInstance->IsPremiumPlayer());
}

PyObject* playerGetPremiumTime(PyObject* poSelf, PyObject* poArgs)
{
	CInstanceBase* pMainInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();

	if (!pMainInstance)
		return Py_BuildValue("i", 0);

	time_t timeNow = time(0);
	int remainTime = (pMainInstance->GetPremiumPlayerTime() - timeNow);

	return Py_BuildValue("i", pMainInstance->GetPremiumPlayerTime());
}
#endif
#ifdef USE_AUTO_HUNT
PyObject* playerClearAutoSkillSlot(PyObject*/* poSelf*/, PyObject* poArgs)
{
    if (PyTuple_Size(poArgs) == 1)
    {
        int32_t iSlot;
        if (!PyTuple_GetInteger(poArgs, 0, &iSlot))
        {
            return Py_BadArgument();
        }

        CPythonPlayer::Instance().ClearAutoSkillSlot(iSlot);
    }
    else
    {
        CPythonPlayer::Instance().ClearAutoSkillSlot();
    }

    return Py_BuildNone();
}

PyObject* playerClearAutoPotionSlot(PyObject*/* poSelf*/, PyObject* poArgs)
{
    if (PyTuple_Size(poArgs) == 1)
    {
        int32_t iSlot;
        if (!PyTuple_GetInteger(poArgs, 0, &iSlot))
        {
            return Py_BadArgument();
        }

        CPythonPlayer::Instance().ClearAutoPotionSlot(iSlot);
    }
    else
    {
        CPythonPlayer::Instance().ClearAutoPotionSlot();
    }

    return Py_BuildNone();
}

PyObject* playerClearAutoAllSlot(PyObject*/* poSelf*/, PyObject* poArgs)
{
    CPythonPlayer::Instance().ClearAutoAllSlot();
    return Py_BuildNone();
}

PyObject* playerSetAutoSlotIndex(PyObject*/* poSelf*/, PyObject* poArgs)
{
    uint8_t bSlotIndex;
    if (!PyTuple_GetByte(poArgs, 0, &bSlotIndex))
    {
        return Py_BadArgument();
    }

    uint32_t dwAttachedSlotIndex;
    if (!PyTuple_GetUnsignedInteger(poArgs, 1, &dwAttachedSlotIndex))
    {
        return Py_BadArgument();
    }

    CPythonPlayer::Instance().SetAutoAttachedSlotIndex(bSlotIndex, dwAttachedSlotIndex);
    return Py_BuildNone();
}

PyObject* playerCheckAutoSlotCoolTime(PyObject*/* poSelf*/, PyObject* poArgs)
{
    uint8_t bSlotIndex;
    if (!PyTuple_GetByte(poArgs, 0, &bSlotIndex))
    {
        return Py_BadArgument();
    }

    uint32_t dwAttachedSlotIndex;
    if (!PyTuple_GetUnsignedInteger(poArgs, 1, &dwAttachedSlotIndex))
    {
        return Py_BadArgument();
    }

    uint32_t dwEditCooldown;
    if (!PyTuple_GetUnsignedInteger(poArgs, 2, &dwEditCooldown))
    {
        return Py_BadArgument();
    }

    return Py_BuildValue("i", CPythonPlayer::Instance().CheckAutoSlotCoolTime(bSlotIndex, dwAttachedSlotIndex, dwEditCooldown));
}

PyObject* playerSetAutoSlotCoolTime(PyObject*/* poSelf*/, PyObject* poArgs)
{
    uint8_t bSlotIndex;
    if (!PyTuple_GetByte(poArgs, 0, &bSlotIndex))
    {
        return Py_BadArgument();
    }

    uint32_t dwEditCooldown;
    if (!PyTuple_GetUnsignedInteger(poArgs, 1, &dwEditCooldown))
    {
        return Py_BadArgument();
    }

    CPythonPlayer::Instance().SetAutoSlotCoolTime(bSlotIndex, dwEditCooldown);
    return Py_BuildNone();
}

PyObject* playerGetAutoSlotCoolTime(PyObject*/* poSelf*/, PyObject* poArgs)
{
    uint8_t bSlotIndex;
    if (!PyTuple_GetByte(poArgs, 0, &bSlotIndex))
    {
        return Py_BadArgument();
    }

    return Py_BuildValue("i", CPythonPlayer::Instance().GetAutoSlotCoolTime(bSlotIndex));
}

PyObject* playerGetAutoSlotIndex(PyObject*/* poSelf*/, PyObject* poArgs)
{
    uint8_t bSlotIndex;
    if (!PyTuple_GetByte(poArgs, 0, &bSlotIndex))
    {
        return Py_BadArgument();
    }

    return Py_BuildValue("i", CPythonPlayer::Instance().GetAutoAttachedSlotIndex(bSlotIndex));
}

PyObject * playerGetPremiumHunt(PyObject*/* poSelf*/, PyObject* poArgs)
{
    return Py_BuildValue("i", CPythonPlayer::Instance().GetStatus(POINT_AUTOHUNT_EXPIRE));
}

PyObject* playerIsToggleSkill(PyObject*/* poSelf*/, PyObject* poArgs)
{
    uint8_t bSlotIndex;
    if (!PyTuple_GetByte(poArgs, 0, &bSlotIndex))
    {
        return Py_BadArgument();
    }

    return Py_BuildValue("b", CPythonPlayer::Instance().IsToggleSkill(bSlotIndex));
}

PyObject* playerItemCanBeAddedToPotionSlots(PyObject*/* poSelf*/, PyObject* poArgs)
{
    uint32_t vnum;
    if (!PyTuple_GetUnsignedInteger(poArgs, 0, &vnum))
    {
        return Py_BadArgument();
    }

    if (std::count(std::begin(st_dwAutoHuntAllowedItems), std::end(st_dwAutoHuntAllowedItems), vnum) == 0)
    {
        return Py_BuildValue("b", false);
    }

    return Py_BuildValue("b", true);
}
#endif

#ifdef USE_PICKUP_FILTER
PyObject* playerPickupFilterClear(PyObject*/* poSelf*/, PyObject* poArgs)
{
#ifdef USE_AUTO_HUNT
    if (CPythonPlayer::Instance().GetStatus(POINT_AUTOHUNT_EXPIRE) < 1)
    {
        return Py_BuildNone();
    }
#endif

    CPythonPlayer::Instance().ClearFilterTable();
    return Py_BuildNone();
}

PyObject* playerPickupFilterUnset(PyObject*/* poSelf*/, PyObject* poArgs)
{
#ifdef USE_AUTO_HUNT
    if (CPythonPlayer::Instance().GetStatus(POINT_AUTOHUNT_EXPIRE) < 1)
    {
        return Py_BuildNone();
    }
#endif

    uint8_t bType;
    if (!PyTuple_GetByte(poArgs, 0, &bType))
    {
        return Py_BadArgument();
    }

    CPythonPlayer::Instance().SetFilterStatus(bType, false);
    return Py_BuildNone();
}

PyObject* playerPickupFilterSet(PyObject*/* poSelf*/, PyObject* poArgs)
{
#ifdef USE_AUTO_HUNT
    if (CPythonPlayer::Instance().GetStatus(POINT_AUTOHUNT_EXPIRE) < 1)
    {
        return Py_BuildNone();
    }
#endif

    uint8_t bType;
    if (!PyTuple_GetByte(poArgs, 0, &bType))
    {
        return Py_BadArgument();
    }

    std::string strCMD;
    if (!PyTuple_GetString(poArgs, 1, strCMD))
    {
        return Py_BadArgument();
    }

    std::vector<std::string> vecArgs;
    split_argument(strCMD.c_str(), vecArgs);

    if (vecArgs.size() < 2)
    {
        return Py_BadArgument();
    }

    uint8_t bIdx;
    if (!str_to_number(bIdx, vecArgs[2].c_str()))
    {
        return Py_BadArgument();
    }

    if (bIdx == FILTER_TYPE_AUTO)
    {
        if (vecArgs.size() < 3)
        {
            return Py_BadArgument();
        }

        uint8_t bAutoPickup;
        if (!str_to_number(bAutoPickup, vecArgs[3].c_str()))
        {
            return Py_BadArgument();
        }

        CPythonPlayer::Instance().SetFilterStatus(bIdx, bAutoPickup == 0 ? false : true);
        return Py_BuildNone();
    }

    if (vecArgs.size() < 9)
    {
        return Py_BadArgument();
    }

    if (bIdx >= FILTER_TYPE_MAX)
    {
        return Py_BadArgument();
    }

    uint8_t bJobNeed;
    if (!str_to_number(bJobNeed, vecArgs[3].c_str()))
    {
        return Py_BadArgument();
    }

    uint8_t bRefineNeed;
    if (!str_to_number(bRefineNeed, vecArgs[4].c_str()))
    {
        return Py_BadArgument();
    }

    uint8_t bLevelNeed;
    if (!str_to_number(bLevelNeed, vecArgs[5].c_str()))
    {
        return Py_BadArgument();
    }

    TFilterTable t;
    t.bJobNeed = bJobNeed == 1 ? true : false;
    t.bRefineNeed = bRefineNeed == 1 ? true : false;
    t.bLevelNeed = bLevelNeed == 1 ? true : false;

    std::vector<std::string> vecJobs;
    split_argument(vecArgs[6].c_str(), vecJobs, "-");

    if (vecJobs.size() != 4)
    {
        return Py_BadArgument();
    }

    for (uint8_t j = 0; j < 4; ++j)
    {
        uint8_t bJob;
        if (!str_to_number(bJob, vecJobs[j].c_str()))
        {
            return Py_BadArgument();
        }

        t.bJobs[j] = bJob == 1 ? true : false;
    }

    std::vector<std::string> vecRefine;
    split_argument(vecArgs[7].c_str(), vecRefine, "-");

    if (vecRefine.size() != 2)
    {
        return Py_BadArgument();
    }

    std::vector<std::string> vecLevel;
    split_argument(vecArgs[8].c_str(), vecLevel, "-");

    if (vecLevel.size() != 2)
    {
        return Py_BadArgument();
    }

    for (uint8_t j = 0; j < 2; ++j)
    {
        if (!str_to_number(t.bRefine[j], vecRefine[j].c_str()) || !str_to_number(t.bLevel[j], vecLevel[j].c_str()))
        {
            return Py_BadArgument();
        }
    }

    memset(&t.bSubs, true, sizeof(t.bSubs));

    if (vecArgs.size() == 10)
    {
        std::vector<std::string> vecSubStatus;
        split_argument(vecArgs[9].c_str(), vecSubStatus, "-");

        for (uint16_t j = 0; j < vecSubStatus.size(); ++j)
        {
            uint8_t bStatus;
            if (!str_to_number(bStatus, vecSubStatus[j].c_str()))
            {
                break;
            }

            if (bStatus != 1)
            {
                t.bSubs[j] = false;
            }
        }
    }

    CPythonPlayer::Instance().AddFilterTable(bIdx, t);
    return Py_BuildNone();
}
#endif

void initPlayer()
{
	static PyMethodDef s_methods[] =
	{
#if defined(ENABLE_WORLDBOSS)
		{"GetWbPoints", playerGetWbPoints, METH_VARARGS},
#endif
#if defined(ENABLE_ULTIMATE_REGEN)
		{"LoadNewRegen", playerLoadNewRegen, METH_VARARGS},
#endif
#if defined(ENABLE_HALLOWEEN_EVENT_2022)
		{"GetSkull", playerGetSkull, METH_VARARGS},
#endif
#if defined(ENABLE_CHANGELOOK)
		{"GetItemTransmutation", playerGetItemTransmutation, METH_VARARGS},
		{"SetChangeLookWindow", playerSetChangeLookWindow, METH_VARARGS},
		{"GetChangeLookWindowOpen", playerGetChangeLookWindowOpen, METH_VARARGS},
#endif
		{"NPOS", playerNPOS, METH_VARARGS},
#if defined(ENABLE_AURA_SYSTEM)
		{"GetAuraItemID", playerGetAuraItemID, METH_VARARGS},
		{"GetAuraItemCount", playerGetAuraItemCount, METH_VARARGS},
		{"GetAuraItemMetinSocket", playerGetAuraItemMetinSocket, METH_VARARGS},
		{"GetAuraItemAttribute", playerGetAuraItemAttribute, METH_VARARGS},
		{"IsAuraRefineWindowEmpty", playerIsAuraRefineWindowEmpty, METH_VARARGS},
		{"GetAuraCurrentItemSlotCount", playerGetAuraCurrentItemSlotCount, METH_VARARGS},
		{"IsAuraRefineWindowOpen", playerIsAuraRefineWindowOpen, METH_VARARGS},
		{"GetAuraRefineWindowType", playerGetAuraRefineWindowType, METH_VARARGS},
		{"FineMoveAuraItemSlot", playerFineMoveAuraItemSlot, METH_VARARGS},
		{"SetAuraActivatedItemSlot", playerSetAuraActivatedItemSlot, METH_VARARGS},
		{"FindActivatedAuraSlot", playerFindActivatedAuraSlot, METH_VARARGS},
		{"FindUsingAuraSlot", playerFindUsingAuraSlot, METH_VARARGS},
		{"GetAuraRefineInfo", playerGetAuraRefineInfo, METH_VARARGS},
		{"GetAuraRefineInfoExpPer", playerGetAuraRefineInfoExpPer, METH_VARARGS},
		{"GetAuraRefineInfoLevel", playerGetAuraRefineInfoLevel, METH_VARARGS},
#endif
#if defined(ENABLE_GAYA_RENEWAL)
		{ "GetGemShopItemVnum",			playerGetGemShopItemVnum,			METH_VARARGS },
		{ "GetGemShopItemCount",		playerGetGemShopItemCount,			METH_VARARGS },
		{ "GetGemShopItemPrice",		playerGetGemShopItemPrice,			METH_VARARGS },
		{ "GetGemShopItemStatus",		playerGetGemShopItemStatus,			METH_VARARGS },
		{ "GetGemShopRefreshTime",		playerGetGemShopRefreshTime,		METH_VARARGS },
		{ "GetGem",						playerGetGem,						METH_VARARGS },
#endif
		{ "IsAntiFlagBySlot",			playerIsAntiFlagBySlot,				METH_VARARGS },
		{ "GetAutoPotionInfo",			playerGetAutoPotionInfo,			METH_VARARGS },
		{ "SetAutoPotionInfo",			playerSetAutoPotionInfo,			METH_VARARGS },

		{ "PickCloseItem",				playerPickCloseItem,				METH_VARARGS },
		//{ "PickCloseMoney",				playerPickCloseMoney,				METH_VARARGS },
		{ "SetGameWindow",				playerSetGameWindow,				METH_VARARGS },
		{ "RegisterEffect",				playerRegisterEffect,				METH_VARARGS },
		{ "RegisterCacheEffect",		playerRegisterCacheEffect,			METH_VARARGS },
		{ "SetMouseState",				playerSetMouseState,				METH_VARARGS },
		{ "SetMouseFunc",				playerSetMouseFunc,					METH_VARARGS },
		{ "GetMouseFunc",				playerGetMouseFunc,					METH_VARARGS },
		{ "SetMouseMiddleButtonState",	playerSetMouseMiddleButtonState,	METH_VARARGS },
		{ "SetMainCharacterIndex",		playerSetMainCharacterIndex,		METH_VARARGS },
		{ "GetMainCharacterIndex",		playerGetMainCharacterIndex,		METH_VARARGS },
		{ "GetMainCharacterName",		playerGetMainCharacterName,			METH_VARARGS },
		{ "GetMainCharacterPosition",	playerGetMainCharacterPosition,		METH_VARARGS },
		{ "IsMainCharacterIndex",		playerIsMainCharacterIndex,			METH_VARARGS },
		{ "CanAttackInstance",			playerCanAttackInstance,			METH_VARARGS },
		{ "IsActingEmotion",			playerIsActingEmotion,				METH_VARARGS },
		{ "IsPVPInstance",				playerIsPVPInstance,				METH_VARARGS },
		{ "IsSameEmpire",				playerIsSameEmpire,					METH_VARARGS },
		{ "IsChallengeInstance",		playerIsChallengeInstance,			METH_VARARGS },
		{ "IsRevengeInstance",			playerIsRevengeInstance,			METH_VARARGS },
		{ "IsCantFightInstance",		playerIsCantFightInstance,			METH_VARARGS },
		{ "GetCharacterDistance",		playerGetCharacterDistance,			METH_VARARGS },
		{ "IsInSafeArea",				playerIsInSafeArea,					METH_VARARGS },
		{ "IsMountingHorse",			playerIsMountingHorse,				METH_VARARGS },
		{ "IsObserverMode",				playerIsObserverMode,				METH_VARARGS },
		{ "ActEmotion",					playerActEmotion,					METH_VARARGS },

		{ "ShowPlayer",					playerShowPlayer,					METH_VARARGS },
		{ "HidePlayer",					playerHidePlayer,					METH_VARARGS },

		{ "ComboAttack",				playerComboAttack,					METH_VARARGS },

		{ "SetAutoCameraRotationSpeed",	playerSetAutoCameraRotationSpeed,	METH_VARARGS },
		{ "SetAttackKeyState",			playerSetAttackKeyState,			METH_VARARGS },
		{ "SetSingleDIKKeyState",		playerSetSingleDIKKeyState,			METH_VARARGS },
		{ "EndKeyWalkingImmediately",	playerEndKeyWalkingImmediately,		METH_VARARGS },
		{ "StartMouseWalking",			playerStartMouseWalking,			METH_VARARGS },
		{ "EndMouseWalking",			playerEndMouseWalking,				METH_VARARGS },
		{ "ResetCameraRotation",		playerResetCameraRotation,			METH_VARARGS },
		{ "SetQuickCameraMode",			playerSetQuickCameraMode,			METH_VARARGS },

		///////////////////////////////////////////////////////////////////////////////////////////

		{ "SetSkill",							playerSetSkill,								METH_VARARGS },
		{ "GetSkillIndex",						playerGetSkillIndex,						METH_VARARGS },
		{ "GetSkillSlotIndex",					playerGetSkillSlotIndex,					METH_VARARGS },
		{ "GetSkillGrade",						playerGetSkillGrade,						METH_VARARGS },
		{ "GetSkillLevel",						playerGetSkillLevel,						METH_VARARGS },
		{ "GetSkillCurrentEfficientPercentage",	playerGetSkillCurrentEfficientPercentage,	METH_VARARGS },
		{ "GetSkillNextEfficientPercentage",	playerGetSkillNextEfficientPercentage,		METH_VARARGS },
		{ "GetLevel",						playerGetLevel,						METH_VARARGS },

		{ "ClickSkillSlot",						playerClickSkillSlot,						METH_VARARGS },
		{ "ChangeCurrentSkillNumberOnly",		playerChangeCurrentSkillNumberOnly,			METH_VARARGS },
		{ "ClearSkillDict",						playerClearSkillDict,						METH_VARARGS },

#ifdef ATTR_LOCK
		{ "GetItemAttrLocked",			playerGetItemAttrLocked,			METH_VARARGS },
#endif

		{ "GetItemIndex",						playerGetItemIndex,							METH_VARARGS },
		{ "GetItemFlags",						playerGetItemFlags,							METH_VARARGS },
		{ "GetItemCount",						playerGetItemCount,							METH_VARARGS },
		{ "GetItemCountByVnum",					playerGetItemCountByVnum,					METH_VARARGS },
#ifdef ENABLE_EXTRA_INVENTORY
		//To fix search item in refine (0)
		{ "GetItemCountbyVnumExtraInventory",	playerGetItemCountByVnumExtra,				METH_VARARGS },
#endif
		{ "GetItemMetinSocket",					playerGetItemMetinSocket,					METH_VARARGS },
		{ "GetItemAttribute",					playerGetItemAttribute,						METH_VARARGS },
#if defined(GAIDEN)
		{ "GetItemUnbindTime",					playerGetItemUnbindTime,					METH_VARARGS },
#endif
		{ "GetISellItemPrice",					playerGetISellItemPrice,					METH_VARARGS },
		{ "MoveItem",							playerMoveItem,								METH_VARARGS },
		{ "SendClickItemPacket",				playerSendClickItemPacket,					METH_VARARGS },

		///////////////////////////////////////////////////////////////////////////////////////////

		{ "GetName",					playerGetName,						METH_VARARGS },
		{ "GetJob",						playerGetJob,						METH_VARARGS },
		{ "GetRace",					playerGetRace,						METH_VARARGS },
		{ "GetPlayTime",				playerGetPlayTime,					METH_VARARGS },
		{ "SetPlayTime",				playerSetPlayTime,					METH_VARARGS },
#ifdef ENABLE_PREMIUM_PLAYERS
		{ "SendPremiumPlayersOpenRequest",		playerSendPremiumPlayersOpenRequest,		METH_VARARGS },
		{ "SendPremiumPlayersListRequest",		playerSendPremiumPlayersListRequest,		METH_VARARGS },
		{ "SendPremiumPlayersActivateRequest",	playerSendPremiumPlayersActivateRequest,	METH_VARARGS },

		{ "IsPremium",							playerIsPremium,							METH_VARARGS },
		{ "GetPremiumTime",						playerGetPremiumTime,						METH_VARARGS },
#endif
		{ "IsSkillCoolTime",			playerIsSkillCoolTime,				METH_VARARGS },
		{ "GetSkillCoolTime",			playerGetSkillCoolTime,				METH_VARARGS },
		{ "IsSkillActive",				playerIsSkillActive,				METH_VARARGS },
		{ "UseGuildSkill",				playerUseGuildSkill,				METH_VARARGS },
		{ "AffectIndexToSkillIndex",	playerAffectIndexToSkillIndex,		METH_VARARGS },
		{ "GetEXP",						playerGetEXP,						METH_VARARGS },
		{ "GetStatus",					playerGetStatus,					METH_VARARGS },
		{ "SetStatus",					playerSetStatus,					METH_VARARGS },
		{ "GetElk",						playerGetElk,						METH_VARARGS },
		{ "GetMoney",					playerGetElk,						METH_VARARGS },

		{ "GetGuildID",					playerGetGuildID,					METH_VARARGS },
		{ "GetGuildName",				playerGetGuildName,					METH_VARARGS },
		{ "GetAlignmentData",			playerGetAlignmentData,				METH_VARARGS },
		{"RequestAddLocalQuickSlot", playerRequestAddLocalQuickSlot, METH_VARARGS},
		{"RequestAddToEmptyLocalQuickSlot", playerRequestAddToEmptyLocalQuickSlot, METH_VARARGS},
		{"RequestDeleteGlobalQuickSlot", playerRequestDeleteGlobalQuickSlot, METH_VARARGS},
		{"RequestMoveGlobalQuickSlotToLocalQuickSlot", playerRequestMoveGlobalQuickSlotToLocalQuickSlot, METH_VARARGS},
		{"RequestUseLocalQuickSlot", playerRequestUseLocalQuickSlot, METH_VARARGS},
		{"LocalQuickSlotIndexToGlobalQuickSlotIndex", playerLocalQuickSlotIndexToGlobalQuickSlotIndex, METH_VARARGS},

		{"GetQuickPage", playerGetQuickPage, METH_VARARGS},
		{"SetQuickPage", playerSetQuickPage, METH_VARARGS},
		{"GetLocalQuickSlot", playerGetLocalQuickSlot, METH_VARARGS},
		{"GetGlobalQuickSlot", playerGetGlobalQuickSlot, METH_VARARGS},
		{"RemoveQuickSlotByValue", playerRemoveQuickSlotByValue, METH_VARARGS},

		{ "isItem",						playerisItem,						METH_VARARGS },
		{ "IsEquipmentSlot",			playerIsEquipmentSlot,				METH_VARARGS },
		{ "IsDSEquipmentSlot",			playerIsDSEquipmentSlot,			METH_VARARGS },
		{ "IsCostumeSlot",				playerIsCostumeSlot,				METH_VARARGS },
		{ "IsValuableItem",				playerIsValuableItem,				METH_VARARGS },
		{ "IsOpenPrivateShop",			playerIsOpenPrivateShop,			METH_VARARGS },

#ifdef ENABLE_NEW_EQUIPMENT_SYSTEM
		{ "IsBeltInventorySlot",			playerIsBeltInventorySlot,			METH_VARARGS },
		{ "IsEquippingBelt",				playerIsEquippingBelt,				METH_VARARGS },
		{ "IsAvailableBeltInventoryCell",	playerIsAvailableBeltInventoryCell,	METH_VARARGS },
#endif

		// Refine
		{ "GetItemGrade",				playerGetItemGrade,					METH_VARARGS },
		{ "CanRefine",					playerCanRefine,					METH_VARARGS },
		{ "CanDetach",					playerCanDetach,					METH_VARARGS },
		{ "CanUnlock",					playerCanUnlock,					METH_VARARGS },
		{ "CanAttachMetin",				playerCanAttachMetin,				METH_VARARGS },
		{ "IsRefineGradeScroll",		playerIsRefineGradeScroll,			METH_VARARGS },

		{ "ClearTarget",				playerClearTarget,					METH_VARARGS },
		{ "SetTarget",					playerSetTarget,					METH_VARARGS },
		{ "OpenCharacterMenu",			playerOpenCharacterMenu,			METH_VARARGS },

		{ "Update",						playerUpdate,						METH_VARARGS },
		{ "Render",						playerRender,						METH_VARARGS },
		{ "Clear",						playerClear,						METH_VARARGS },

		// Party
		{ "IsPartyMember",				playerIsPartyMember,				METH_VARARGS },
		{ "IsPartyLeader",				playerIsPartyLeader,				METH_VARARGS },
		{ "IsPartyLeaderByPID",			playerIsPartyLeaderByPID,			METH_VARARGS },
		{ "GetPartyMemberHPPercentage",	playerGetPartyMemberHPPercentage,	METH_VARARGS },
		{ "GetPartyMemberState",		playerGetPartyMemberState,			METH_VARARGS },
		{ "GetPartyMemberAffects",		playerGetPartyMemberAffects,		METH_VARARGS },
		{ "RemovePartyMember",			playerRemovePartyMember,			METH_VARARGS },
		{ "ExitParty",					playerExitParty,					METH_VARARGS },

		// PK Mode
		{ "GetPKMode",					playerGetPKMode,					METH_VARARGS },

		// Mobile
		{ "HasMobilePhoneNumber",		playerHasMobilePhoneNumber,			METH_VARARGS },

		// Emotion
		{ "RegisterEmotionIcon",		playerRegisterEmotionIcon,			METH_VARARGS },
		{ "GetEmotionIconImage",		playerGetEmotionIconImage,			METH_VARARGS },

		// For System
		{ "SetWeaponAttackBonusFlag",	playerSetWeaponAttackBonusFlag,		METH_VARARGS },
		{ "ToggleCoolTime",				playerToggleCoolTime,				METH_VARARGS },
		{ "ToggleLevelLimit",			playerToggleLevelLimit,				METH_VARARGS },
		{ "GetTargetVID",				playerGetTargetVID,					METH_VARARGS },

		{ "SetItemData",				playerSetItemData,					METH_VARARGS },
		{ "SetItemMetinSocket",			playerSetItemMetinSocket,			METH_VARARGS },
		{ "SetItemAttribute",			playerSetItemAttribute,				METH_VARARGS },
		{ "SetItemCount",				playerSetItemCount,					METH_VARARGS },

		{ "GetItemLink",				playerGetItemLink,					METH_VARARGS },
		{ "SlotTypeToInvenType",		playerSlotTypeToInvenType,			METH_VARARGS },
		{ "SendDragonSoulRefine",		playerSendDragonSoulRefine,			METH_VARARGS },
#ifdef ENABLE_DS_REFINE_ALL
		{"SendDragonSoulRefineAll", playerSendDragonSoulRefineAll, METH_VARARGS},
#endif
		{"GetItemTypeBySlot", playerGetItemTypeBySlot, METH_VARARGS},
		{"GetItemSubTypeBySlot", playerGetItemSubTypeBySlot, METH_VARARGS},
#ifdef ENABLE_SEALBIND_SYSTEM
		{ "CanSealItem",				playerCanSealItem,					METH_VARARGS },
		{ "GetItemSealDate",			playerGetItemSealDate,				METH_VARARGS },
		{ "GetItemUnSealLeftTime",		GetItemUnSealLeftTime,				METH_VARARGS },
#endif
#ifdef ENABLE_SKILL_COLOR_SYSTEM
		{ "SetSkillColor",					playerSetSkillColor,				METH_VARARGS },
		{ "GetSkillColor",					playerGetSkillColor,				METH_VARARGS },
#endif
#if defined(USE_BATTLEPASS)
		{"GetBattlePassID", playerGetBattlePassID, METH_VARARGS},
#endif
#ifdef USE_AUTO_HUNT
        { "ClearAutoSkillSlot", playerClearAutoSkillSlot, METH_VARARGS },
        { "ClearAutoPotionSlot", playerClearAutoPotionSlot, METH_VARARGS },
        { "ClearAutoAllSlot", playerClearAutoAllSlot, METH_VARARGS },
        { "SetAutoSlotIndex", playerSetAutoSlotIndex, METH_VARARGS },
        { "CheckSlotCoolTime", playerCheckAutoSlotCoolTime, METH_VARARGS },
        { "SetAutoSlotCoolTime", playerSetAutoSlotCoolTime, METH_VARARGS },
        { "GetAutoSlotCoolTime", playerGetAutoSlotCoolTime, METH_VARARGS },
        { "GetAutoSlotIndex", playerGetAutoSlotIndex, METH_VARARGS },
        { "GetPremiumHunt", playerGetPremiumHunt, METH_VARARGS },
        { "IsToggleSkill", playerIsToggleSkill, METH_VARARGS },
        { "ItemCanBeAddedToPotionSlots", playerItemCanBeAddedToPotionSlots, METH_VARARGS },
#endif
#ifdef USE_PICKUP_FILTER
        { "PickupFilterClear", playerPickupFilterClear, METH_VARARGS },
        { "PickupFilterUnset", playerPickupFilterUnset, METH_VARARGS },
        { "PickupFilterSet", playerPickupFilterSet, METH_VARARGS },
#endif
        { NULL, NULL, NULL },
    };

	PyObject* poModule = Py_InitModule("player", s_methods);

#if defined(ENABLE_AURA_SYSTEM)
	PyModule_AddIntConstant(poModule, "SLOT_TYPE_AURA", SLOT_TYPE_AURA);
	PyModule_AddIntConstant(poModule, "AURA_REFINE", AURA_REFINE);
	PyModule_AddIntConstant(poModule, "AURA_MAX_LEVEL", c_AuraMaxLevel);
	PyModule_AddIntConstant(poModule, "AURA_SLOT_MAIN", AURA_SLOT_MAIN);
	PyModule_AddIntConstant(poModule, "AURA_SLOT_SUB", AURA_SLOT_SUB);
	PyModule_AddIntConstant(poModule, "AURA_SLOT_RESULT", AURA_SLOT_RESULT);
	PyModule_AddIntConstant(poModule, "AURA_SLOT_MAX", AURA_SLOT_MAX);
	PyModule_AddIntConstant(poModule, "AURA_WINDOW_TYPE_ABSORB", AURA_WINDOW_TYPE_ABSORB);
	PyModule_AddIntConstant(poModule, "AURA_WINDOW_TYPE_GROWTH", AURA_WINDOW_TYPE_GROWTH);
	PyModule_AddIntConstant(poModule, "AURA_WINDOW_TYPE_EVOLVE", AURA_WINDOW_TYPE_EVOLVE);
	PyModule_AddIntConstant(poModule, "AURA_WINDOW_TYPE_MAX", AURA_WINDOW_TYPE_MAX);
	PyModule_AddIntConstant(poModule, "AURA_REFINE_INFO_SLOT_CURRENT", AURA_REFINE_INFO_SLOT_CURRENT);
	PyModule_AddIntConstant(poModule, "AURA_REFINE_INFO_SLOT_NEXT", AURA_REFINE_INFO_SLOT_NEXT);
	PyModule_AddIntConstant(poModule, "AURA_REFINE_INFO_SLOT_EVOLVED", AURA_REFINE_INFO_SLOT_EVOLVED);
	PyModule_AddIntConstant(poModule, "AURA_REFINE_INFO_STEP", AURA_REFINE_INFO_STEP);
	PyModule_AddIntConstant(poModule, "AURA_REFINE_INFO_LEVEL_MIN", AURA_REFINE_INFO_LEVEL_MIN);
	PyModule_AddIntConstant(poModule, "AURA_REFINE_INFO_LEVEL_MAX", AURA_REFINE_INFO_LEVEL_MAX);
	PyModule_AddIntConstant(poModule, "AURA_REFINE_INFO_NEED_EXP", AURA_REFINE_INFO_NEED_EXP);
	PyModule_AddIntConstant(poModule, "AURA_REFINE_INFO_MATERIAL_VNUM", AURA_REFINE_INFO_MATERIAL_VNUM);
	PyModule_AddIntConstant(poModule, "AURA_REFINE_INFO_MATERIAL_COUNT", AURA_REFINE_INFO_MATERIAL_COUNT);
	PyModule_AddIntConstant(poModule, "AURA_REFINE_INFO_NEED_GOLD", AURA_REFINE_INFO_NEED_GOLD);
	PyModule_AddIntConstant(poModule, "AURA_REFINE_INFO_EVOLVE_PCT", AURA_REFINE_INFO_EVOLVE_PCT);
	PyModule_AddIntConstant(poModule, "ITEM_SOCKET_AURA_DRAIN_ITEM_VNUM", CPythonPlayer::ITEM_SOCKET_AURA_DRAIN_ITEM_VNUM);
	PyModule_AddIntConstant(poModule, "ITEM_SOCKET_AURA_CURRENT_LEVEL", CPythonPlayer::ITEM_SOCKET_AURA_CURRENT_LEVEL);
	PyModule_AddIntConstant(poModule, "ITEM_SOCKET_AURA_BOOST", CPythonPlayer::ITEM_SOCKET_AURA_BOOST);
	PyModule_AddIntConstant(poModule, "ITEM_VALUE_AURA_MATERIAL_EXP", CPythonPlayer::ITEM_VALUE_AURA_MATERIAL_EXP);
	PyModule_AddIntConstant(poModule, "ITEM_VALUE_AURA_BOOST_PERCENT", CPythonPlayer::ITEM_VALUE_AURA_BOOST_PERCENT);
	PyModule_AddIntConstant(poModule, "ITEM_VALUE_AURA_BOOST_TIME", CPythonPlayer::ITEM_VALUE_AURA_BOOST_TIME);
	PyModule_AddIntConstant(poModule, "ITEM_VALUE_AURA_BOOST_UNLIMITED", CPythonPlayer::ITEM_VALUE_AURA_BOOST_UNLIMITED);
#endif
	PyModule_AddIntConstant(poModule, "LEVEL",					POINT_LEVEL);
    PyModule_AddIntConstant(poModule, "VOICE",					POINT_VOICE);
    PyModule_AddIntConstant(poModule, "EXP",					POINT_EXP);
    PyModule_AddIntConstant(poModule, "NEXT_EXP",				POINT_NEXT_EXP);
    PyModule_AddIntConstant(poModule, "HP",						POINT_HP);
    PyModule_AddIntConstant(poModule, "MAX_HP",					POINT_MAX_HP);
    PyModule_AddIntConstant(poModule, "SP",						POINT_SP);
    PyModule_AddIntConstant(poModule, "MAX_SP",					POINT_MAX_SP);
    PyModule_AddIntConstant(poModule, "STAMINA",				POINT_STAMINA);
    PyModule_AddIntConstant(poModule, "MAX_STAMINA",			POINT_MAX_STAMINA);
    PyModule_AddIntConstant(poModule, "ELK",					POINT_GOLD);
#if defined(ENABLE_GAYA_RENEWAL)
	PyModule_AddIntConstant(poModule, "GEM",						POINT_GEM);
#endif
    PyModule_AddIntConstant(poModule, "ST",						POINT_ST);
    PyModule_AddIntConstant(poModule, "HT",						POINT_HT);
    PyModule_AddIntConstant(poModule, "DX",						POINT_DX);
    PyModule_AddIntConstant(poModule, "IQ",						POINT_IQ);
    PyModule_AddIntConstant(poModule, "ATT_POWER",				POINT_ATT_POWER);
	PyModule_AddIntConstant(poModule, "ATT_MIN",				POINT_MIN_ATK);
	PyModule_AddIntConstant(poModule, "ATT_MAX",				POINT_MAX_ATK);
	PyModule_AddIntConstant(poModule, "MIN_MAGIC_WEP",			POINT_MIN_MAGIC_WEP);
	PyModule_AddIntConstant(poModule, "MAX_MAGIC_WEP",			POINT_MAX_MAGIC_WEP);
    PyModule_AddIntConstant(poModule, "ATT_SPEED",				POINT_ATT_SPEED);
	PyModule_AddIntConstant(poModule, "ATT_BONUS",				POINT_ATT_GRADE_BONUS);
    PyModule_AddIntConstant(poModule, "EVADE_RATE",				POINT_EVADE_RATE);
    PyModule_AddIntConstant(poModule, "MOVING_SPEED",			POINT_MOV_SPEED);
    PyModule_AddIntConstant(poModule, "DEF_GRADE",				POINT_DEF_GRADE);
    PyModule_AddIntConstant(poModule, "DEF_BONUS",				POINT_DEF_GRADE_BONUS);
    PyModule_AddIntConstant(poModule, "CASTING_SPEED",			POINT_CASTING_SPEED);
    PyModule_AddIntConstant(poModule, "MAG_ATT",				POINT_MAGIC_ATT_GRADE);
    PyModule_AddIntConstant(poModule, "MAG_DEF",				POINT_MAGIC_DEF_GRADE);
    PyModule_AddIntConstant(poModule, "EMPIRE_POINT",			POINT_EMPIRE_POINT);
	PyModule_AddIntConstant(poModule, "STAT",					POINT_STAT);
	PyModule_AddIntConstant(poModule, "SKILL_PASSIVE",			POINT_SUB_SKILL);
	PyModule_AddIntConstant(poModule, "SKILL_SUPPORT",			POINT_SUB_SKILL);
	PyModule_AddIntConstant(poModule, "SKILL_ACTIVE",			POINT_SKILL);
	PyModule_AddIntConstant(poModule, "SKILL_HORSE",			POINT_HORSE_SKILL);
	PyModule_AddIntConstant(poModule, "PLAYTIME",				POINT_PLAYTIME);
	PyModule_AddIntConstant(poModule, "BOW_DISTANCE",			POINT_BOW_DISTANCE);
	PyModule_AddIntConstant(poModule, "HP_RECOVERY",			POINT_HP_RECOVERY);
	PyModule_AddIntConstant(poModule, "SP_RECOVERY",			POINT_SP_RECOVERY);
	PyModule_AddIntConstant(poModule, "ATTACKER_BONUS",			POINT_PARTY_ATT_GRADE);
    PyModule_AddIntConstant(poModule, "MAX_NUM",				POINT_MAX_NUM);
	////
	PyModule_AddIntConstant(poModule, "POINT_CRITICAL_PCT",		POINT_CRITICAL_PCT);
	PyModule_AddIntConstant(poModule, "POINT_PENETRATE_PCT",	POINT_PENETRATE_PCT);
	PyModule_AddIntConstant(poModule, "POINT_MALL_ATTBONUS",	POINT_MALL_ATTBONUS);
	PyModule_AddIntConstant(poModule, "POINT_MALL_DEFBONUS",	POINT_MALL_DEFBONUS);
	PyModule_AddIntConstant(poModule, "POINT_MALL_EXPBONUS",	POINT_MALL_EXPBONUS);
	PyModule_AddIntConstant(poModule, "POINT_MALL_ITEMBONUS",	POINT_MALL_ITEMBONUS);
	PyModule_AddIntConstant(poModule, "POINT_MALL_GOLDBONUS",	POINT_MALL_GOLDBONUS);
	PyModule_AddIntConstant(poModule, "POINT_MAX_HP_PCT",		POINT_MAX_HP_PCT);
	PyModule_AddIntConstant(poModule, "POINT_MAX_SP_PCT",		POINT_MAX_SP_PCT);

	PyModule_AddIntConstant(poModule, "POINT_SKILL_DAMAGE_BONUS",		POINT_SKILL_DAMAGE_BONUS);
	PyModule_AddIntConstant(poModule, "POINT_NORMAL_HIT_DAMAGE_BONUS",		POINT_NORMAL_HIT_DAMAGE_BONUS);
	PyModule_AddIntConstant(poModule, "POINT_SKILL_DEFEND_BONUS",		POINT_SKILL_DEFEND_BONUS);
	PyModule_AddIntConstant(poModule, "POINT_NORMAL_HIT_DEFEND_BONUS",		POINT_NORMAL_HIT_DEFEND_BONUS);
	PyModule_AddIntConstant(poModule, "POINT_PC_UNUSED01",		POINT_PC_UNUSED01);
	PyModule_AddIntConstant(poModule, "POINT_PC_UNUSED02",		POINT_PC_UNUSED02);

	PyModule_AddIntConstant(poModule, "ENERGY",		POINT_ENERGY);
	PyModule_AddIntConstant(poModule, "ENERGY_END_TIME",		POINT_ENERGY_END_TIME);

    PyModule_AddIntConstant(poModule, "SKILL_GRADE_NORMAL",			CPythonPlayer::SKILL_NORMAL);
    PyModule_AddIntConstant(poModule, "SKILL_GRADE_MASTER",			CPythonPlayer::SKILL_MASTER);
    PyModule_AddIntConstant(poModule, "SKILL_GRADE_GRAND_MASTER",	CPythonPlayer::SKILL_GRAND_MASTER);
    PyModule_AddIntConstant(poModule, "SKILL_GRADE_PERFECT_MASTER",	CPythonPlayer::SKILL_PERFECT_MASTER);

	PyModule_AddIntConstant(poModule, "CATEGORY_ACTIVE",		CPythonPlayer::CATEGORY_ACTIVE);
	PyModule_AddIntConstant(poModule, "CATEGORY_PASSIVE",		CPythonPlayer::CATEGORY_PASSIVE);

	PyModule_AddIntConstant(poModule, "INVENTORY_PAGE_COLUMN",	c_Inventory_Page_Column);
	PyModule_AddIntConstant(poModule, "INVENTORY_PAGE_ROW",		c_Inventory_Page_Row);
	PyModule_AddIntConstant(poModule, "INVENTORY_PAGE_SIZE",	c_Inventory_Page_Size);
	PyModule_AddIntConstant(poModule, "INVENTORY_PAGE_COUNT",	c_Inventory_Page_Count);
	PyModule_AddIntConstant(poModule, "INVENTORY_SLOT_COUNT",	c_Inventory_Count);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_SLOT_START",	c_Equipment_Start);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_PAGE_COUNT",	c_Equipment_Count);
#if defined(USE_ATTR_6TH_7TH)
	PyModule_AddIntConstant(poModule, "ITEM_SLOT_COUNT", c_ItemSlot_Count);
#endif
#ifdef ENABLE_NEW_EQUIPMENT_SYSTEM
	PyModule_AddIntConstant(poModule, "NEW_EQUIPMENT_SLOT_START",	c_New_Equipment_Start);
	PyModule_AddIntConstant(poModule, "NEW_EQUIPMENT_SLOT_COUNT",	c_New_Equipment_Count);
#endif

	PyModule_AddIntConstant(poModule, "MBF_SKILL",	CPythonPlayer::MBF_SKILL);
	PyModule_AddIntConstant(poModule, "MBF_ATTACK",	CPythonPlayer::MBF_ATTACK);
	PyModule_AddIntConstant(poModule, "MBF_CAMERA",	CPythonPlayer::MBF_CAMERA);
	PyModule_AddIntConstant(poModule, "MBF_SMART",	CPythonPlayer::MBF_SMART);
	PyModule_AddIntConstant(poModule, "MBF_MOVE",	CPythonPlayer::MBF_MOVE);
	PyModule_AddIntConstant(poModule, "MBF_AUTO",	CPythonPlayer::MBF_AUTO);
	PyModule_AddIntConstant(poModule, "MBS_PRESS",	CPythonPlayer::MBS_PRESS);
	PyModule_AddIntConstant(poModule, "MBS_CLICK",	CPythonPlayer::MBS_CLICK);
	PyModule_AddIntConstant(poModule, "MBT_RIGHT",	CPythonPlayer::MBT_RIGHT);
	PyModule_AddIntConstant(poModule, "MBT_LEFT",	CPythonPlayer::MBT_LEFT);

	// Public code with server
	PyModule_AddIntConstant(poModule, "SLOT_TYPE_NONE",						SLOT_TYPE_NONE);
	PyModule_AddIntConstant(poModule, "SLOT_TYPE_INVENTORY",				SLOT_TYPE_INVENTORY);
	PyModule_AddIntConstant(poModule, "SLOT_TYPE_SKILL",					SLOT_TYPE_SKILL);
	// Special indecies for client
	PyModule_AddIntConstant(poModule, "SLOT_TYPE_SHOP",						SLOT_TYPE_SHOP);
	PyModule_AddIntConstant(poModule, "SLOT_TYPE_EXCHANGE_OWNER",			SLOT_TYPE_EXCHANGE_OWNER);
	PyModule_AddIntConstant(poModule, "SLOT_TYPE_EXCHANGE_TARGET",			SLOT_TYPE_EXCHANGE_TARGET);
	PyModule_AddIntConstant(poModule, "SLOT_TYPE_QUICK_SLOT", SLOT_TYPE_QUICK_SLOT);
	PyModule_AddIntConstant(poModule, "SLOT_TYPE_SAFEBOX",					SLOT_TYPE_SAFEBOX);
	PyModule_AddIntConstant(poModule, "SLOT_TYPE_PRIVATE_SHOP",				SLOT_TYPE_PRIVATE_SHOP);
	PyModule_AddIntConstant(poModule, "SLOT_TYPE_MALL",						SLOT_TYPE_MALL);
	PyModule_AddIntConstant(poModule, "SLOT_TYPE_EMOTION",					SLOT_TYPE_EMOTION);
	PyModule_AddIntConstant(poModule, "SLOT_TYPE_DRAGON_SOUL_INVENTORY",	SLOT_TYPE_DRAGON_SOUL_INVENTORY);
	PyModule_AddIntConstant(poModule, "RESERVED_WINDOW",					RESERVED_WINDOW);
	PyModule_AddIntConstant(poModule, "INVENTORY",							INVENTORY);
	PyModule_AddIntConstant(poModule, "EQUIPMENT",							EQUIPMENT);
	PyModule_AddIntConstant(poModule, "SAFEBOX",							SAFEBOX);
	PyModule_AddIntConstant(poModule, "MALL",								MALL);
	PyModule_AddIntConstant(poModule, "DRAGON_SOUL_INVENTORY",				DRAGON_SOUL_INVENTORY);
	PyModule_AddIntConstant(poModule, "GROUND",								GROUND);
	PyModule_AddIntConstant(poModule, "ITEM_MONEY",					-1);

	PyModule_AddIntConstant(poModule, "SKILL_SLOT_COUNT",			SKILL_MAX_NUM);

	PyModule_AddIntConstant(poModule, "EFFECT_PICK",				CPythonPlayer::EFFECT_PICK);

	PyModule_AddIntConstant(poModule, "METIN_SOCKET_TYPE_NONE",					CPythonPlayer::METIN_SOCKET_TYPE_NONE);
	PyModule_AddIntConstant(poModule, "METIN_SOCKET_TYPE_SILVER",				CPythonPlayer::METIN_SOCKET_TYPE_SILVER);
	PyModule_AddIntConstant(poModule, "METIN_SOCKET_TYPE_GOLD",					CPythonPlayer::METIN_SOCKET_TYPE_GOLD);
	PyModule_AddIntConstant(poModule, "METIN_SOCKET_MAX_NUM",					ITEM_SOCKET_SLOT_MAX_NUM);
	// refactored attribute slot begin
	PyModule_AddIntConstant(poModule, "ATTRIBUTE_SLOT_NORM_NUM",					ITEM_ATTRIBUTE_SLOT_NORM_NUM);
	PyModule_AddIntConstant(poModule, "ATTRIBUTE_SLOT_RARE_NUM",					ITEM_ATTRIBUTE_SLOT_RARE_NUM);
	PyModule_AddIntConstant(poModule, "ATTRIBUTE_SLOT_NORM_START",					ITEM_ATTRIBUTE_SLOT_NORM_START);
	PyModule_AddIntConstant(poModule, "ATTRIBUTE_SLOT_NORM_END",					ITEM_ATTRIBUTE_SLOT_NORM_END);
	PyModule_AddIntConstant(poModule, "ATTRIBUTE_SLOT_RARE_START",					ITEM_ATTRIBUTE_SLOT_RARE_START);
	PyModule_AddIntConstant(poModule, "ATTRIBUTE_SLOT_RARE_END",					ITEM_ATTRIBUTE_SLOT_RARE_END);
	PyModule_AddIntConstant(poModule, "ATTRIBUTE_SLOT_MAX_NUM",						ITEM_ATTRIBUTE_SLOT_MAX_NUM);
	// refactored attribute slot end

	PyModule_AddIntConstant(poModule, "REFINE_CANT",							REFINE_CANT);
	PyModule_AddIntConstant(poModule, "REFINE_OK",								REFINE_OK);
	PyModule_AddIntConstant(poModule, "REFINE_ALREADY_MAX_SOCKET_COUNT",		REFINE_ALREADY_MAX_SOCKET_COUNT);
	PyModule_AddIntConstant(poModule, "REFINE_NEED_MORE_GOOD_SCROLL",			REFINE_NEED_MORE_GOOD_SCROLL);
	PyModule_AddIntConstant(poModule, "REFINE_CANT_MAKE_SOCKET_ITEM",			REFINE_CANT_MAKE_SOCKET_ITEM);
	PyModule_AddIntConstant(poModule, "REFINE_NOT_NEXT_GRADE_ITEM",				REFINE_NOT_NEXT_GRADE_ITEM);
	PyModule_AddIntConstant(poModule, "REFINE_CANT_REFINE_METIN_TO_EQUIPMENT",	REFINE_CANT_REFINE_METIN_TO_EQUIPMENT);
	PyModule_AddIntConstant(poModule, "REFINE_CANT_REFINE_ROD",					REFINE_CANT_REFINE_ROD);
	PyModule_AddIntConstant(poModule, "ATTACH_METIN_CANT",						ATTACH_METIN_CANT);
	PyModule_AddIntConstant(poModule, "ATTACH_METIN_OK",						ATTACH_METIN_OK);
	PyModule_AddIntConstant(poModule, "ATTACH_METIN_NOT_MATCHABLE_ITEM",		ATTACH_METIN_NOT_MATCHABLE_ITEM);
	PyModule_AddIntConstant(poModule, "ATTACH_METIN_NO_MATCHABLE_SOCKET",		ATTACH_METIN_NO_MATCHABLE_SOCKET);
	PyModule_AddIntConstant(poModule, "ATTACH_METIN_NOT_EXIST_GOLD_SOCKET",		ATTACH_METIN_NOT_EXIST_GOLD_SOCKET);
	PyModule_AddIntConstant(poModule, "ATTACH_METIN_CANT_ATTACH_TO_EQUIPMENT",	ATTACH_METIN_CANT_ATTACH_TO_EQUIPMENT);
	PyModule_AddIntConstant(poModule, "DETACH_METIN_CANT",						DETACH_METIN_CANT);
	PyModule_AddIntConstant(poModule, "DETACH_METIN_OK",						DETACH_METIN_OK);

	// Party
	PyModule_AddIntConstant(poModule, "PARTY_STATE_NORMAL",						CPythonPlayer::PARTY_ROLE_NORMAL);
	PyModule_AddIntConstant(poModule, "PARTY_STATE_LEADER",						CPythonPlayer::PARTY_ROLE_LEADER);
	PyModule_AddIntConstant(poModule, "PARTY_STATE_ATTACKER",					CPythonPlayer::PARTY_ROLE_ATTACKER);
	PyModule_AddIntConstant(poModule, "PARTY_STATE_TANKER",						CPythonPlayer::PARTY_ROLE_TANKER);
	PyModule_AddIntConstant(poModule, "PARTY_STATE_BUFFER",						CPythonPlayer::PARTY_ROLE_BUFFER);
	PyModule_AddIntConstant(poModule, "PARTY_STATE_SKILL_MASTER",				CPythonPlayer::PARTY_ROLE_SKILL_MASTER);
	PyModule_AddIntConstant(poModule, "PARTY_STATE_BERSERKER",					CPythonPlayer::PARTY_ROLE_BERSERKER);
	PyModule_AddIntConstant(poModule, "PARTY_STATE_DEFENDER",					CPythonPlayer::PARTY_ROLE_DEFENDER);
	PyModule_AddIntConstant(poModule, "PARTY_STATE_MAX_NUM",					CPythonPlayer::PARTY_ROLE_MAX_NUM);

	// Skill Index
	PyModule_AddIntConstant(poModule, "SKILL_INDEX_TONGSOL",		c_iSkillIndex_Tongsol);
	PyModule_AddIntConstant(poModule, "SKILL_INDEX_FISHING",		c_iSkillIndex_Fishing);
	PyModule_AddIntConstant(poModule, "SKILL_INDEX_MINING",			c_iSkillIndex_Mining);
	PyModule_AddIntConstant(poModule, "SKILL_INDEX_MAKING",			c_iSkillIndex_Making);
	PyModule_AddIntConstant(poModule, "SKILL_INDEX_COMBO",			c_iSkillIndex_Combo);
	PyModule_AddIntConstant(poModule, "SKILL_INDEX_LANGUAGE1",		c_iSkillIndex_Language1);
	PyModule_AddIntConstant(poModule, "SKILL_INDEX_LANGUAGE2",		c_iSkillIndex_Language2);
	PyModule_AddIntConstant(poModule, "SKILL_INDEX_LANGUAGE3",		c_iSkillIndex_Language3);
	PyModule_AddIntConstant(poModule, "SKILL_INDEX_POLYMORPH",		c_iSkillIndex_Polymorph);
	PyModule_AddIntConstant(poModule, "SKILL_INDEX_RIDING",			c_iSkillIndex_Riding);
	PyModule_AddIntConstant(poModule, "SKILL_INDEX_SUMMON",			c_iSkillIndex_Summon);

	// PK Mode
	PyModule_AddIntConstant(poModule, "PK_MODE_PEACE",				PK_MODE_PEACE);
	PyModule_AddIntConstant(poModule, "PK_MODE_REVENGE",			PK_MODE_REVENGE);
	PyModule_AddIntConstant(poModule, "PK_MODE_FREE",				PK_MODE_FREE);
	PyModule_AddIntConstant(poModule, "PK_MODE_PROTECT",			PK_MODE_PROTECT);
	PyModule_AddIntConstant(poModule, "PK_MODE_GUILD",				PK_MODE_GUILD);
	PyModule_AddIntConstant(poModule, "PK_MODE_MAX_NUM",			PK_MODE_MAX_NUM);


#ifdef ENABLE_FEATURES_REFINE_SYSTEM
	// Refine Percentage System

	PyModule_AddIntConstant(poModule, "REFINE_PERCENTAGE_LOW", REFINE_PERCENTAGE_LOW);
	PyModule_AddIntConstant(poModule, "REFINE_PERCENTAGE_MEDIUM", REFINE_PERCENTAGE_MEDIUM);
	PyModule_AddIntConstant(poModule, "REFINE_PERCENTAGE_EXTRA", REFINE_PERCENTAGE_EXTRA);

	PyModule_AddIntConstant(poModule, "REFINE_VNUM_POTION_LOW", REFINE_VNUM_POTION_LOW);
	PyModule_AddIntConstant(poModule, "REFINE_VNUM_POTION_MEDIUM", REFINE_VNUM_POTION_MEDIUM);
	PyModule_AddIntConstant(poModule, "REFINE_VNUM_POTION_EXTRA", REFINE_VNUM_POTION_EXTRA);
#endif



	// Block Mode
	PyModule_AddIntConstant(poModule, "BLOCK_EXCHANGE",				BLOCK_EXCHANGE);
	PyModule_AddIntConstant(poModule, "BLOCK_PARTY",				BLOCK_PARTY_INVITE);
	PyModule_AddIntConstant(poModule, "BLOCK_GUILD",				BLOCK_GUILD_INVITE);
	PyModule_AddIntConstant(poModule, "BLOCK_WHISPER",				BLOCK_WHISPER);
	PyModule_AddIntConstant(poModule, "BLOCK_FRIEND",				BLOCK_MESSENGER_INVITE);
	PyModule_AddIntConstant(poModule, "BLOCK_PARTY_REQUEST",		BLOCK_PARTY_REQUEST);

	// Party
	PyModule_AddIntConstant(poModule, "PARTY_EXP_NON_DISTRIBUTION",		PARTY_EXP_DISTRIBUTION_NON_PARITY);
	PyModule_AddIntConstant(poModule, "PARTY_EXP_DISTRIBUTION_PARITY",	PARTY_EXP_DISTRIBUTION_PARITY);

	// Emotion
	PyModule_AddIntConstant(poModule, "EMOTION_CLAP",			EMOTION_CLAP);
	PyModule_AddIntConstant(poModule, "EMOTION_CHEERS_1",		EMOTION_CHEERS_1);
	PyModule_AddIntConstant(poModule, "EMOTION_CHEERS_2",		EMOTION_CHEERS_2);
	PyModule_AddIntConstant(poModule, "EMOTION_DANCE_1",		EMOTION_DANCE_1);
	PyModule_AddIntConstant(poModule, "EMOTION_DANCE_2",		EMOTION_DANCE_2);
	PyModule_AddIntConstant(poModule, "EMOTION_DANCE_3",		EMOTION_DANCE_3);
	PyModule_AddIntConstant(poModule, "EMOTION_DANCE_4",		EMOTION_DANCE_4);
	PyModule_AddIntConstant(poModule, "EMOTION_DANCE_5",		EMOTION_DANCE_5);
	PyModule_AddIntConstant(poModule, "EMOTION_DANCE_6",		EMOTION_DANCE_6);				// PSY 강남스타일
	PyModule_AddIntConstant(poModule, "EMOTION_CONGRATULATION",	EMOTION_CONGRATULATION);
	PyModule_AddIntConstant(poModule, "EMOTION_FORGIVE",		EMOTION_FORGIVE);
	PyModule_AddIntConstant(poModule, "EMOTION_ANGRY",			EMOTION_ANGRY);
	PyModule_AddIntConstant(poModule, "EMOTION_ATTRACTIVE",		EMOTION_ATTRACTIVE);
	PyModule_AddIntConstant(poModule, "EMOTION_SAD",			EMOTION_SAD);
	PyModule_AddIntConstant(poModule, "EMOTION_SHY",			EMOTION_SHY);
	PyModule_AddIntConstant(poModule, "EMOTION_CHEERUP",		EMOTION_CHEERUP);
	PyModule_AddIntConstant(poModule, "EMOTION_BANTER",			EMOTION_BANTER);
	PyModule_AddIntConstant(poModule, "EMOTION_JOY",			EMOTION_JOY);

	PyModule_AddIntConstant(poModule, "EMOTION_KISS",			EMOTION_KISS);
	PyModule_AddIntConstant(poModule, "EMOTION_FRENCH_KISS",	EMOTION_FRENCH_KISS);
	PyModule_AddIntConstant(poModule, "EMOTION_SLAP",			EMOTION_SLAP);

	PyModule_AddIntConstant(poModule, "EMOTION_SELFIE",	EMOTION_SELFIE);
	PyModule_AddIntConstant(poModule, "EMOTION_DANCE_7",	EMOTION_DANCE_7);
	PyModule_AddIntConstant(poModule, "EMOTION_DOZE",	EMOTION_DOZE);
	PyModule_AddIntConstant(poModule, "EMOTION_EXERCISE",	EMOTION_EXERCISE);
	PyModule_AddIntConstant(poModule, "EMOTION_PUSHUP",	EMOTION_PUSHUP);

	//// 자동물약 타입
	PyModule_AddIntConstant(poModule, "AUTO_POTION_TYPE_HP",	CPythonPlayer::AUTO_POTION_TYPE_HP);
	PyModule_AddIntConstant(poModule, "AUTO_POTION_TYPE_SP",	CPythonPlayer::AUTO_POTION_TYPE_SP);

	// 용혼석
	PyModule_AddIntConstant(poModule, "DRAGON_SOUL_PAGE_SIZE",	c_DragonSoul_Inventory_Box_Size);
	PyModule_AddIntConstant(poModule, "DRAGON_SOUL_PAGE_COUNT",	DRAGON_SOUL_GRADE_MAX);
	PyModule_AddIntConstant(poModule, "DRAGON_SOUL_SLOT_COUNT",	c_DragonSoul_Inventory_Count);
	PyModule_AddIntConstant(poModule, "DRAGON_SOUL_EQUIPMENT_SLOT_START",	c_DragonSoul_Equip_Start);
	PyModule_AddIntConstant(poModule, "DRAGON_SOUL_EQUIPMENT_PAGE_COUNT",	DS_DECK_MAX_NUM);
	PyModule_AddIntConstant(poModule, "DRAGON_SOUL_EQUIPMENT_FIRST_SIZE",	c_DragonSoul_Equip_Slot_Max);

	// 용혼석 개량창
	PyModule_AddIntConstant(poModule, "DRAGON_SOUL_REFINE_CLOSE",	DS_SUB_HEADER_CLOSE);
	PyModule_AddIntConstant(poModule, "DS_SUB_HEADER_DO_UPGRADE",	DS_SUB_HEADER_DO_UPGRADE);
	PyModule_AddIntConstant(poModule, "DS_SUB_HEADER_DO_IMPROVEMENT",	DS_SUB_HEADER_DO_IMPROVEMENT);
	PyModule_AddIntConstant(poModule, "DS_SUB_HEADER_DO_REFINE",	DS_SUB_HEADER_DO_REFINE);
#ifdef INGAME_WIKI
	PyModule_AddIntConstant(poModule, "MAIN_RACE_WARRIOR_M",		MAIN_RACE_WARRIOR_M);
	PyModule_AddIntConstant(poModule, "MAIN_RACE_ASSASSIN_W",		MAIN_RACE_ASSASSIN_W);
	PyModule_AddIntConstant(poModule, "MAIN_RACE_SURA_M",			MAIN_RACE_SURA_M);
	PyModule_AddIntConstant(poModule, "MAIN_RACE_SHAMAN_W",			MAIN_RACE_SHAMAN_W);
	PyModule_AddIntConstant(poModule, "MAIN_RACE_WARRIOR_W",		MAIN_RACE_WARRIOR_W);
	PyModule_AddIntConstant(poModule, "MAIN_RACE_ASSASSIN_M",		MAIN_RACE_ASSASSIN_M);
	PyModule_AddIntConstant(poModule, "MAIN_RACE_SURA_W",			MAIN_RACE_SURA_W);
	PyModule_AddIntConstant(poModule, "MAIN_RACE_SHAMAN_M",			MAIN_RACE_SHAMAN_M);
#ifdef INGAME_WIKI_WOLFMAN
	PyModule_AddIntConstant(poModule, "MAIN_RACE_WOLFMAN_M",		MAIN_RACE_WOLFMAN_M);
#endif
	PyModule_AddIntConstant(poModule, "MAIN_RACE_MAX_NUM",			MAIN_RACE_MAX_NUM);
#endif
#ifdef ENABLE_EXTRA_INVENTORY
	// EXTRA INVENTORY
	PyModule_AddIntConstant(poModule, "EXTRA_INVENTORY_PAGE_COLUMN",		c_Extra_Inventory_Page_Column);
	PyModule_AddIntConstant(poModule, "EXTRA_INVENTORY_PAGE_ROW",			c_Extra_Inventory_Page_Row);
	PyModule_AddIntConstant(poModule, "EXTRA_INVENTORY_PAGE_SIZE",			c_Extra_Inventory_Page_Size);
	PyModule_AddIntConstant(poModule, "EXTRA_INVENTORY_PAGE_COUNT",			c_Extra_Inventory_Page_Count);
	PyModule_AddIntConstant(poModule, "EXTRA_INVENTORY_SLOT_COUNT",			c_Extra_Inventory_Count);
	PyModule_AddIntConstant(poModule, "EXTRA_INVENTORY_CATEGORY_COUNT",		c_Extra_Inventory_Category_Count);
	PyModule_AddIntConstant(poModule, "EXTRA_INVENTORY",					EXTRA_INVENTORY);
	PyModule_AddIntConstant(poModule, "SLOT_TYPE_EXTRA_INVENTORY",			SLOT_TYPE_EXTRA_INVENTORY);
#endif
#ifdef ENABLE_SWITCHBOT_WORLDARD
	PyModule_AddIntConstant(poModule, "SWITCHBOT", SWITCHBOT);
	PyModule_AddIntConstant(poModule, "SLOT_TYPE_SWITCHBOT", SLOT_TYPE_SWITCHBOT);
#endif
#if defined(ENABLE_PETSKIN)
	PyModule_AddIntConstant(poModule, "COSTUME_PETSKIN_SLOT", c_Costume_PetSkin);
#endif
#if defined(ENABLE_MOUNTSKIN)
	PyModule_AddIntConstant(poModule, "COSTUME_MOUNTSKIN_SLOT", c_Costume_MountSkin);
#endif
#ifdef ENABLE_COSTUME_EFFECT
	PyModule_AddIntConstant(poModule, "COSTUME_EFFECT_BODY_SLOT", c_Costume_EffectBody);
	PyModule_AddIntConstant(poModule, "COSTUME_EFFECT_WEAPON_SLOT", c_Costume_EffectWeapon);
#endif
#ifdef WJ_ENABLE_TRADABLE_ICON
	PyModule_AddIntConstant(poModule, "ON_TOP_WND_NONE",			ON_TOP_WND_NONE);
	PyModule_AddIntConstant(poModule, "ON_TOP_WND_SHOP",			ON_TOP_WND_SHOP);
	PyModule_AddIntConstant(poModule, "ON_TOP_WND_EXCHANGE",		ON_TOP_WND_EXCHANGE);
	PyModule_AddIntConstant(poModule, "ON_TOP_WND_SAFEBOX",			ON_TOP_WND_SAFEBOX);
	PyModule_AddIntConstant(poModule, "ON_TOP_WND_PRIVATE_SHOP",	ON_TOP_WND_PRIVATE_SHOP);
	PyModule_AddIntConstant(poModule, "ON_TOP_WND_ITEM_COMB",		ON_TOP_WND_ITEM_COMB);
	PyModule_AddIntConstant(poModule, "ON_TOP_WND_PET_FEED",		ON_TOP_WND_PET_FEED);
#if defined(ENABLE_NEW_FISH_EVENT)
	PyModule_AddIntConstant(poModule, "ON_TOP_WND_FISH_EVENT", ON_TOP_WND_FISH_EVENT);
#endif
#if defined(ENABLE_CHANGELOOK)
	PyModule_AddIntConstant(poModule, "ON_TOP_WND_CHANGE_LOOK", ON_TOP_WND_CHANGE_LOOK);
#endif
#if defined(USE_ATTR_6TH_7TH)
	PyModule_AddIntConstant(poModule, "ON_TOP_WND_ATTR67", ON_TOP_WND_ATTR67);
#endif
#endif
#if defined(ENABLE_NEW_FISH_EVENT)
	PyModule_AddIntConstant (poModule, "SLOT_TYPE_FISH_EVENT", SLOT_TYPE_FISH_EVENT);
#endif
#ifdef ENABLE_REWARD_SYSTEM
	PyModule_AddIntConstant(poModule, "REWARD_105", REWARD_105);
	PyModule_AddIntConstant(poModule, "REWARD_PET_115", REWARD_PET_115);
	PyModule_AddIntConstant(poModule, "REWARD_120", REWARD_120);
	PyModule_AddIntConstant(poModule, "REWARD_LEGENDARY_SKILL", REWARD_LEGENDARY_SKILL);
	PyModule_AddIntConstant(poModule, "REWARD_LEGENDARY_SKILL_SET", REWARD_LEGENDARY_SKILL_SET);
	PyModule_AddIntConstant(poModule, "REWARD_KILL_DRAGON", REWARD_KILL_DRAGON);
	PyModule_AddIntConstant(poModule, "REWARD_KILL_RAZADOR", REWARD_KILL_RAZADOR);
	PyModule_AddIntConstant(poModule, "REWARD_KILL_NEMERE", REWARD_KILL_NEMERE);
	PyModule_AddIntConstant(poModule, "REWARD_OFFLINESHOP_SLOT", REWARD_OFFLINESHOP_SLOT);
	PyModule_AddIntConstant(poModule, "REWARD_INVENTORY_SLOT", REWARD_INVENTORY_SLOT);
	PyModule_AddIntConstant(poModule, "REWARD_AVERAGE", REWARD_AVERAGE);
	PyModule_AddIntConstant(poModule, "REWARD_ELEMENT", REWARD_ELEMENT);
	PyModule_AddIntConstant(poModule, "REWARD_BATTLEPASS", REWARD_BATTLEPASS);
	PyModule_AddIntConstant(poModule, "REWARD_CUSTOM_SASH", REWARD_CUSTOM_SASH);
	PyModule_AddIntConstant(poModule, "REWARD_AURA", REWARD_AURA);
	PyModule_AddIntConstant(poModule, "REWARD_ENERGY", REWARD_ENERGY);
	PyModule_AddIntConstant(poModule, "REWARD_112_BIO", REWARD_112_BIO);
	PyModule_AddIntConstant(poModule, "REWARD_120_BIO", REWARD_120_BIO);
	PyModule_AddIntConstant(poModule, "REWARD_LEADER_SHIP", REWARD_LEADER_SHIP);
	PyModule_AddIntConstant(poModule, "REWARD_BUFFI_LEGENDARY_SKILL", REWARD_BUFFI_LEGENDARY_SKILL);
	PyModule_AddIntConstant(poModule, "REWARD_MAX", REWARD_MAX);
#endif
#if defined(ENABLE_EVENT_MANAGER)
	PyModule_AddIntConstant(poModule, "BONUS_EVENT", BONUS_EVENT);
	PyModule_AddIntConstant(poModule, "DOUBLE_BOSS_LOOT_EVENT", DOUBLE_BOSS_LOOT_EVENT);
	PyModule_AddIntConstant(poModule, "DOUBLE_METIN_LOOT_EVENT", DOUBLE_METIN_LOOT_EVENT);
	PyModule_AddIntConstant(poModule, "DOUBLE_MISSION_BOOK_EVENT", DOUBLE_MISSION_BOOK_EVENT);
	PyModule_AddIntConstant(poModule, "DUNGEON_COOLDOWN_EVENT", DUNGEON_COOLDOWN_EVENT);
	PyModule_AddIntConstant(poModule, "DUNGEON_TICKET_LOOT_EVENT", DUNGEON_TICKET_LOOT_EVENT);
	PyModule_AddIntConstant(poModule, "EMPIRE_WAR_EVENT", EMPIRE_WAR_EVENT);
	PyModule_AddIntConstant(poModule, "MOONLIGHT_EVENT", MOONLIGHT_EVENT);
	PyModule_AddIntConstant(poModule, "TOURNAMENT_EVENT", TOURNAMENT_EVENT);
	PyModule_AddIntConstant(poModule, "WHELL_OF_FORTUNE_EVENT", WHELL_OF_FORTUNE_EVENT);
#if defined(ENABLE_HALLOWEEN_EVENT_2022)
	PyModule_AddIntConstant(poModule, "HALLOWEEN_EVENT", HALLOWEEN_EVENT);
#endif
	PyModule_AddIntConstant(poModule, "NPC_SEARCH_EVENT", NPC_SEARCH_EVENT);
	PyModule_AddIntConstant(poModule, "EXP_EVENT", EXP_EVENT);
	PyModule_AddIntConstant(poModule, "ITEM_DROP_EVENT", ITEM_DROP_EVENT);
	PyModule_AddIntConstant(poModule, "YANG_DROP_EVENT", YANG_DROP_EVENT);
#if defined(ENABLE_NEW_FISH_EVENT)
	PyModule_AddIntConstant(poModule, "JIGSAW_EVENT", JIGSAW_EVENT);
#endif
#if defined(ENABLE_LETTERS_EVENT)
	PyModule_AddIntConstant(poModule, "LETTERS_EVENT", LETTERS_EVENT);
#endif
#if defined(ENABLE_OKEY_CARD_GAME)
	PyModule_AddIntConstant(poModule, "OKEY_CARD_EVENT", OKEY_CARD_EVENT);
#endif
#endif
	PyModule_AddIntConstant(poModule, "WND_EQUIP_NORMAL", WND_EQUIP_NORMAL);
	PyModule_AddIntConstant(poModule, "WND_EQUIP_COSTUMES", WND_EQUIP_COSTUMES);
	PyModule_AddIntConstant(poModule, "WND_EQUIP_MAX", WND_EQUIP_MAX);
#ifdef USE_AUTO_HUNT
    PyModule_AddIntConstant(poModule, "SLOT_TYPE_AUTO", SLOT_TYPE_AUTO);
    PyModule_AddIntConstant(poModule, "AUTO_SKILL_SLOT_MAX", AUTO_SKILL_SLOT_MAX);
    PyModule_AddIntConstant(poModule, "AUTO_POTION_SLOT_START", AUTO_POTION_SLOT_START);
    PyModule_AddIntConstant(poModule, "AUTO_POTION_SLOT_MAX", AUTO_POTION_SLOT_MAX);
    PyModule_AddIntConstant(poModule, "USHRT_MAX", USHRT_MAX);
#endif
}

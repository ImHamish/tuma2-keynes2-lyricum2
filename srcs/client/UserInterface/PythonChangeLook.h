#pragma once

#if defined(ENABLE_CHANGELOOK)
#include "Packet.h"

class CPythonChangeLook : public CSingleton<CPythonChangeLook>
{
public:

	enum EChangeLook
	{
		WINDOW_MAX_MATERIALS = 3,
		LIMIT_RANGE = 1000,
	};

	int64_t dwCost;
	typedef std::vector<TChangeLookMaterial> TChangeLookMaterials;

public:
	CPythonChangeLook();
	virtual ~CPythonChangeLook();

	void Clear();
	void AddMaterial(BYTE bPos, TItemPos tPos);
	void RemoveMaterial(BYTE bPos);
	void RemoveAllMaterials();
	int64_t GetCost() { return dwCost; }
	void SetCost(int64_t dwCostR) { dwCost = dwCostR; }
	bool GetAttachedItem(BYTE bPos, BYTE& bHere, WORD& wCell);
	BOOL IsCleanScroll(DWORD dwItemVnum);
	BOOL IsTransmutationTicket(DWORD dwItemVnum);

protected:
	TChangeLookMaterials m_vMaterials;
};
#endif

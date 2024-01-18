#pragma once

#include "ItemData.h"

class CItemManager : public CSingleton<CItemManager>
{
#ifdef USE_MULTIPLE_OPENING
    public:
        int32_t GetItemType(const uint32_t vnum);
#endif

	public:
		enum EItemDescCol
		{
			ITEMDESC_COL_VNUM,
			ITEMDESC_COL_NAME,
			ITEMDESC_COL_DESC,
			ITEMDESC_COL_SUMM,
			ITEMDESC_COL_NUM,
		};

#ifdef ENABLE_AURA_SYSTEM
	public:
		enum EAuraScaleCol {
			AURA_SCALE_COL_VNUM,
			AURA_SCALE_COL_JOB,
			AURA_SCALE_COL_SEX,
			AURA_SCALE_COL_MESH_SCALE_X,
			AURA_SCALE_COL_MESH_SCALE_Y,
			AURA_SCALE_COL_MESH_SCALE_Z,
			AURA_SCALE_COL_PARTICLE_SCALE,
			AURA_SCALE_COL_NUM,
		};

		bool LoadAuraScale(const char*);
#endif

#ifdef ENABLE_ACCE_SYSTEM
		enum EItemScaleColumn
		{
			ITEMSCALE_VNUM,
			ITEMSCALE_JOB,
			ITEMSCALE_SEX,
			ITEMSCALE_SCALE_X,
			ITEMSCALE_SCALE_Y,
			ITEMSCALE_SCALE_Z,
			ITEMSCALE_POSITION_X,
			ITEMSCALE_POSITION_Y,
			ITEMSCALE_POSITION_Z,
			ITEMSCALE_NUM,
		};
#endif




	public:
		typedef std::map<DWORD, CItemData*> TItemMap;
		typedef std::map<std::string, CItemData*> TItemNameMap;
#ifdef INGAME_WIKI
	public:
		typedef std::vector<CItemData*> TItemVec;
		typedef std::vector<DWORD> TItemNumVec;
	
	public:
		void WikiAddVnumToBlacklist(DWORD vnum)
		{
			auto it = m_ItemMap.find(vnum);
			if (it != m_ItemMap.end())
				it->second->SetBlacklisted(true);
		};
		
		TItemNumVec* WikiGetLastItems()
		{
			return &m_tempItemVec;
		}
		
		bool IsUniqueRealItem(const uint32_t) const;
		bool								CanLoadWikiItem(DWORD dwVnum);
		DWORD							GetWikiItemStartRefineVnum(DWORD dwVnum);
		std::string							GetWikiItemBaseRefineName(DWORD dwVnum);
		size_t								WikiLoadClassItems(BYTE classType, DWORD raceFilter);
		std::tuple<const char*, int>	SelectByNamePart(const char * namePart);
	
	protected:
		TItemNumVec m_tempItemVec;
	
	private:
		bool IsFilteredAntiflag(CItemData* itemData, DWORD raceFilter);
#endif
	public:
		CItemManager();
		virtual ~CItemManager();

		void			Destroy();

		BOOL			SelectItemData(DWORD dwIndex);
		CItemData *		GetSelectedItemDataPointer();

#ifdef ENABLE_ITEM_EXTRA_PROTO
		CItemData::TItemExtraProto* GetSelectedExtraProto();
#endif

		BOOL			GetItemDataPointer(DWORD dwItemID, CItemData ** ppItemData);

		/////
		bool			LoadItemDesc(const char* c_szFileName);
		bool			LoadItemList(const char* c_szFileName);
		bool			LoadItemTable(const char* c_szFileName);
#if defined(ENABLE_SHINING_SYSTEM)
		bool LoadShiningTable(const char* c_szFileName);
#endif
		CItemData* MakeItemData(DWORD dwIndex);
#ifdef ENABLE_ITEM_EXTRA_PROTO
		bool			LoadItemExtraProto(std::string filename);
		CItemData::TItemExtraProto* GetExtraProto(DWORD vnum);
#endif
#ifdef __ENABLE_NEW_OFFLINESHOP__
		void			GetItemsNameMap(std::map<DWORD, std::string>& inMap);
#endif

#ifdef ENABLE_ACCE_SYSTEM
		bool LoadItemScale(const char* path);
#endif
#ifdef INGAME_WIKI
		bool			CanIncrRefineLevel();
#endif

	protected:
		TItemMap m_ItemMap;
		std::vector<CItemData*>  m_vec_ItemRange;
		CItemData * m_pSelectedItemData;
#ifdef ENABLE_ITEM_EXTRA_PROTO
		std::map<DWORD, CItemData::TItemExtraProto> m_map_extraProto;
		CItemData::TItemExtraProto* m_pSelectedExtraProto;
#endif
};

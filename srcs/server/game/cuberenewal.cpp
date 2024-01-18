
#define _cube_cpp_

#include "stdafx.h"
#include "constants.h"
#include "utils.h"
#include "log.h"
#include "char.h"
#include "locale_service.h"
#include "item.h"
#include "item_manager.h"
#include "questmanager.h"
#include <sstream>
#include "packet.h"
#include "desc_client.h"
#include "config.h"

#ifdef __ENABLE_NEW_OFFLINESHOP__
#include "new_offlineshop.h"
#include "new_offlineshop_manager.h"
#endif
#ifdef ENABLE_STOLE_COSTUME
#include "../common/stole_length.h"
#endif

static std::vector<CUBE_RENEWAL_DATA*>	s_cube_proto;

typedef std::vector<CUBE_RENEWAL_VALUE>	TCubeValueVector;

struct SCubeMaterialInfo
{
	SCubeMaterialInfo()
	{
		bHaveComplicateMaterial = false;
	};

	CUBE_RENEWAL_VALUE			reward;							// º¸»ó?? ¹¹³?
	TCubeValueVector	material;						// ?ç·áµé?º ¹¹³?
	unsigned long long				gold;							// µ·?º ¾ó¸¶µå³?
	int 				percent;
	std::string		category;
	TCubeValueVector	complicateMaterial;				// º¹?â??-_- ?ç·áµé
#if defined(ENABLE_GAYA_RENEWAL)
	DWORD 				gaya;
#endif
#ifdef ENABLE_CUBE_RENEWAL_COPY_WORLDARD
	DWORD 				allowCopy;
#endif

	std::string			infoText;		
	bool				bHaveComplicateMaterial;		//
};

struct SItemNameAndLevel
{
	SItemNameAndLevel() { level = 0; }

	std::string		name;
	int				level;
};


typedef std::vector<SCubeMaterialInfo>								TCubeResultList;
typedef std::unordered_map<DWORD, TCubeResultList>				TCubeMapByNPC;				// °¢°¢?? NPCº°·? ¾î¶² °? ¸¸µé ¼ö ??°í ?ç·á°¡ ¹º?ö...

TCubeMapByNPC cube_info_map;


static bool FN_check_valid_npc( WORD vnum )
{
	for ( std::vector<CUBE_RENEWAL_DATA*>::iterator iter = s_cube_proto.begin(); iter != s_cube_proto.end(); iter++ )
	{
		if ( std::find((*iter)->npc_vnum.begin(), (*iter)->npc_vnum.end(), vnum) != (*iter)->npc_vnum.end() )
			return true;
	}

	return false;
}


static bool FN_check_cube_data (CUBE_RENEWAL_DATA *cube_data)
{
	DWORD	i = 0;
	DWORD	end_index = 0;

	end_index = cube_data->npc_vnum.size();
	for (i=0; i<end_index; ++i)
	{
		if ( cube_data->npc_vnum[i] == 0 )	return false;
	}

	end_index = cube_data->item.size();
	for (i=0; i<end_index; ++i)
	{
		if ( cube_data->item[i].vnum == 0 )		return false;
		if ( cube_data->item[i].count == 0 )	return false;
	}

	end_index = cube_data->reward.size();
	for (i=0; i<end_index; ++i)
	{
		if ( cube_data->reward[i].vnum == 0 )	return false;
		if ( cube_data->reward[i].count == 0 )	return false;
	}
	return true;
}

static int FN_check_cube_item_vnum_material(const SCubeMaterialInfo& materialInfo, int index)
{
	if ((unsigned int)index <= materialInfo.material.size()){
		return materialInfo.material[index-1].vnum;
	}
	return 0;
}

static int FN_check_cube_item_count_material(const SCubeMaterialInfo& materialInfo,int index)
{
	if ((unsigned int)index <= materialInfo.material.size()){
		return materialInfo.material[index-1].count;
	}

	return 0;
}

CUBE_RENEWAL_DATA::CUBE_RENEWAL_DATA()
{
	this->gold = 0;
#if defined(ENABLE_GAYA_RENEWAL)
	this->gaya = 0;
#endif
	this->category = "WORLDARD";
#ifdef ENABLE_CUBE_RENEWAL_COPY_WORLDARD
	this->allowCopy = 0;
#endif
}

void Cube_init()
{
    if (g_bAuthServer)
    {
        return;
    }

	CUBE_RENEWAL_DATA * p_cube = NULL;
	std::vector<CUBE_RENEWAL_DATA*>::iterator iter;

	char file_name[256+1];
	snprintf(file_name, sizeof(file_name), "%s/cube.txt", LocaleService_GetBasePath().c_str());

	sys_log(0, "Cube_Init %s", file_name);

	for (iter = s_cube_proto.begin(); iter!=s_cube_proto.end(); iter++)
	{
		p_cube = *iter;
		M2_DELETE(p_cube);
	}

	s_cube_proto.clear();

	if (false == Cube_load(file_name))
		sys_err("Cube_Init failed");
}

bool Cube_load (const char *file)
{
	FILE	*fp;


	const char	*value_string;

	char	one_line[256];
	long long		value1, value2;
	const char	*delim = " \t\r\n";
	char	*v, *token_string;
	//char *v1;
	CUBE_RENEWAL_DATA	*cube_data = NULL;
	CUBE_RENEWAL_VALUE	cube_value = {0,0};

	if (0 == file || 0 == file[0])
		return false;

	if ((fp = fopen(file, "r")) == 0)
		return false;

	while (fgets(one_line, 256, fp))
	{
		value1 = value2 = 0;

		if (one_line[0] == '#')
			continue;

		token_string = strtok(one_line, delim);

		if (NULL == token_string)
			continue;

		// set value1, value2
		if ((v = strtok(NULL, delim)))
			str_to_number(value1, v);
		    value_string = v;

		if ((v = strtok(NULL, delim)))
			str_to_number(value2, v);

		TOKEN("section")
		{
			cube_data = M2_NEW CUBE_RENEWAL_DATA;
		}
		else TOKEN("npc")
		{
			cube_data->npc_vnum.push_back((WORD)value1);
		}
		else TOKEN("item")
		{
			cube_value.vnum		= value1;
			cube_value.count	= value2;

			cube_data->item.push_back(cube_value);
		}
		else TOKEN("reward")
		{
			cube_value.vnum		= value1;
			cube_value.count	= value2;

			cube_data->reward.push_back(cube_value);
		}
		else TOKEN("percent")
		{

			cube_data->percent = value1;
		}

		else TOKEN("category")
		{
			cube_data->category = value_string;
		}
#ifdef ENABLE_CUBE_RENEWAL_COPY_WORLDARD
		else TOKEN("allow_copy")
		{
			cube_data->allowCopy = value1;
		}
#endif
#if defined(ENABLE_GAYA_RENEWAL)
		else TOKEN("gaya")
		{
			cube_data->gaya = value1;
		}
#endif
		else TOKEN("gold")
		{
			// ?¦?¶¿¡ ??¿ä?? ±?¾?
			cube_data->gold = value1;
		}
		else TOKEN("end")
		{

			// TODO : check cube data
			if (false == FN_check_cube_data(cube_data))
			{
				M2_DELETE(cube_data);
				continue;
			}
			s_cube_proto.push_back(cube_data);
		}
	}

	fclose(fp);
	return true;
}


SItemNameAndLevel SplitItemNameAndLevelFromName(const std::string& name)
{
	int level = 0;
	SItemNameAndLevel info;
	info.name = name;

	size_t pos = name.find("+");
	
	if (std::string::npos != pos)
	{
		const std::string levelStr = name.substr(pos + 1, name.size() - pos - 1);
		str_to_number(level, levelStr.c_str());

		info.name = name.substr(0, pos);
	}

	info.level = level;

	return info;
};


bool Cube_InformationInitialize()
{
	for (unsigned int i = 0; i < s_cube_proto.size(); ++i)
	{
		CUBE_RENEWAL_DATA* cubeData = s_cube_proto[i];

		const std::vector<CUBE_RENEWAL_VALUE>& rewards = cubeData->reward;

		if (1 != rewards.size())
		{
			sys_err("[CubeInfo] WARNING! Does not support multiple rewards (count: %d)", rewards.size());			
			continue;
		}

		const CUBE_RENEWAL_VALUE& reward = rewards.at(0);
		const WORD& npcVNUM = cubeData->npc_vnum.at(0);
		// bool bComplicate = false;
		
		TCubeMapByNPC& cubeMap = cube_info_map;
		TCubeResultList& resultList = cubeMap[npcVNUM];
		SCubeMaterialInfo materialInfo;

		materialInfo.reward = reward;
		materialInfo.gold = cubeData->gold;
		materialInfo.percent = cubeData->percent;
#if defined(ENABLE_GAYA_RENEWAL)
		materialInfo.gaya = cubeData->gaya;
#endif
#ifdef ENABLE_CUBE_RENEWAL_COPY_WORLDARD
		materialInfo.allowCopy = cubeData->allowCopy;
#endif
		materialInfo.material = cubeData->item;
		materialInfo.category = cubeData->category;

		resultList.push_back(materialInfo);
	}

	return true;
}


void Cube_open (LPCHARACTER ch)
{
	LPCHARACTER	npc;
	npc = ch->GetQuestNPC();

	

	if (NULL==npc)
	{
		if (test_server)
			sys_log(0, "cube_npc is NULL");
		return;
	}

	DWORD npcVNUM = npc->GetRaceNum();

	if ( FN_check_valid_npc(npcVNUM) == false )
	{
		if ( test_server == true )
		{
			sys_log(0, "cube not valid NPC");
		}
		return;
	}


	if (ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCubeOpen()
#ifdef ENABLE_ACCE_SYSTEM
 || ch->IsAcceOpen()
#endif
#ifdef __ENABLE_NEW_OFFLINESHOP__
 || ch->GetOfflineShopGuest() || ch->GetAuctionGuest()
#endif
#if defined(ENABLE_AURA_SYSTEM)
 || ch->IsAuraRefineWindowOpen()
#endif
#ifdef __ATTR_TRANSFER_SYSTEM__
 || ch->IsAttrTransferOpen()
#endif
#if defined(ENABLE_CHANGELOOK)
 || ch->isChangeLookOpened()
#endif
#if defined(USE_ATTR_6TH_7TH)
 || ch->IsOpenAttr67Add()
#endif
	)
	{
#if defined(ENABLE_TEXTS_RENEWAL)
		ch->ChatPacketNew(CHAT_TYPE_INFO, 815, "");
#endif
		return;
	}

	long distance = DISTANCE_APPROX(ch->GetX() - npc->GetX(), ch->GetY() - npc->GetY());

	if (distance >= CUBE_MAX_DISTANCE)
	{
		sys_log(1, "CUBE: TOO_FAR: %s distance %d", ch->GetName(), distance);
		return;
	}


	SendDateCubeRenewalPackets(ch,CUBE_RENEWAL_SUB_HEADER_CLEAR_DATES_RECEIVE);
	SendDateCubeRenewalPackets(ch,CUBE_RENEWAL_SUB_HEADER_DATES_RECEIVE,npcVNUM);
	SendDateCubeRenewalPackets(ch,CUBE_RENEWAL_SUB_HEADER_DATES_LOADING);
	SendDateCubeRenewalPackets(ch,CUBE_RENEWAL_SUB_HEADER_OPEN_RECEIVE);

	ch->SetCubeNpc(npc);
}

void Cube_close(LPCHARACTER ch)
{
	ch->SetCubeNpc(NULL);
}

void Cube_Make(LPCHARACTER ch, int index, int count_item, int index_item_improve)
{
	LPCHARACTER	npc;

	npc = ch->GetQuestNPC();

	if (!ch->IsCubeOpen()) {
		return;
	}

	if (NULL == npc)
	{
		return;
	}

	if (index < 0)
		return;

	if (count_item <= 0)
		return;

	int index_value = 0;
	bool material_check = true;
	bool item_frozen = false;
	LPITEM pItem;
	int iEmptyPos;
#ifdef ENABLE_CUBE_RENEWAL_COPY_WORLDARD
	DWORD copyAttr[ITEM_ATTRIBUTE_MAX_NUM][2];
	DWORD copySocket[ITEM_SOCKET_MAX_NUM];
	bool item_copy_bonus = false;
#endif
	const TCubeResultList& resultList = cube_info_map[npc->GetRaceNum()];
	for (TCubeResultList::const_iterator iter = resultList.begin(); resultList.end() != iter; ++iter)
	{
		if (index_value == index)
		{
			const SCubeMaterialInfo& materialInfo = *iter;

			for (unsigned int i = 0; i < materialInfo.material.size(); ++i)
			{
				if (ch->CountSpecifyItemRenewal(materialInfo.material[i].vnum) < (materialInfo.material[i].count*count_item))
				{
					item_frozen = true;
				}

				if (ch->CountSpecifyItem(materialInfo.material[i].vnum) < (materialInfo.material[i].count*count_item))
				{
					material_check = false;
				}

			}

			if (materialInfo.gold != 0){
				if (ch->GetGold() < (materialInfo.gold*count_item)) {
#if defined(ENABLE_TEXTS_RENEWAL)
					ch->ChatPacketNew(CHAT_TYPE_INFO, 232, "");
#endif
					return;
				}
			}

#if defined(ENABLE_GAYA_RENEWAL)
			if (materialInfo.gaya != 0){
				if ((int32_t)ch->GetGem() < (int32_t)(materialInfo.gaya*count_item))
				{
#if defined(ENABLE_TEXTS_RENEWAL)
					ch->ChatPacketNew(CHAT_TYPE_INFO, 524, "");
#endif
					return;
				}
			}
#endif

			if (item_frozen && material_check)
			{
#if defined(ENABLE_TEXTS_RENEWAL)
				ch->ChatPacketNew(CHAT_TYPE_INFO, 816, "");
#endif
				return;
			}

			if (material_check){
				
				int percent_number;
				int total_items_give = 0;
				
				int porcent_item_improve = 0;

				if (index_item_improve != -1)
				{

					LPITEM item = ch->GetInventoryItem(index_item_improve);
					if(item != NULL)
					{

						if(item->GetCount() <= 40){
							if (materialInfo.percent+item->GetCount() <= 100){
								porcent_item_improve = item->GetCount();
							}

							if(materialInfo.percent < 100)
							{
								if (materialInfo.percent+item->GetCount() > 100){
									porcent_item_improve = 100 - materialInfo.percent;
								}
							}
						}
					}

					if(porcent_item_improve != 0)
					{
						item->SetCount(item->GetCount()-porcent_item_improve);
					}
				}

				for (int i = 0; i < count_item; ++i)
				{
					percent_number = number(1,100);
					if ( percent_number<=materialInfo.percent+porcent_item_improve)
					{
						total_items_give++;
					}
				}

#ifdef ENABLE_CUBE_RENEWAL_COPY_WORLDARD
                if (materialInfo.allowCopy > 0)
                {
                    auto t = ITEM_MANAGER::instance().GetTable(materialInfo.reward.vnum);
                    if (!t)
                    {
                        sys_err("%s: unknown material reward vnum %u.", ch->GetName(), materialInfo.reward.vnum);
                        return;
                    }

                    pItem = ch->FindSpecifyItem(materialInfo.allowCopy);
                    if (pItem && !item_copy_bonus)
                    {
                        const uint8_t itemType = t->bType;
                        switch (itemType)
                        {
                            case ITEM_WEAPON:
                            case ITEM_ARMOR:
                            {
                                if (t->bSubType != pItem->GetSubType())
                                {
                                    break;
                                }

                                for (uint8_t j = 0; j < ITEM_ATTRIBUTE_MAX_NUM; j++)
                                {
                                    copyAttr[j][0] = pItem->GetAttributeType(j);
                                    copyAttr[j][1] = pItem->GetAttributeValue(j);
                                }

                                if (itemType == ITEM_WEAPON || itemType == ITEM_ARMOR)
                                {
                                    for (uint8_t j = 0; j < ITEM_SOCKET_MAX_NUM; j++)
                                    {
                                        copySocket[j] = pItem->GetSocket(j);
                                    }
                                }

                                item_copy_bonus = true;
                                break;
                            }
                            default:
                            {
                                break;
                            }
                        }
                    }

                    if (pItem)
                    {
                        pItem = nullptr;
                    }
                }
#endif

                pItem = ITEM_MANAGER::instance().CreateItem(materialInfo.reward.vnum, materialInfo.reward.count * total_items_give);
                if (!pItem)
                {
                    return;
                }

                for (unsigned int i = 0; i < materialInfo.material.size(); ++i)
                {
                    ch->RemoveSpecifyItem(materialInfo.material[i].vnum, (materialInfo.material[i].count*count_item), true);
                }

                if (materialInfo.gold != 0)
                {
#ifdef ENABLE_NEW_GOLD_LIMIT
                    ch->ChangeGold(-static_cast<long long>(materialInfo.gold*count_item));
#else
                    ch->PointChange(POINT_GOLD, -static_cast<long long>(materialInfo.gold*count_item), false);
#endif
                }

#ifdef ENABLE_GAYA_RENEWAL
                if (materialInfo.gaya != 0)
                {
                    ch->PointChange(POINT_GEM, -(materialInfo.gaya*count_item), false);
                }
#endif

                if (total_items_give < 1)
                {
                    M2_DESTROY_ITEM(pItem);

#ifdef ENABLE_TEXTS_RENEWAL
                    ch->ChatPacketNew(CHAT_TYPE_INFO, 817, "");
#endif
                    return;
                }

#ifdef ENABLE_CUBE_RENEWAL_COPY_WORLDARD
                if (materialInfo.allowCopy != 0 && item_copy_bonus)
                {
                    pItem->ClearAttribute();

                    for (uint8_t a = 0; a < ITEM_ATTRIBUTE_MAX_NUM; a++)
                    {
                        pItem->SetForceAttribute(a, copyAttr[a][0], copyAttr[a][1]);
                    }

                    for (uint8_t b = 0; b < ITEM_SOCKET_MAX_NUM; b++)
                    {
                        pItem->SetSocket(b, copySocket[b]);
                    }
                }
#endif

                if (!item_copy_bonus && pItem->GetType() == ITEM_COSTUME)
                {
                    pItem->ClearAttribute();
#ifdef ENABLE_STOLE_COSTUME
                    if (pItem->GetSubType() == COSTUME_STOLE)
                    {
                        uint8_t grade = pItem->GetValue(0);
                        if (grade > 0)
                        {
                            grade = grade > 4 ? 4 : grade;
                            uint8_t random = (grade * 4);
    
                            for (int i = 0; i < MAX_ATTR; i++)
                            {
                                pItem->SetForceAttribute(i, stoleInfoTable[i][0], stoleInfoTable[i][number(random - 3, random)]);
                            }
                        }
                    }
                    else
                    {
                        pItem->AlterToMagicItem();
                    }
#else
                    pItem->AlterToMagicItem();
#endif
                }

#ifdef USE_BATTLEPASS
                ch->UpdateExtBattlePassMissionProgress(BP_ITEM_CRAFT, pItem->GetCount(), pItem->GetVnum());
#endif

#ifdef ENABLE_TEXTS_RENEWAL
                ch->ChatPacketNew(CHAT_TYPE_INFO, 818, "");
#endif

#ifdef __HIGHLIGHT_SYSTEM__
                ch->AutoGiveItem(pItem, true, true);
#else
                ch->AutoGiveItem(pItem, true);
#endif
            }
#ifdef ENABLE_TEXTS_RENEWAL
            else
            {
                ch->ChatPacketNew(CHAT_TYPE_INFO, 819, "");
            }
#endif
		}

		index_value++;
	}
}

void SendDateCubeRenewalPackets(LPCHARACTER ch, BYTE subheader, DWORD npcVNUM)
{
	const LPDESC d = ch ? ch->GetDesc() : nullptr;
	if (!d)
	{
		return;
	}

	TPacketGCCubeRenewalReceive pack;
	pack.subheader = subheader;

	if(subheader == CUBE_RENEWAL_SUB_HEADER_DATES_RECEIVE)
	{
		const TCubeResultList& resultList = cube_info_map[npcVNUM];
		for (TCubeResultList::const_iterator iter = resultList.begin(); resultList.end() != iter; ++iter)
		{

			const SCubeMaterialInfo& materialInfo = *iter;

			pack.date_cube_renewal.vnum_reward = materialInfo.reward.vnum;
			pack.date_cube_renewal.count_reward = materialInfo.reward.count;

			auto pItem = ITEM_MANAGER::instance().GetTable(materialInfo.reward.vnum);
			if (!pItem)
			{
				sys_err("%s: unknown material reward vnum %u with category %s", ch->GetName(), materialInfo.reward.vnum, materialInfo.category.c_str());
				continue;
			}

			pack.date_cube_renewal.item_reward_stackable = IS_SET(pItem->dwFlags, ITEM_FLAG_STACKABLE) && !IS_SET(pItem->dwAntiFlags, ITEM_ANTIFLAG_STACK);

			pack.date_cube_renewal.vnum_material_1 = FN_check_cube_item_vnum_material(materialInfo,1);
			pack.date_cube_renewal.count_material_1 = FN_check_cube_item_count_material(materialInfo,1);

			pack.date_cube_renewal.vnum_material_2 = FN_check_cube_item_vnum_material(materialInfo,2);
			pack.date_cube_renewal.count_material_2 = FN_check_cube_item_count_material(materialInfo,2);

			pack.date_cube_renewal.vnum_material_3 = FN_check_cube_item_vnum_material(materialInfo,3);
			pack.date_cube_renewal.count_material_3 = FN_check_cube_item_count_material(materialInfo,3);

			pack.date_cube_renewal.vnum_material_4 = FN_check_cube_item_vnum_material(materialInfo,4);
			pack.date_cube_renewal.count_material_4 = FN_check_cube_item_count_material(materialInfo,4);

			pack.date_cube_renewal.vnum_material_5 = FN_check_cube_item_vnum_material(materialInfo,5);
			pack.date_cube_renewal.count_material_5 = FN_check_cube_item_count_material(materialInfo,5);

			pack.date_cube_renewal.gold = materialInfo.gold;
			pack.date_cube_renewal.percent = materialInfo.percent;
#if defined(ENABLE_GAYA_RENEWAL)
			pack.date_cube_renewal.gaya = materialInfo.gaya;
#endif
#ifdef ENABLE_CUBE_RENEWAL_COPY_WORLDARD
			pack.date_cube_renewal.allowCopy = materialInfo.allowCopy;
#endif
			strlcpy(pack.date_cube_renewal.category, materialInfo.category.c_str(), sizeof(pack.date_cube_renewal.category));

			d->Packet(&pack, sizeof(pack));
		}
	}
	else
	{
		d->Packet(&pack, sizeof(pack));
	}
}

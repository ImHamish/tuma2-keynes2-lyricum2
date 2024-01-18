#include "../common/stl.h"
#include "../common/tables.h"


class CSwitchBonus : public singleton<CSwitchBonus>
{

	enum
	{
		VNUM_CHANGE_ITEM = 71084,
		COUNT_CHANGE_ITEM = 400,
		// Blue Switchers
		SWITCHER_NORMAL_VNUM = 71084,


		// Green Switchers
		SWITCHER_LIMIT_VNUM = 71151,
		SWITCHER_LIMIT2_VNUM = 76023,

		// Costume Switchers
		SWITCHER_COSTUME_VNUM = 70064,
		SWITCHER_COSTUME2_VNUM = 88963,
		
		// Talisman Switchers
		SWITCHER_PENDANT_VNUM = 86051,
		SWITCHER_PENDANT2_VNUM = 88965,
		
		// Zodiac Switchers
		SWITCHER_ZODIAC_VNUM = 86060,

	};

	public:
		CSwitchBonus();
		virtual ~CSwitchBonus();

		bool CheckElementsSB(LPCHARACTER ch);
		bool CanMoveIntoSB(LPITEM item);
		void OpenSB(LPCHARACTER ch);
		void ClearSB(LPCHARACTER ch);
		void CloseSB(LPCHARACTER ch);
		void SendSBItem(LPCHARACTER ch,LPITEM item);
		void ChangeSB(LPCHARACTER ch,const TPlayerItemAttribute* aAttr, int vnum_change);
		bool CheckVnumsChange(LPITEM item, int vnum_item);
		void SendSBPackets(LPCHARACTER ch,BYTE subheader, int id_bonus = 0, int value_bonus_0 = 0, int value_bonus_1 = 0, int value_bonus_2 = 0, int value_bonus_3 = 0, int value_bonus_4 = 0);
		bool IsItemZodiac(DWORD vnum);
};
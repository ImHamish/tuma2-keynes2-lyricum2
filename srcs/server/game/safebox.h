#pragma once

class CHARACTER;
class CItem;
class CGrid;

class CSafebox
{
	public:
		CSafebox(LPCHARACTER, int32_t, DWORD);
		~CSafebox();

		bool Add(DWORD, LPITEM);
		LPITEM Get(DWORD);
		LPITEM Remove(DWORD);
		void ChangeSize(int32_t);

		bool MoveItem(BYTE, BYTE, WORD);
		LPITEM GetItem(BYTE);

		void Save();

		bool IsEmpty(DWORD, BYTE);
#if defined(ITEM_CHECKINOUT_UPDATE)
		int32_t GetEmptySafebox(BYTE);
#endif
		bool IsValidPosition(DWORD);

		void SetWindowMode(BYTE);

	protected:
		void __Destroy();

		LPCHARACTER m_pkChrOwner;
		LPITEM m_pkItems[SAFEBOX_MAX_NUM];
		CGrid* m_pkGrid;
		int32_t m_iSize;
		long m_lGold;
		BYTE m_bWindowMode;
};

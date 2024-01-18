#ifndef __ITEM_ADDON_H
#define __ITEM_ADDON_H

class CItemAddonManager : public singleton<CItemAddonManager>
{
	public:
		CItemAddonManager();
		virtual ~CItemAddonManager();

#ifdef ENABLE_SWITCHBOT_WORLDARD
		void ApplyAddonTo(int iAddonType, LPITEM pItem, bool switchbot = false);
#else
		void ApplyAddonTo(int iAddonType, LPITEM pItem);
#endif
};

#endif

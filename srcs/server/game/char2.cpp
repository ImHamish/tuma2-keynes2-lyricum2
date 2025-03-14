#include "stdafx.h"
#include "char.h"
#include "desc.h"
#include "buffer_manager.h"
#ifdef ENABLE_HWID
#include "hwidmanager.h"
#endif

#if defined(ENABLE_TEXTS_RENEWAL)
void CHARACTER::ChatPacketNew(BYTE type, DWORD idx, const char * format, ...) {
	if (type != CHAT_TYPE_INFO && 
		type != CHAT_TYPE_NOTICE && 
		type != CHAT_TYPE_BIG_NOTICE
#ifdef ENABLE_DICE_SYSTEM
		 && type != CHAT_TYPE_DICE_INFO
#endif
#ifdef ENABLE_NEW_CHAT
		 && type != CHAT_TYPE_INFO_EXP
		 && type != CHAT_TYPE_INFO_ITEM
		 && type != CHAT_TYPE_INFO_VALUE
#endif
		&& type != CHAT_TYPE_DIALOG
	)
		return;

	LPDESC d = GetDesc();
	if (!d)
		return;

	char chatbuf[CHAT_MAX_LEN + 1];
	va_list args;
	va_start(args, format);
	int len = vsnprintf(chatbuf, sizeof(chatbuf), format, args);
	va_end(args);

	TPacketGCChatNew p;
	p.header = HEADER_GC_CHAT_NEW;
	p.type = type;
	p.idx = idx;
	p.size = sizeof(p) + len;

	TEMP_BUFFER buf;
	buf.write(&p, sizeof(p));
	buf.write(chatbuf, len);

	d->Packet(buf.read_peek(), buf.size());
}
#endif

#ifdef __DUNGEON_INFO_SYSTEM__
void CHARACTER::SetQuestDamage(int race, int dmg) {
	if (race != 693 && 
		race != 768 && 
		race != 1093 && 
		race != 2092 && 
		race != 2493 && 
		race != 2598 && 
		race != 3962 && 
		race != 4011 && 
		race != 4158 && 
		race != 6091 && 
		race != 6191 && 
		race != 6192 && 
		race != 6118 && 
		race != 6393
#if defined(ENABLE_HALLOWEEN_EVENT_2022)
 && race != 4468
 && race != 4568
 && race != 4668
#endif
 && race != 6756
 && race != 4103
 && race != 4525
		) {
		return;
	}

	auto it = dungeonDamage.find(race);
	if (it == dungeonDamage.end()) {
		dungeonDamage.insert(dungeonDamage.begin(), std::pair<int, int>(race, dmg));
	} else {
		if (dmg > it->second)
		{
			it->second = dmg;
		}
	}
}

int CHARACTER::GetQuestDamage(int race) {
	auto it = dungeonDamage.find(race);
	return it == dungeonDamage.end() ? 0 : it->second;
}
#endif

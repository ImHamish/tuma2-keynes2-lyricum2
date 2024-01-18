#include "stdafx.h"

#if defined(ENABLE_OKEY_CARD_GAME)
#include "char.h"
#include "log.h"
#include "db.h"
#if defined(ENABLE_EVENT_MANAGER)
#include "char_manager.h"
#endif

#if defined(ENABLE_EVENT_MANAGER)
void CHARACTER::SendOkeyGameEventInfo()
{
	ChatPacket(CHAT_TYPE_COMMAND, "okeycards_event %u",
		CHARACTER_MANAGER::Instance().CheckEventIsActive(OKEY_CARD_EVENT) == NULL ? 0 : 1);
}
#endif

void CHARACTER::Cards_open(uint32_t safeMode)
{
#if defined(ENABLE_EVENT_MANAGER)
	if (!CHARACTER_MANAGER::Instance().CheckEventIsActive(OKEY_CARD_EVENT))
	{
		return;
	}
#else
	return;
#endif

	if (GetExchange() || GetMyShop() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen()
#ifdef ENABLE_ACCE_SYSTEM
 || IsAcceOpen()
#endif
#ifdef __ENABLE_NEW_OFFLINESHOP__
 || GetOfflineShopGuest() || GetAuctionGuest()
#endif
#if defined(ENABLE_AURA_SYSTEM)
 || IsAuraRefineWindowOpen()
#endif
#ifdef __ATTR_TRANSFER_SYSTEM__
 || IsAttrTransferOpen()
#endif
#if defined(ENABLE_CHANGELOOK)
 || isChangeLookOpened()
#endif
#if defined(USE_ATTR_6TH_7TH)
 || IsOpenAttr67Add()
#endif
	)
	{
		ChatPacketNew(CHAT_TYPE_INFO, 291, "");
		return;
	}

	if (character_cards.cards_left <= 0) 
	{
		if (GetGold() < OKEY_CARD_PLAY_PRICE)
		{
			ChatPacketNew(CHAT_TYPE_INFO, 232, ""); 
			return;
		}

		if (CountSpecifyItem(OKEY_CARD_SET_ITEMVNUM) < 1)
		{
			ChatPacketNew(CHAT_TYPE_INFO, 1609, ""); 
			return;
		}

#if defined(ENABLE_NEW_GOLD_LIMIT)
		ChangeGold(-OKEY_CARD_PLAY_PRICE);
#else
		PointChange(POINT_GOLD, -OKEY_CARD_PLAY_PRICE);
#endif
		RemoveSpecifyItem(OKEY_CARD_SET_ITEMVNUM, 1);

		Cards_clean_list();
		character_cards.cards_left = MAX_OKEY_CARDS;
	}

	ChatPacket(CHAT_TYPE_COMMAND, "cards open %u", safeMode);
	SendUpdatedInformations();
}

void CHARACTER::Cards_clean_list()
{
	memset(&character_cards, 0, sizeof(character_cards));
	memset(&randomized_cards, 0, sizeof(randomized_cards));
}

int32_t CHARACTER::GetEmptySpaceInHand()
{
	for (uint8_t i = 0; i < MAX_OKEY_CARDS_IN_HAND; ++i)
	{
		if (character_cards.cards_in_hand[i].type == 0)
			return i;
	}

	return -1;
}

void CHARACTER::Cards_pullout()
{
	int32_t empty_space = GetEmptySpaceInHand();
	if (empty_space == -1)
	{
		ChatPacketNew(CHAT_TYPE_INFO, 1610, ""); 
		return;
	}

	if (character_cards.cards_left < 1)
	{
		ChatPacketNew(CHAT_TYPE_INFO, 1611, ""); 
		return;
	}

	if (GetAllCardsCount() >= 5)
	{
		ChatPacketNew(CHAT_TYPE_INFO, 1612, ""); 
		return;
	}

	RandomizeCards();
	SendUpdatedInformations();
}

void CHARACTER::RandomizeCards()
{
	uint8_t card_type = number(1, 3);
	uint8_t card_value = number(1, 8);

	if (CardWasRandomized(card_type, card_value) != false)
	{
		RandomizeCards();
	}
	else
	{
		int32_t emptySpace = GetEmptySpaceInHand();
		if (emptySpace != -1)
		{
			character_cards.cards_in_hand[emptySpace].type = card_type;
			character_cards.cards_in_hand[emptySpace].value = card_value;
			character_cards.cards_left -= 1;
		}
	}
}

bool CHARACTER::CardWasRandomized(uint8_t type, uint8_t value)
{
	for (uint8_t i = 0; i < MAX_OKEY_CARDS; ++i)
	{
		if (randomized_cards[i].type == type && randomized_cards[i].value == value)
		{
			return true;
		}
	}

	for (uint8_t i = 0; i < MAX_OKEY_CARDS; ++i)
	{
		if (randomized_cards[i].type == 0)
		{
			randomized_cards[i].type = type;
			randomized_cards[i].value = value;
			return false;
		}
	}

	return false;
}

void CHARACTER::SendUpdatedInformations()
{
	ChatPacket(CHAT_TYPE_COMMAND, "cards info %d %d %d %d %d %d"
											" %d %d %d %d %d %d",
			character_cards.cards_in_hand[0].type, character_cards.cards_in_hand[0].value, 
			character_cards.cards_in_hand[1].type, character_cards.cards_in_hand[1].value,
			character_cards.cards_in_hand[2].type, character_cards.cards_in_hand[2].value,
			character_cards.cards_in_hand[3].type, character_cards.cards_in_hand[3].value,
			character_cards.cards_in_hand[4].type, character_cards.cards_in_hand[4].value,
			character_cards.cards_left, character_cards.points);

	ChatPacket(CHAT_TYPE_COMMAND, "cards finfo %d %d %d %d %d %d %d",
			character_cards.cards_in_field[0].type, character_cards.cards_in_field[0].value,
			character_cards.cards_in_field[1].type, character_cards.cards_in_field[1].value,
			character_cards.cards_in_field[2].type, character_cards.cards_in_field[2].value,
			character_cards.field_points);
}

void CHARACTER::SendReward()
{
	ChatPacket(CHAT_TYPE_COMMAND, "cards reward %d %d %d %d %d %d %d",
			character_cards.cards_in_field[0].type, character_cards.cards_in_field[0].value,
			character_cards.cards_in_field[1].type, character_cards.cards_in_field[1].value,
			character_cards.cards_in_field[2].type, character_cards.cards_in_field[2].value,
			character_cards.field_points);
}

void CHARACTER::CardsDestroy(uint32_t rejectIndex)
{
	if (rejectIndex + 1 > MAX_OKEY_CARDS_IN_HAND)
	{
		return;
	}

	if (character_cards.cards_in_hand[rejectIndex].type == 0)
	{
		return;
	}

	character_cards.cards_in_hand[rejectIndex].type = 0;
	character_cards.cards_in_hand[rejectIndex].value = 0;
	SendUpdatedInformations();
}

void CHARACTER::CardsAccept(uint32_t acceptIndex)
{
#if defined(ENABLE_EVENT_MANAGER)
	if (!CHARACTER_MANAGER::Instance().CheckEventIsActive(OKEY_CARD_EVENT))
	{
		return;
	}
#else
	return;
#endif

	if (acceptIndex + 1 > MAX_OKEY_CARDS_IN_HAND)
	{
		return;
	}

	if (character_cards.cards_in_hand[acceptIndex].type == 0)
	{
		return;
	}

	int32_t emptySpace = GetEmptySpaceInField();
	if (emptySpace != -1)
	{
		character_cards.cards_in_field[emptySpace].type = character_cards.cards_in_hand[acceptIndex].type;
		character_cards.cards_in_field[emptySpace].value = character_cards.cards_in_hand[acceptIndex].value;
		character_cards.cards_in_hand[acceptIndex].type = 0;
		character_cards.cards_in_hand[acceptIndex].value = 0;
	}

	if (GetEmptySpaceInField() == -1)
	{
		if (CheckReward())
		{
			SendReward();
			ResetField();
		}
		else
		{
			RestoreField();
		}
	}

	SendUpdatedInformations();
}

void CHARACTER::CardsRestore(uint32_t restoreIndex)
{
	if (restoreIndex + 1 > MAX_OKEY_CARDS_IN_FIELD)
	{
		return;
	}

	if (character_cards.cards_in_field[restoreIndex].type == 0)
	{
		return;
	}

	int32_t emptySpace = GetEmptySpaceInHand();
	if (emptySpace == -1)
	{
		return;
	}

	character_cards.cards_in_hand[emptySpace].type = character_cards.cards_in_field[restoreIndex].type;
	character_cards.cards_in_hand[emptySpace].value = character_cards.cards_in_field[restoreIndex].value;
	character_cards.cards_in_field[restoreIndex].type = 0;
	character_cards.cards_in_field[restoreIndex].value = 0;

	SendUpdatedInformations();
}

int32_t CHARACTER::GetEmptySpaceInField()
{
	for (uint8_t i = 0; i < MAX_OKEY_CARDS_IN_FIELD; ++i)
	{
		if (character_cards.cards_in_field[i].type == 0)
		{
			return i;
		}
	}

	return -1;
}

uint32_t CHARACTER::GetAllCardsCount()
{
	uint32_t count = 0;

	for (uint8_t i = 0; i < MAX_OKEY_CARDS_IN_HAND; ++i)
	{
		if (character_cards.cards_in_hand[i].type != 0)
		{
			++count;
		}
	}

	for (uint8_t i = 0; i < MAX_OKEY_CARDS_IN_FIELD; ++i)
	{
		if (character_cards.cards_in_field[i].type != 0)
		{
			++count;
		}
	}

	return count;
}

bool CHARACTER::TypesAreSame()
{
	if (character_cards.cards_in_field[0].type == character_cards.cards_in_field[1].type &&
		character_cards.cards_in_field[1].type == character_cards.cards_in_field[2].type)
	{
		return true;
	}

	return false;
}

bool CHARACTER::ValuesAreSame()
{
	if (character_cards.cards_in_field[0].value == character_cards.cards_in_field[1].value &&
		character_cards.cards_in_field[1].value == character_cards.cards_in_field[2].value)
	{
		return true;
	}

	return false;
}

bool CHARACTER::CardsMatch()
{
	if (character_cards.cards_in_field[0].value == character_cards.cards_in_field[1].value - 1 &&
		character_cards.cards_in_field[1].value == character_cards.cards_in_field[2].value -1)
	{
		return true;
	}
	else if (character_cards.cards_in_field[0].value == character_cards.cards_in_field[2].value - 1 &&
		character_cards.cards_in_field[2].value == character_cards.cards_in_field[1].value -1)
	{
		return true;
	}
	else if (character_cards.cards_in_field[1].value == character_cards.cards_in_field[0].value - 1 &&
		character_cards.cards_in_field[0].value == character_cards.cards_in_field[2].value -1)
	{
		return true;
	}
	else if (character_cards.cards_in_field[0].value == character_cards.cards_in_field[1].value - 1 &&
		character_cards.cards_in_field[2].value == character_cards.cards_in_field[0].value -1)
	{
		return true;
	}
	else if (character_cards.cards_in_field[1].value == character_cards.cards_in_field[0].value - 1 &&
		character_cards.cards_in_field[2].value == character_cards.cards_in_field[1].value -1)
	{
		return true;
	}
	else if (character_cards.cards_in_field[1].value == character_cards.cards_in_field[2].value - 1 &&
		character_cards.cards_in_field[2].value == character_cards.cards_in_field[0].value -1)
	{
		return true;
	}

	return false;
}

uint32_t CHARACTER::GetLowestCard()
{
	return MIN(character_cards.cards_in_field[0].value,
				MIN(character_cards.cards_in_field[1].value, character_cards.cards_in_field[2].value));
}

bool CHARACTER::CheckReward()
{
	if (TypesAreSame() && ValuesAreSame())
	{
		character_cards.field_points = 150;
		character_cards.points += 150;
		return true;
	}
	else if (TypesAreSame() && CardsMatch())
	{
		character_cards.field_points = 100;
		character_cards.points += 100;
		return true;
	}
	else if (ValuesAreSame())
	{
		character_cards.field_points = (GetLowestCard() * 10) + 10;
		character_cards.points += (GetLowestCard() * 10) + 10;
		return true;
	}
	else if (CardsMatch())
	{
		character_cards.field_points = GetLowestCard() * 10;
		character_cards.points += GetLowestCard() * 10;
		return true;
	}

	RestoreField();
	return false;
}

void CHARACTER::RestoreField()
{
	for (uint8_t i = 0; i < MAX_OKEY_CARDS_IN_FIELD; ++i)
	{
		int32_t emptySpace = GetEmptySpaceInHand();
		if (emptySpace != -1)
		{
			character_cards.cards_in_hand[emptySpace].type = character_cards.cards_in_field[i].type;
			character_cards.cards_in_hand[emptySpace].value = character_cards.cards_in_field[i].value;
			character_cards.cards_in_field[i].type = 0;
			character_cards.cards_in_field[i].value = 0;

			SendUpdatedInformations();
		}
	}
}

void CHARACTER::ResetField()
{
	for (uint8_t i = 0; i < MAX_OKEY_CARDS_IN_FIELD; ++i)
	{
		character_cards.cards_in_field[i].type = 0;
		character_cards.cards_in_field[i].value = 0;
	}
}

void CHARACTER::CardsEnd()
{
	if (character_cards.points >= 400)
	{
		AutoGiveItem(OKEY_CARD_GOLDEN_PRIZE_ITEMVNUM);
	}
	else if (character_cards.points < 400 && character_cards.points >= 250)
	{
		AutoGiveItem(OKEY_CARD_SILVER_PRIZE_ITEMVNUM);
	}
	else if (character_cards.points > 0)
	{
		AutoGiveItem(OKEY_CARD_BRONZE_PRIZE_ITEMVNUM);
	}

	LogManager::instance().OkayEventLog(GetPlayerID(), GetName(), character_cards.points);

	Cards_clean_list();
	SendUpdatedInformations();
}

void CHARACTER::GetGlobalRank(const char* szPlace, const char* szPoint, char* buf, size_t buflen)
{
	std::unique_ptr<SQLMsg> msg(DBManager::Instance().DirectQuery("SELECT name, SUM(points) as "
			"spoints FROM log.okay_event GROUP BY pid ORDER BY spoints DESC LIMIT 10"));

	if (!msg || !msg->Get() || msg->Get()->uiNumRows == 0)
	{
		return;
	}

	int pos = 1;
	int len = 0, len2;
	*buf = '\0';

	MYSQL_ROW row;
	while (NULL != (row = mysql_fetch_row(msg->Get()->pSQLResult)))
	{
		int32_t points = 0;
		str_to_number(points, row[1]);
		len2 = snprintf(buf + len, buflen - len, "[ENTER]");

		if (len2 < 0 || len2 >= (int) buflen - len)
		{
			len += (buflen - len) - 1;
		}
		else
		{
			len += len2;
		}

		len2 = snprintf(buf + len, buflen - len,
				"%d. %s - %s - %d %s.", 
				pos,
				szPlace,
				row[0],
				points,
				szPoint);

		++pos;

		if (len2 < 0 || len2 >= (int) buflen - len)
		{
			len += (buflen - len) - 1;
		}
		else
		{
			len += len2;
		}
	}
}

void CHARACTER::GetRundRank(const char* szPlace, const char* szPoint, char* buf, size_t buflen)
{
	std::unique_ptr<SQLMsg> msg(DBManager::Instance().DirectQuery("SELECT name, points FROM "
			"log.okay_event ORDER BY points DESC LIMIT 10"));

	if (!msg || !msg->Get() || msg->Get()->uiNumRows == 0)
	{
		return;
	}

	int pos = 1;
	int len = 0, len2;
	*buf = '\0';

	MYSQL_ROW row;
	while (NULL != (row = mysql_fetch_row(msg->Get()->pSQLResult)))
	{
		int32_t points = 0;
		str_to_number(points, row[1]);
		len2 = snprintf(buf + len, buflen - len, "[ENTER]");

		if (len2 < 0 || len2 >= (int) buflen - len)
		{
			len += (buflen - len) - 1;
		}
		else
		{
			len += len2;
		}

		len2 = snprintf(buf + len, buflen - len,
				"%d. %s - %s - %d %s.", 
				pos,
				szPlace,
				row[0],
				points,
				szPoint);

		++pos;

		if (len2 < 0 || len2 >= (int) buflen - len)
		{
			len += (buflen - len) - 1;
		}
		else
		{
			len += len2;
		}
	}
}
#endif

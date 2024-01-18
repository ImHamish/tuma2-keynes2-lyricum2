#include "stdafx.h"
#include "config.h"
#include "char.h"
#include "char_manager.h"
#include "regen.h"
#include "mob_manager.h"
#include "dungeon.h"


#include "desc.h"

LPREGEN	regen_list = NULL;
LPREGEN_EXCEPTION regen_exception_list = NULL;

std::map<int32_t, std::vector<SBossPosition*>> g_BossPosition;

enum ERegenModes
{
	MODE_TYPE = 0,
	MODE_SX,
	MODE_SY,
	MODE_EX,
	MODE_EY,
	MODE_Z_SECTION,
	MODE_DIRECTION,
	MODE_REGEN_TIME,
	MODE_REGEN_PERCENT,
	MODE_MAX_COUNT,
	MODE_VNUM
};

static bool get_word(FILE *fp, char *buf) // 워드단위로 받는다.
{
	int i = 0;
	int c;

	int semicolon_mode = 0;

	while ((c = fgetc(fp)) != EOF)
	{
		if (i == 0)
		{
			if (c == '"')
			{
				semicolon_mode = 1;
				continue;
			}

			if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
				continue;
		}

		if (semicolon_mode)
		{
			if (c == '"')
			{
				buf[i] = '\0';
				return true;
			}

			buf[i++] = c;
		}
		else
		{
			if ((c == ' ' || c == '\t' || c == '\n' || c == '\r'))
			{
				buf[i] = '\0';
				return true;
			}

			buf[i++] = c;
		}

		if (i == 2 && buf[0] == '/' && buf[1] == '/')
		{
			buf[i] = '\0';
			return true;
		}
	}

	buf[i] = '\0';
	return (i != 0);
}

static void next_line(FILE *fp)
{
	int c;

	while ((c = fgetc(fp)) != EOF)
		if (c == '\n')
			return;
}

static bool read_line(FILE *fp, LPREGEN regen, const char* filename)
{
	char szTmp[256];

	int mode = MODE_TYPE;
	int tmpTime;
	DWORD i;

	while (get_word(fp, szTmp))
	{
		if (!strncmp(szTmp, "//", 2))
		{
			next_line(fp);
			continue;
		}

		switch (mode)
		{
			case MODE_TYPE:
				if (szTmp[0] == 'm')
				{
					regen->type = REGEN_TYPE_MOB;

					if (szTmp[1] == 'a')
						regen->is_aggressive = true;

					if (szTmp[1] == 's')
						regen->is_static = true;
				}
				else if (szTmp[0] == 'g')
				{
					regen->type = REGEN_TYPE_GROUP;

					if (szTmp[1] == 'a')
						regen->is_aggressive = true;

					if (szTmp[1] == 's')
						regen->is_static = true;
				}
				else if (szTmp[0] == 'e')
					regen->type = REGEN_TYPE_EXCEPTION;
				else if (szTmp[0] == 'r') 
				{
					regen->type = REGEN_TYPE_GROUP_GROUP;

					if (szTmp[1] == 's')
						regen->is_static = true;
				}
				else if (szTmp[0] == 's')
					regen->type = REGEN_TYPE_ANYWHERE;
				else
				{
					sys_err("filename(%s) read_line: unknown regen type %c", filename, szTmp[0]);
					exit(1);
				}

				++mode;
				break;

			case MODE_SX:
				str_to_number(regen->sx, szTmp);
				++mode;
				break;

			case MODE_SY:
				str_to_number(regen->sy, szTmp);
				++mode;
				break;

			case MODE_EX:
				{
					int iX = 0;
					str_to_number(iX, szTmp);

					regen->sx -= iX;
					regen->ex = regen->sx + iX * 2;

					regen->sx *= 100;
					regen->ex *= 100;

					++mode;
				}
				break;

			case MODE_EY:
				{
					int iY = 0;
					str_to_number(iY, szTmp);

					regen->sy -= iY;
					regen->ey = regen->sy + iY * 2;

					regen->sy *= 100;
					regen->ey *= 100;

					++mode;
				}
				break;

			case MODE_Z_SECTION:
				str_to_number(regen->z_section, szTmp);

				// 익셉션 이면 나가주자.
				if (regen->type == REGEN_TYPE_EXCEPTION)
					return true;

				++mode;
				break;

			case MODE_DIRECTION:
				str_to_number(regen->direction, szTmp);
				++mode;
				break;

			case MODE_REGEN_TIME:
				regen->time = 0;
				tmpTime = 0;

				for (i = 0; i < strlen(szTmp); ++i)
				{
					switch (szTmp[i])
					{
						case 'h':
							regen->time += tmpTime * 3600;
							tmpTime = 0;
							break;

						case 'm':
							regen->time += tmpTime * 60;
							tmpTime = 0;
							break;

						case 's':
							regen->time += tmpTime;
							tmpTime = 0;
							break;

						default:
							if (szTmp[i] >= '0' && szTmp[i] <= '9')
							{
								tmpTime *= 10;
								tmpTime += (szTmp[i] - '0');
							}
					}
				}

				++mode;
				break;

			case MODE_REGEN_PERCENT:
				++mode;
				break;

			case MODE_MAX_COUNT:
				regen->count = 0;
				str_to_number(regen->max_count, szTmp);
				++mode;
				break;

			case MODE_VNUM:
				str_to_number(regen->vnum, szTmp);
				++mode;
				return true;
		}
	}

	return false;
}

bool is_regen_exception(long x, long y)
{
	LPREGEN_EXCEPTION exc;

	for (exc = regen_exception_list; exc; exc = exc->next)
	{
		if (exc->sx <= x && exc->sy <= y)
			if (exc->ex >= x && exc->ey >= y)
				return true;
	}

	return false;
}

static void regen_spawn_dungeon(LPREGEN regen, LPDUNGEON pDungeon, bool bOnce)
{
	DWORD	num;
	DWORD	i;

	num = (regen->max_count - regen->count);

	if (regen->boss_pos)
		regen->boss_pos->is_alive = regen->count > 0;

	if (!num)
		return;

	for (i = 0; i < num; ++i)
	{
		LPCHARACTER ch = NULL;

		if (regen->type == REGEN_TYPE_ANYWHERE)
		{
			ch = CHARACTER_MANAGER::instance().SpawnMobRandomPosition(regen->vnum, regen->lMapIndex);

			if (ch)
			{
				++regen->count;
				ch->SetDungeon(pDungeon);
			}
		}
		else if (regen->sx == regen->ex && regen->sy == regen->ey)
		{
			ch = CHARACTER_MANAGER::instance().SpawnMob(regen->vnum,
					regen->lMapIndex,
					regen->sx,
					regen->sy,
					regen->z_section,
					false,
					regen->direction == 0 ? number(0, 7) * 45 : (regen->direction - 1) * 45);

			if (ch)
			{
				++regen->count;
				ch->SetDungeon(pDungeon);
			}
		}
		else
		{
			if (regen->type == REGEN_TYPE_MOB)
			{
				ch = CHARACTER_MANAGER::Instance().SpawnMobRange(regen->vnum, regen->lMapIndex, regen->sx, regen->sy, regen->ex, regen->ey, true);

				if (ch)
				{
					++regen->count;
					ch->SetDungeon(pDungeon);
				}
			}
			else if (regen->type == REGEN_TYPE_GROUP)
			{
				if (CHARACTER_MANAGER::Instance().SpawnGroup(regen->vnum, regen->lMapIndex, regen->sx, regen->sy, regen->ex, regen->ey, bOnce ? NULL : regen, regen->is_aggressive, pDungeon))
					++regen->count;
			}
			else if (regen->type == REGEN_TYPE_GROUP_GROUP)
			{
				if (CHARACTER_MANAGER::Instance().SpawnGroupGroup(regen->vnum, regen->lMapIndex, regen->sx, regen->sy, regen->ex, regen->ey, bOnce ? NULL : regen, regen->is_aggressive, pDungeon))
					++regen->count;
			}
		}

		if (ch && !bOnce)
			ch->SetRegen(regen);
		
#ifdef __DEFENSE_WAVE__
		if (pDungeon)
		{
			LPCHARACTER mast = pDungeon->GetMast();
			if (mast)
			{
				ch->SetVictim(mast);
			}
		}
#endif
	}
}

static void regen_spawn(LPREGEN regen, bool bOnce)
{
	DWORD	num;
	DWORD	i;

	num = (regen->max_count - regen->count);

	if (!num)
		return;

	for (i = 0; i < num; ++i)
	{
		LPCHARACTER ch = NULL;

		if (regen->type == REGEN_TYPE_ANYWHERE)
		{
			ch = CHARACTER_MANAGER::instance().SpawnMobRandomPosition(regen->vnum, regen->lMapIndex);

			if (ch)
				++regen->count;
		}
		else if (regen->sx == regen->ex && regen->sy == regen->ey)
		{
			ch = CHARACTER_MANAGER::instance().SpawnMob(regen->vnum,
					regen->lMapIndex,
					regen->sx,
					regen->sy,
					regen->z_section,
					false,
					regen->direction == 0 ? number(0, 7) * 45 : (regen->direction - 1) * 45);

			if (ch)
				++regen->count;
		}
		else
		{
			if (regen->type == REGEN_TYPE_MOB)
			{
				ch = CHARACTER_MANAGER::Instance().SpawnMobRange(regen->vnum, regen->lMapIndex, regen->sx, regen->sy, regen->ex, regen->ey, true, regen->is_aggressive, regen->is_aggressive );

				if (ch)
					++regen->count;
			}
			else if (regen->type == REGEN_TYPE_GROUP)
			{
				if (CHARACTER_MANAGER::Instance().SpawnGroup(regen->vnum, regen->lMapIndex, regen->sx, regen->sy, regen->ex, regen->ey, bOnce ? NULL : regen, regen->is_aggressive))
					++regen->count;
			}
			else if (regen->type == REGEN_TYPE_GROUP_GROUP)
			{
				if (CHARACTER_MANAGER::Instance().SpawnGroupGroup(regen->vnum, regen->lMapIndex, regen->sx, regen->sy, regen->ex, regen->ey, bOnce ? NULL : regen, regen->is_aggressive))
					++regen->count;
			}
		}

		if (ch && !bOnce)
			ch->SetRegen(regen);
	}
}

EVENTFUNC(dungeon_regen_event)
{
	dungeon_regen_event_info* info = dynamic_cast<dungeon_regen_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "dungeon_regen_event> <Factor> Null pointer" );
		return 0;
	}

	LPDUNGEON pDungeon = CDungeonManager::instance().Find(info->dungeon_id);
	if (pDungeon == NULL) {
		return 0;
	}

	LPREGEN	regen = info->regen;
	if (regen->time == 0)
	{
		regen->event = NULL;
	}

	regen_spawn_dungeon(regen, pDungeon, false);
	return PASSES_PER_SEC(regen->time);
}

bool regen_do(const char* filename, long lMapIndex, int base_x, int base_y, LPDUNGEON pDungeon, bool bOnce)
{
	if (g_bNoRegen)
		return true;

	if ( lMapIndex >= 114 && lMapIndex <= 117 )
		return true;

	LPREGEN regen = NULL;
	FILE* fp = fopen(filename, "rt");

	if (NULL == fp)
	{
		sys_err("SYSTEM: regen_do: %s: file not found", filename);
		return false;
	}

	while (true)
	{
		REGEN tmp{};

		if (!read_line(fp, &tmp, filename))
			break;

		if (tmp.type == REGEN_TYPE_MOB ||
			tmp.type == REGEN_TYPE_GROUP ||
			tmp.type == REGEN_TYPE_GROUP_GROUP ||
			tmp.type == REGEN_TYPE_ANYWHERE)
		{
			if (!bOnce)
			{
				regen = M2_NEW REGEN;
				memcpy(regen, &tmp, sizeof(REGEN));
			}
			else
				regen = &tmp;

			if (pDungeon)
				regen->is_aggressive = true;

			regen->lMapIndex = lMapIndex;
			regen->count = 0;

			regen->sx += base_x;
			regen->ex += base_x;

			regen->sy += base_y;
			regen->ey += base_y;

			if (regen->sx > regen->ex)
			{
				regen->sx ^= regen->ex;
				regen->ex ^= regen->sx;
				regen->sx ^= regen->ex;
			}

			if (regen->sy > regen->ey)
			{
				regen->sy ^= regen->ey;
				regen->ey ^= regen->sy;
				regen->sy ^= regen->ey;
			}

			if (regen->type == REGEN_TYPE_MOB)
			{
				const CMob * p = CMobManager::instance().Get(regen->vnum);

				if (!p)
				{
					sys_err("In %s, No mob data by vnum %u", filename, regen->vnum);
					if (!bOnce) {
						M2_DELETE(regen);
					}
					continue;
				}
			}

			if (!bOnce && pDungeon != NULL)
			{
				dungeon_regen_event_info* info = AllocEventInfo<dungeon_regen_event_info>();

				info->regen = regen;
				info->dungeon_id = pDungeon->GetId();

				regen->event = event_create(dungeon_regen_event, info, PASSES_PER_SEC(number(0, 16)) + PASSES_PER_SEC(regen->time));

				pDungeon->AddRegen(regen);
				// regen_id should be determined at this point,
				// before the call to CHARACTER::SetRegen()
			}

			// 처음엔 무조건 리젠 해준다.
			regen_spawn_dungeon(regen, pDungeon, bOnce);

		}
	}

	fclose(fp);
	return true;
}

bool regen_load_in_file(const char* filename, long lMapIndex, int base_x, int base_y)
{
	if (g_bNoRegen)
		return true;

	LPREGEN regen = NULL;
	FILE * fp = fopen(filename, "rt");

	if (NULL == fp)
	{
		sys_err("SYSTEM: regen_do: %s: file not found", filename);
		return false;
	}

	while (true)
	{
		REGEN tmp{};

		if (!read_line(fp, &tmp, filename))
			break;

		if (tmp.type == REGEN_TYPE_MOB ||
			tmp.type == REGEN_TYPE_GROUP ||
			tmp.type == REGEN_TYPE_GROUP_GROUP ||
			tmp.type == REGEN_TYPE_ANYWHERE)
		{
			regen = &tmp;

			regen->is_aggressive = true;

			regen->lMapIndex = lMapIndex;
			regen->count = 0;

			regen->sx += base_x;
			regen->ex += base_x;

			regen->sy += base_y;
			regen->ey += base_y;

			if (regen->sx > regen->ex)
			{
				regen->sx ^= regen->ex;
				regen->ex ^= regen->sx;
				regen->sx ^= regen->ex;
			}

			if (regen->sy > regen->ey)
			{
				regen->sy ^= regen->ey;
				regen->ey ^= regen->sy;
				regen->sy ^= regen->ey;
			}

			if (regen->type == REGEN_TYPE_MOB)
			{
				const CMob * p = CMobManager::instance().Get(regen->vnum);

				if (!p)
				{
					sys_err("In %s, No mob data by vnum %u", filename, regen->vnum);
					continue;
				}
			}

			// 처음엔 무조건 리젠 해준다.
			regen_spawn(regen, true);
		}
	}

	fclose(fp);
	return true;
}

EVENTFUNC(regen_event)
{
	regen_event_info* info = dynamic_cast<regen_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "regen_event> <Factor> Null pointer" );
		return 0;
	}

	LPREGEN	regen = info->regen;
	if (!is_valid_regen(regen))
		return 0;

	if (regen->time == 0)
		regen->event = NULL;

	regen_spawn(regen, false);
	return PASSES_PER_SEC(regen->time);
}

#ifdef ENABLE_ATLAS_BOSS
bool regen_load(const char* filename, long lMapIndex, int base_x, int base_y, bool bossFile)
#else
bool regen_load(const char* filename, long lMapIndex, int base_x, int base_y)
#endif
{
	if (g_bNoRegen)
		return true;

	LPREGEN regen = NULL;
	FILE* fp = fopen(filename, "rt");

	if (NULL == fp)
	{
		sys_log(0, "SYSTEM: regen_load: %s: file not found", filename);
		return false;
	}

	while (true)
	{
		REGEN tmp{};

		if (!read_line(fp, &tmp, filename))
			break;

		if (tmp.type == REGEN_TYPE_MOB ||
			tmp.type == REGEN_TYPE_GROUP ||
			tmp.type == REGEN_TYPE_GROUP_GROUP ||
			tmp.type == REGEN_TYPE_ANYWHERE)
		{
			if (test_server)
			{
				CMobManager::instance().IncRegenCount(tmp.type, tmp.vnum, tmp.max_count, tmp.time);
			}

			regen = M2_NEW REGEN;
			memcpy(regen, &tmp, sizeof(REGEN));
			INSERT_TO_TW_LIST(regen, regen_list, prev, next);

			regen->lMapIndex = lMapIndex;
			regen->count = 0;

			regen->sx += base_x;
			regen->ex += base_x;

			regen->sy += base_y;
			regen->ey += base_y;

			if (regen->sx > regen->ex)
			{
				regen->sx ^= regen->ex;
				regen->ex ^= regen->sx;
				regen->sx ^= regen->ex;
			}

			if (regen->sy > regen->ey)
			{
				regen->sy ^= regen->ey;
				regen->ey ^= regen->sy;
				regen->sy ^= regen->ey;
			}
			
			if (regen->type == REGEN_TYPE_MOB)
			{
				const CMob * p = CMobManager::instance().Get(regen->vnum);

				if (!p)
				{
					sys_err("In %s, No mob data by vnum %u", filename, regen->vnum);
				}
				else if (p->m_table.bType == CHAR_TYPE_NPC || p->m_table.bType == CHAR_TYPE_WARP || p->m_table.bType == CHAR_TYPE_GOTO)
				{
					SECTREE_MANAGER::instance().InsertNPCPosition(lMapIndex,
							p->m_table.bType,
#ifdef ENABLE_MULTI_NAMES
							regen->vnum
#else
							p->m_table.szLocaleName
#endif
							, (regen->sx+regen->ex) / 2 - base_x,
							(regen->sy+regen->ey) / 2 - base_y);
				}
				else if (p->m_table.bType == CHAR_TYPE_MONSTER)
				{
					if (p->m_table.bRank == EMobRank::MOB_RANK_BOSS || p->m_table.bRank == EMobRank::MOB_RANK_KING)
					{
						regen->boss_pos = g_BossPosition[lMapIndex].emplace_back(new SBossPosition(regen->vnum, (regen->sx + regen->ex) / 2 - base_x, (regen->sy + regen->ey) / 2 - base_y, regen->time));
					}
				}
			}
			else if (regen->type == REGEN_TYPE_GROUP)
			{
				if (CMobGroup* group = CMobManager::Instance().GetGroup(regen->vnum))
				{
					for (const auto& vnum : group->GetMemberVector()) {
						const CMob* mob = CMobManager::Instance().Get(vnum);

						if (!mob)
						{
							sys_err("In %s, No mob data by vnum %u", filename, regen->vnum);
							continue;
						}

						if (mob->m_table.bType == CHAR_TYPE_MONSTER)
						{
							if (mob->m_table.bRank == EMobRank::MOB_RANK_BOSS || mob->m_table.bRank == EMobRank::MOB_RANK_KING)
							{
								regen->boss_pos = g_BossPosition[lMapIndex].emplace_back(new SBossPosition(vnum, (regen->sx + regen->ex) / 2 - base_x, (regen->sy + regen->ey) / 2 - base_y, regen->time));
							}
						}
				}
			}
		}
#ifdef ENABLE_ATLAS_BOSS
			if ((bossFile) && (tmp.type == REGEN_TYPE_MOB)) {
				const CMob * p = CMobManager::instance().Get(regen->vnum);
				if (!p)
					sys_err("In %s, No mob data by vnum %u", filename, regen->vnum);
				else {
					SECTREE_MANAGER::instance().InsertBossPosition(lMapIndex, p->m_table.bType, 
#ifdef ENABLE_MULTI_NAMES
					regen->vnum
#else
					p->m_table.szLocaleName
#endif
					, (regen->sx+regen->ex) / 2 - base_x, (regen->sy+regen->ey) / 2 - base_y, tmp.time);
				}
			}
#endif
			
			//NO_REGEN
			// Desc: 	regen.txt (외 리젠관련 텍스트 ) 에서 리젠 시간을 0으로 세팅할시
			// 			리젠을 하지 안한다.
			if (regen->time != 0)
			{
				// 처음엔 무조건 리젠 해준다.
				if (!regen->is_static)
				{
					regen_spawn(regen, false);
					regen_event_info* info = AllocEventInfo<regen_event_info>();

					info->regen = regen;

					regen->event = event_create(regen_event, info, PASSES_PER_SEC(number(0, 16)) + PASSES_PER_SEC(regen->time));
				}
			}
			//END_NO_REGEN
		}
		else if (tmp.type == REGEN_TYPE_EXCEPTION)
		{
			LPREGEN_EXCEPTION exc;

			exc = M2_NEW REGEN_EXCEPTION;

			exc->sx = tmp.sx;
			exc->sy = tmp.sy;
			exc->ex = tmp.ex;
			exc->ey = tmp.ey;
			exc->z_section = tmp.z_section;

			INSERT_TO_TW_LIST(exc, regen_exception_list, prev, next);
		}
	}

	fclose(fp);
	return true;
}

void regen_free(void)
{
	g_BossPosition.clear();

	LPREGEN		regen, next_regen;
	LPREGEN_EXCEPTION	exc, next_exc;

	for (regen = regen_list; regen; regen = next_regen)
	{
		next_regen = regen->next;

		event_cancel(&regen->event);
		M2_DELETE(regen);
	}

	regen_list = NULL;

	for (exc = regen_exception_list; exc; exc = next_exc)
	{
		next_exc = exc->next;

		M2_DELETE(exc);
	}

	regen_exception_list = NULL;
}

static void check_regen_static(LPREGEN regen, int hour, int minute, int second)
{
	static constexpr int seconds_in_minute = 60;
	static constexpr int seconds_in_hour = 3600;
	static constexpr int seconds_in_day = 86400;

	if (regen->time < seconds_in_minute)
	{
		if (regen->time == second)
			regen_spawn(regen, false);
	}
	else if (regen->time >= seconds_in_minute && regen->time < seconds_in_hour)
	{
		if ((minute % (regen->time / seconds_in_minute) == 0) && second == 0)
			regen_spawn(regen, false);
	}
	else if (regen->time >= seconds_in_hour && regen->time < seconds_in_day)
	{
		if (const uint32_t hour_ = regen->time / seconds_in_hour; (hour % hour_ == 0) && ((regen->time - (hour_ * seconds_in_hour)) / 60) == minute && second == 0)
			regen_spawn(regen, false);
	}
}

int time_until_next_respawn_static(const LPREGEN regen, const int hour, const int minute, const int second)
{
	if (regen->time == 0)
		return 0;

	static constexpr int seconds_in_minute = 60;
	static constexpr int seconds_in_hour = 3600;

	const int timeSinceLastRespawn = hour * seconds_in_hour + minute * seconds_in_minute + second;
	if (timeSinceLastRespawn == 0)
		return 0;

	// Calculate the time remaining until the next scheduled respawn
	const int secondsUntilNextRespawn = regen->time - (timeSinceLastRespawn % regen->time);

	return secondsUntilNextRespawn;
}

void regen_spawn_static(const int hour, const int minute, const int second)
{
	if (regen_list != nullptr)
	{
		for (auto regen = regen_list; regen; regen = regen->next)
		{
			if (regen->boss_pos)
				regen->boss_pos->is_alive = regen->count > 0;

			if (!regen->is_static)
				continue;

			check_regen_static(regen, hour, minute, second);
		}
	}
}

void regen_reset(int x, int y)
{
	LPREGEN regen;

	for (regen = regen_list; regen; regen = regen->next)
	{
		if (!regen->event)
			continue;

		// 좌표가 있으면 좌표 내에 있는 리젠 리스트만 리젠 시킨다.
		if (x != 0 || y != 0)
		{
			if (x >= regen->sx && x <= regen->ex)
				if (y >= regen->sy && y <= regen->ey)
					event_reset_time(regen->event, 1);
		}
		// 없으면 전부 리젠
		else
			event_reset_time(regen->event, 1);
	}
}

bool is_valid_regen(LPREGEN currRegen)
{
	LPREGEN regen;

	for (regen = regen_list; regen; regen = regen->next)
	{
		if (regen == currRegen)
		{
			return true;
		}
	}

	return false;
}

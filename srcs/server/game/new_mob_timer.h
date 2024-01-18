#pragma once

#if defined(ENABLE_ULTIMATE_REGEN)
//EVERDAY = 0,
//SUNDAY = 1,
//MONDAY = 2,
//TUESDAY = 3,
//WEDNESDAY = 4,
//THURSDAY = 5,
//FRIDAY = 6,
//SATURDAY = 7,

enum ENewRegenFile {
	R_ID,
	R_MAPINDEX,
	R_MAPNAME,
	R_X,
	R_Y,
	R_DIRECTION,
	R_CHANNEL,
	R_DAY,
	R_HOUR,
	R_MINUTE,
	R_SECOND,
	R_BROADCAST,
	R_SAFE_RANGE,
	R_DAYS_RANGE,
	R_HOURS_RANGE,
	R_MINUTE_RANGE,
	R_VNUM,
};

typedef struct SNewRegen {
	WORD id;
	long mapIndex;
	long x, y;
	uint8_t direction;
	uint8_t channel;
	uint8_t day, hour, minute, second;
	bool broadcast;
	long safeRange;
	uint32_t mob_vnum;
	uint8_t days_range;
	uint8_t hours_range;
	uint8_t minute_range;
	LPCHARACTER	bossPtr;
	bool blockAttack;
	bool p2pAlive;
	int32_t leftTime;
} TNewRegen;

class CNewMobTimer : public singleton<CNewMobTimer> {
	public:
		CNewMobTimer();
		~CNewMobTimer();

		void Destroy();
		void Initialize();

	protected:
		LPEVENT m_pkMobRegenTimerEvent;
		std::vector<TNewRegen> m_vecRegenData;
		std::vector<uint32_t> m_vecHasVnums;

	public:
		long Update();

		void CalculateLeftTimeReal(TNewRegen*);
		int CalculateLeftTime(TNewRegen*);
		void GetTrackData(LPCHARACTER, WORD);
		void UpdateNewRegen(WORD id, bool isAlive, bool isP2P = false);
		void UpdatePlayers();
#if defined(ENABLE_TEXTS_RENEWAL) && defined(ENABLE_WORLDBOSS)
		bool IsWorldBoss(uint32_t);
#endif

		bool HasMob(uint32_t);

		bool LoadFile(const char*);
		bool ReadLine(FILE*, TNewRegen&);

		bool CheckDamage(LPCHARACTER, LPCHARACTER);
		void Dead(LPCHARACTER, LPCHARACTER);

		void SpawnBoss(const SECTREE_MAP*, TNewRegen&);
};
#endif

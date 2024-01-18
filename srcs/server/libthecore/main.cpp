#include "stdafx.h"

extern void GOST_Init();

LPHEART thecore_heart = nullptr;

volatile int32_t shutdowned = FALSE;
volatile int32_t tics = 0;
uint32_t thecore_profiler[NUM_PF];

// newstuff
int32_t bCheckpointCheck = 1;

static bool pid_init()
{
#ifndef _WIN32
	FILE* fp = nullptr;
	if ((fp = fopen("pid", "w")))
	{
		fprintf(fp, "%d", getpid());
		fclose(fp);
		sys_log(0, "\nStart of pid: %d\n", getpid()); // @warme012
	}
	else
	{
		printf("pid_init(): could not open file for writing. (filename: ./pid)");
		sys_err("\nError writing pid file\n");
		return false;
	}
#endif
	return true;
}

static void pid_deinit()
{
#ifndef _WIN32
    remove("./pid");
	sys_log(0, "\nEnd of pid\n"); // @warme012
#endif
}

bool thecore_init(int32_t fps, HEARTFUNC heartbeat_func)
{
#ifdef _WIN32
	srand(time(nullptr));
#else
	srandom(time(0) + getpid() + getuid());
	srandomdev();
#endif
	signal_setup();

	if (!log_init() || !pid_init())
		return false;

	GOST_Init();

	thecore_heart = heart_new(1000000 / fps, heartbeat_func);
	return true;
}

void thecore_shutdown()
{
	shutdowned = TRUE;
}

int32_t thecore_idle()
{
	thecore_tick();

	if (shutdowned)
		return 0;

	int32_t pulses;
	uint32_t t = get_dword_time();

	if (!((pulses = heart_idle(thecore_heart))))
	{
		thecore_profiler[PF_IDLE] += (get_dword_time() - t);
		return 0;
	}

	thecore_profiler[PF_IDLE] += (get_dword_time() - t);
	return pulses;
}

void thecore_destroy()
{
	pid_deinit();
	log_destroy();
}

int32_t thecore_pulse()
{
	return (thecore_heart->pulse);
}

float thecore_pulse_per_second()
{
	return static_cast<float>(thecore_heart->passes_per_sec);
}

float thecore_time()
{
	return static_cast<float>(thecore_heart->pulse) / static_cast<float>(thecore_heart->passes_per_sec);
}

int32_t thecore_is_shutdowned()
{
	return shutdowned;
}

void thecore_tick()
{
	++tics;
}

#ifndef __INC_LIBTHECORE_FDWATCH_H__
#define __INC_LIBTHECORE_FDWATCH_H__

#ifndef _WIN32

    typedef struct fdwatch	FDWATCH;
    typedef struct fdwatch *	LPFDWATCH;

    enum EFdwatch
    {
	FDW_NONE		= 0,
	FDW_READ		= 1,
	FDW_WRITE		= 2,
	FDW_WRITE_ONESHOT	= 4,
	FDW_EOF			= 8
    };

    typedef struct kevent	KEVENT;
    typedef struct kevent *	LPKEVENT;
    typedef int32_t			KQUEUE;

    struct fdwatch
    {
	KQUEUE		kq;

	int32_t		nfiles;

	LPKEVENT	kqevents;
	int32_t		nkqevents;

	LPKEVENT	kqrevents;
	int32_t *		fd_event_idx;

	void **		fd_data;
	int32_t *		fd_rw;
    };

#else

typedef struct fdwatch FDWATCH;
typedef struct fdwatch* LPFDWATCH;

enum EFdwatch
{
	FDW_NONE = 0,
	FDW_READ = 1,
	FDW_WRITE = 2,
	FDW_WRITE_ONESHOT = 4,
	FDW_EOF = 8,
};

struct fdwatch
{
	fd_set rfd_set;
	fd_set wfd_set;

	socket_t* select_fds;
	int32_t* select_rfdidx;

	int32_t nselect_fds;

	fd_set working_rfd_set;
	fd_set working_wfd_set;

	int32_t nfiles;

	void** fd_data;
	int32_t* fd_rw;
};

#endif // WIN32

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern LPFDWATCH fdwatch_new(int32_t nfiles);
extern void fdwatch_clear_fd(LPFDWATCH fdw, socket_t fd);
extern void fdwatch_delete(LPFDWATCH fdw);
extern int32_t fdwatch_check_fd(LPFDWATCH fdw, socket_t fd);
extern int32_t fdwatch_check_event(LPFDWATCH fdw, socket_t fd, uint32_t event_idx);
extern void fdwatch_clear_event(LPFDWATCH fdw, socket_t fd, uint32_t event_idx);
extern void fdwatch_add_fd(LPFDWATCH fdw, socket_t fd, void* client_data, int32_t rw, int32_t oneshot);
extern int32_t fdwatch(LPFDWATCH fdw, struct timeval* timeout);
extern void* fdwatch_get_client_data(LPFDWATCH fdw, uint32_t event_idx);
extern void fdwatch_del_fd(LPFDWATCH fdw, socket_t fd);
extern int32_t fdwatch_get_buffer_size(LPFDWATCH fdw, socket_t fd);
extern int32_t fdwatch_get_ident(LPFDWATCH fdw, uint32_t event_idx);

#ifdef __cplusplus
}
#endif

#endif

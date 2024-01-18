#ifndef __INC_LIBTHECORE_EVENT_H__
#define __INC_LIBTHECORE_EVENT_H__

#if VSTD_HAS_PRAGMA_ONCE
#pragma once
#endif

#include <intrusive_ptr.h>

struct event_info_data
{
    event_info_data()
    {
        
    }

    virtual ~event_info_data()
    {
        
    }
};

typedef struct event EVENT;
typedef eastl::intrusive_ptr<EVENT> LPEVENT;
typedef int32_t (*TEVENTFUNC) (LPEVENT event, int32_t processing_time);

#define EVENTFUNC(name) int32_t (name) (LPEVENT event, int32_t processing_time)
#define EVENTINFO(name) struct name : public event_info_data

struct TQueueElement;

struct event
{
    event() = default;

    ~event()
    {
        if (info != nullptr)
        {
            delete info;
        }
    }

    TEVENTFUNC          func{};
    event_info_data*    info{};
    TQueueElement *     q_el{};
    char                is_force_to_end{};
    char                is_processing{};
    size_t              ref_count{};
};

extern void intrusive_ptr_add_ref(EVENT* p);
extern void intrusive_ptr_release(EVENT* p);

template<class T> // T should be a subclass of event_info_data
T* AllocEventInfo()
{
    return new T;
}

extern void     event_destroy();
extern int32_t  event_process(int32_t pulse);
extern int32_t  event_count();

extern LPEVENT  event_create_ex(TEVENTFUNC func, event_info_data* info, int32_t when);

#define event_create(func, info, when) event_create_ex(func, info, when)

extern void     event_cancel(LPEVENT * event);
extern int32_t  event_processing_time(LPEVENT event);
extern int32_t  event_time(LPEVENT event);
extern void     event_reset_time(LPEVENT event, int32_t when);
extern void     event_set_verbose(int32_t level);

extern event_info_data* FindEventInfo(DWORD dwID);
extern event_info_data* event_info(LPEVENT event);
#endif // __INC_LIBTHECORE_EVENT_H__

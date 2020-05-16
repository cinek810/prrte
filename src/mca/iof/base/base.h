/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2011 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2008-2020 Cisco Systems, Inc.  All rights reserved
 * Copyright (c) 2012-2013 Los Alamos National Security, LLC.
 *                         All rights reserved.
 * Copyright (c) 2015-2019 Intel, Inc.  All rights reserved.
 * Copyright (c) 2017      IBM Corporation.  All rights reserved.
 * Copyright (c) 2017      Mellanox Technologies. All rights reserved.
 * Copyright (c) 2018      Research Organization for Information Science
 *                         and Technology (RIST).  All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
/**
 * @file
 *
 * I/O Forwarding Service
 */

#ifndef MCA_IOF_BASE_H
#define MCA_IOF_BASE_H

#include "prte_config.h"
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_UIO_H
#include <sys/uio.h>
#endif
#ifdef HAVE_NET_UIO_H
#include <net/uio.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <signal.h>

#include "src/class/prte_list.h"
#include "src/class/prte_bitmap.h"
#include "src/mca/mca.h"
#include "src/event/event-internal.h"
#include "src/util/fd.h"

#include "src/mca/iof/iof.h"
#include "src/runtime/prte_globals.h"
#include "src/mca/rml/rml_types.h"
#include "src/threads/threads.h"
#include "src/mca/errmgr/errmgr.h"

BEGIN_C_DECLS

/*
 * MCA framework
 */
PRTE_EXPORT extern prte_mca_base_framework_t prte_iof_base_framework;
/*
 * Select an available component.
 */
PRTE_EXPORT int prte_iof_base_select(void);

/* track xon/xoff of processes */
typedef struct {
    prte_object_t super;
    prte_job_t *jdata;
    prte_bitmap_t xoff;
} prte_iof_job_t;
PRTE_EXPORT PRTE_CLASS_DECLARATION(prte_iof_job_t);

/*
 * Maximum size of single msg
 */
#define PRTE_IOF_BASE_MSG_MAX           4096
#define PRTE_IOF_BASE_TAG_MAX             50
#define PRTE_IOF_BASE_TAGGED_OUT_MAX    8192
#define PRTE_IOF_MAX_INPUT_BUFFERS        50

typedef struct {
    prte_list_item_t super;
    bool pending;
    bool always_writable;
    prte_event_t *ev;
    struct timeval tv;
    int fd;
    prte_list_t outputs;
} prte_iof_write_event_t;
PRTE_EXPORT PRTE_CLASS_DECLARATION(prte_iof_write_event_t);

typedef struct {
    prte_list_item_t super;
    prte_process_name_t name;
    prte_process_name_t daemon;
    prte_iof_tag_t tag;
    prte_iof_write_event_t *wev;
    bool xoff;
    bool exclusive;
    bool closed;
} prte_iof_sink_t;
PRTE_EXPORT PRTE_CLASS_DECLARATION(prte_iof_sink_t);

struct prte_iof_proc_t;
typedef struct {
    prte_object_t super;
    struct prte_iof_proc_t *proc;
    prte_event_t *ev;
    struct timeval tv;
    int fd;
    prte_iof_tag_t tag;
    bool active;
    bool always_readable;
    prte_iof_sink_t *sink;
} prte_iof_read_event_t;
PRTE_EXPORT PRTE_CLASS_DECLARATION(prte_iof_read_event_t);

typedef struct {
    prte_list_item_t super;
    prte_process_name_t name;
    prte_iof_sink_t *stdinev;
    prte_iof_read_event_t *revstdout;
    prte_iof_read_event_t *revstderr;
    prte_list_t *subscribers;
    bool copy;
} prte_iof_proc_t;
PRTE_EXPORT PRTE_CLASS_DECLARATION(prte_iof_proc_t);

typedef struct {
    prte_list_item_t super;
    char data[PRTE_IOF_BASE_TAGGED_OUT_MAX];
    int numbytes;
} prte_iof_write_output_t;
PRTE_EXPORT PRTE_CLASS_DECLARATION(prte_iof_write_output_t);

/* the iof globals struct */
struct prte_iof_base_t {
    size_t                  output_limit;
    prte_iof_sink_t         *iof_write_stdout;
    prte_iof_sink_t         *iof_write_stderr;
    bool                    redirect_app_stderr_to_stdout;
};
typedef struct prte_iof_base_t prte_iof_base_t;

/* Write event macro's */

static inline bool
prte_iof_base_fd_always_ready(int fd)
{
    return prte_fd_is_regular(fd) ||
           (prte_fd_is_chardev(fd) && !isatty(fd)) ||
           prte_fd_is_blkdev(fd);
}

#define PRTE_IOF_SINK_BLOCKSIZE (1024)

#define PRTE_IOF_SINK_ACTIVATE(wev)                                     \
    do {                                                                \
        struct timeval *tv = NULL;                                      \
        wev->pending = true;                                            \
        PRTE_POST_OBJECT(wev);                                          \
        if (wev->always_writable) {                                     \
            /* Regular is always write ready. Use timer to activate */  \
            tv = &wev->tv;                                        \
        }                                                               \
        if (prte_event_add(wev->ev, tv)) {                              \
            PRTE_ERROR_LOG(PRTE_ERR_BAD_PARAM);                         \
        }                                                               \
    } while(0);


/* define an output "sink", adding it to the provided
 * endpoint list for this proc */
#define PRTE_IOF_SINK_DEFINE(snk, nm, fid, tg, wrthndlr)                \
    do {                                                                \
        prte_iof_sink_t *ep;                                            \
        PRTE_OUTPUT_VERBOSE((1,                                         \
                            prte_iof_base_framework.framework_output,   \
                            "defining endpt: file %s line %d fd %d",    \
                            __FILE__, __LINE__, (fid)));                \
        ep = PRTE_NEW(prte_iof_sink_t);                                  \
        ep->name.jobid = (nm)->jobid;                                   \
        ep->name.vpid = (nm)->vpid;                                     \
        ep->tag = (tg);                                                 \
        if (0 <= (fid)) {                                               \
            ep->wev->fd = (fid);                                        \
            ep->wev->always_writable =                                  \
                    prte_iof_base_fd_always_ready(fid);                 \
            if(ep->wev->always_writable) {                              \
                prte_event_evtimer_set(prte_event_base,                 \
                                       ep->wev->ev,  wrthndlr, ep);     \
            } else {                                                    \
                prte_event_set(prte_event_base,                         \
                               ep->wev->ev, ep->wev->fd,                \
                               PRTE_EV_WRITE,                           \
                               wrthndlr, ep);                           \
            }                                                           \
            prte_event_set_priority(ep->wev->ev, PRTE_MSG_PRI);         \
        }                                                               \
        *(snk) = ep;                                                    \
        PRTE_POST_OBJECT(ep);                                           \
    } while(0);

/* Read event macro's */
#define PRTE_IOF_READ_ADDEV(rev)                                \
    do {                                                        \
        struct timeval *tv = NULL;                              \
        if (rev->always_readable) {                             \
            tv = &rev->tv;                                      \
        }                                                       \
        if (prte_event_add(rev->ev, tv)) {                      \
            PRTE_ERROR_LOG(PRTE_ERR_BAD_PARAM);                 \
        }                                                       \
    } while(0);

#define PRTE_IOF_READ_ACTIVATE(rev)                             \
    do {                                                        \
        rev->active = true;                                     \
        PRTE_POST_OBJECT(rev);                                  \
        PRTE_IOF_READ_ADDEV(rev);                               \
    } while(0);


/* add list of structs that has name of proc + prte_iof_tag_t - when
 * defining a read event, search list for proc, add flag to the tag.
 * when closing a read fd, find proc on list and zero out that flag
 * when all flags = 0, then iof is complete - set message event to
 * daemon processor indicating proc iof is terminated
 */
#define PRTE_IOF_READ_EVENT(rv, p, fid, tg, cbfunc, actv)               \
    do {                                                                \
        prte_iof_read_event_t *rev;                                     \
        PRTE_OUTPUT_VERBOSE((1,                                         \
                            prte_iof_base_framework.framework_output,   \
                            "%s defining read event for %s: %s %d",     \
                            PRTE_NAME_PRINT(PRTE_PROC_MY_NAME),         \
                            PRTE_NAME_PRINT(&(p)->name),                \
                            __FILE__, __LINE__));                       \
        rev = PRTE_NEW(prte_iof_read_event_t);                           \
        PRTE_RETAIN((p));                                                \
        rev->proc = (struct prte_iof_proc_t*)(p);                       \
        rev->tag = (tg);                                                \
        rev->fd = (fid);                                                \
        rev->always_readable = prte_iof_base_fd_always_ready(fid);      \
        *(rv) = rev;                                                    \
        if(rev->always_readable) {                                      \
            prte_event_evtimer_set(prte_event_base,                     \
                                   rev->ev, (cbfunc), rev);             \
        } else {                                                        \
            prte_event_set(prte_event_base,                             \
                           rev->ev, (fid),                              \
                           PRTE_EV_READ,                                \
                           (cbfunc), rev);                              \
        }                                                               \
        prte_event_set_priority(rev->ev, PRTE_MSG_PRI);                 \
        if ((actv)) {                                                   \
            PRTE_IOF_READ_ACTIVATE(rev)                                 \
        }                                                               \
    } while(0);


PRTE_EXPORT int prte_iof_base_flush(void);

PRTE_EXPORT extern prte_iof_base_t prte_iof_base;

/* base functions */
PRTE_EXPORT int prte_iof_base_write_output(const prte_process_name_t *name, prte_iof_tag_t stream,
                                             const unsigned char *data, int numbytes,
                                             prte_iof_write_event_t *channel);
PRTE_EXPORT void prte_iof_base_static_dump_output(prte_iof_read_event_t *rev);
PRTE_EXPORT void prte_iof_base_write_handler(int fd, short event, void *cbdata);

END_C_DECLS

#endif /* MCA_IOF_BASE_H */

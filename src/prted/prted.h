/*
 * Copyright (c) 2004-2007 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2011 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2019      Intel, Inc.  All rights reserved.
 * Copyright (c) 2020      Cisco Systems, Inc.  All rights reserved
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#ifndef PRTED_H
#define PRTED_H

#include "prte_config.h"
#include "types.h"

#include <time.h>

#include "src/dss/dss_types.h"
#include "src/class/prte_pointer_array.h"
#include "src/mca/rml/rml_types.h"

BEGIN_C_DECLS

/* main orted routine */
PRTE_EXPORT int prte_daemon(int argc, char *argv[]);

/* orted communication functions */
PRTE_EXPORT void prte_daemon_recv(int status, prte_process_name_t* sender,
                      prte_buffer_t *buffer, prte_rml_tag_t tag,
                      void* cbdata);

/* direct cmd processing entry points */
PRTE_EXPORT void prte_daemon_cmd_processor(int fd, short event, void *data);
PRTE_EXPORT int prte_daemon_process_commands(prte_process_name_t* sender,
                                               prte_buffer_t *buffer,
                                               prte_rml_tag_t tag);

END_C_DECLS

/* Local function */
int send_to_local_applications(prte_pointer_array_t *dead_names);

#endif /* PRTED_H */

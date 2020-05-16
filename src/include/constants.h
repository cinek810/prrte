/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2006 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2014      Research Organization for Information Science
 *                         and Technology (RIST). All rights reserved.
 * Copyright (c) 2015-2019 Intel, Inc.  All rights reserved.
 * Copyright (c) 2020      Cisco Systems, Inc.  All rights reserved
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#ifndef PRTE_CONSTANTS_H
#define PRTE_CONSTANTS_H

#include "constants.h"
#include "prte_config.h"

BEGIN_C_DECLS

#define PRTE_ERR_BASE            0
#define PRTE_ERR_SPLIT         100

enum {
    /* Error codes inherited from PRTE.  Still enum values so that we
       get the nice debugger help. */

    PRTE_SUCCESS                            = (PRTE_ERR_BASE),

    PRTE_ERROR                              = (PRTE_ERR_BASE -  1),
    PRTE_ERR_OUT_OF_RESOURCE                = (PRTE_ERR_BASE -  2), /* fatal error */
    PRTE_ERR_TEMP_OUT_OF_RESOURCE           = (PRTE_ERR_BASE -  3), /* try again later */
    PRTE_ERR_RESOURCE_BUSY                  = (PRTE_ERR_BASE -  4),
    PRTE_ERR_BAD_PARAM                      = (PRTE_ERR_BASE -  5),  /* equivalent to MPI_ERR_ARG error code */
    PRTE_ERR_FATAL                          = (PRTE_ERR_BASE -  6),
    PRTE_ERR_NOT_IMPLEMENTED                = (PRTE_ERR_BASE -  7),
    PRTE_ERR_NOT_SUPPORTED                  = (PRTE_ERR_BASE -  8),
    PRTE_ERR_INTERRUPTED                    = (PRTE_ERR_BASE -  9),
    PRTE_ERR_WOULD_BLOCK                    = (PRTE_ERR_BASE - 10),
    PRTE_ERR_IN_ERRNO                       = (PRTE_ERR_BASE - 11),
    PRTE_ERR_UNREACH                        = (PRTE_ERR_BASE - 12),
    PRTE_ERR_NOT_FOUND                      = (PRTE_ERR_BASE - 13),
    PRTE_EXISTS                             = (PRTE_ERR_BASE - 14), /* indicates that the specified object already exists */
    PRTE_ERR_TIMEOUT                        = (PRTE_ERR_BASE - 15),
    PRTE_ERR_NOT_AVAILABLE                  = (PRTE_ERR_BASE - 16),
    PRTE_ERR_PERM                           = (PRTE_ERR_BASE - 17), /* no permission */
    PRTE_ERR_VALUE_OUT_OF_BOUNDS            = (PRTE_ERR_BASE - 18),
    PRTE_ERR_FILE_READ_FAILURE              = (PRTE_ERR_BASE - 19),
    PRTE_ERR_FILE_WRITE_FAILURE             = (PRTE_ERR_BASE - 20),
    PRTE_ERR_FILE_OPEN_FAILURE              = (PRTE_ERR_BASE - 21),
    PRTE_ERR_PACK_MISMATCH                  = (PRTE_ERR_BASE - 22),
    PRTE_ERR_PACK_FAILURE                   = (PRTE_ERR_BASE - 23),
    PRTE_ERR_UNPACK_FAILURE                 = (PRTE_ERR_BASE - 24),
    PRTE_ERR_UNPACK_INADEQUATE_SPACE        = (PRTE_ERR_BASE - 25),
    PRTE_ERR_UNPACK_READ_PAST_END_OF_BUFFER = (PRTE_ERR_BASE - 26),
    PRTE_ERR_TYPE_MISMATCH                  = (PRTE_ERR_BASE - 27),
    PRTE_ERR_OPERATION_UNSUPPORTED          = (PRTE_ERR_BASE - 28),
    PRTE_ERR_UNKNOWN_DATA_TYPE              = (PRTE_ERR_BASE - 29),
    PRTE_ERR_BUFFER                         = (PRTE_ERR_BASE - 30),
    PRTE_ERR_DATA_TYPE_REDEF                = (PRTE_ERR_BASE - 31),
    PRTE_ERR_DATA_OVERWRITE_ATTEMPT         = (PRTE_ERR_BASE - 32),
    PRTE_ERR_MODULE_NOT_FOUND               = (PRTE_ERR_BASE - 33),
    PRTE_ERR_TOPO_SLOT_LIST_NOT_SUPPORTED   = (PRTE_ERR_BASE - 34),
    PRTE_ERR_TOPO_SOCKET_NOT_SUPPORTED      = (PRTE_ERR_BASE - 35),
    PRTE_ERR_TOPO_CORE_NOT_SUPPORTED        = (PRTE_ERR_BASE - 36),
    PRTE_ERR_NOT_ENOUGH_SOCKETS             = (PRTE_ERR_BASE - 37),
    PRTE_ERR_NOT_ENOUGH_CORES               = (PRTE_ERR_BASE - 38),
    PRTE_ERR_INVALID_PHYS_CPU               = (PRTE_ERR_BASE - 39),
    PRTE_ERR_MULTIPLE_AFFINITIES            = (PRTE_ERR_BASE - 40),
    PRTE_ERR_SLOT_LIST_RANGE                = (PRTE_ERR_BASE - 41),
    PRTE_ERR_NETWORK_NOT_PARSEABLE          = (PRTE_ERR_BASE - 42),
    PRTE_ERR_SILENT                         = (PRTE_ERR_BASE - 43),
    PRTE_ERR_NOT_INITIALIZED                = (PRTE_ERR_BASE - 44),
    PRTE_ERR_NOT_BOUND                      = (PRTE_ERR_BASE - 45),
    PRTE_ERR_TAKE_NEXT_OPTION               = (PRTE_ERR_BASE - 46),
    PRTE_ERR_PROC_ENTRY_NOT_FOUND           = (PRTE_ERR_BASE - 47),
    PRTE_ERR_DATA_VALUE_NOT_FOUND           = (PRTE_ERR_BASE - 48),
    PRTE_ERR_CONNECTION_FAILED              = (PRTE_ERR_BASE - 49),
    PRTE_ERR_AUTHENTICATION_FAILED          = (PRTE_ERR_BASE - 50),
    PRTE_ERR_COMM_FAILURE                   = (PRTE_ERR_BASE - 51),
    PRTE_ERR_SERVER_NOT_AVAIL               = (PRTE_ERR_BASE - 52),
    PRTE_ERR_IN_PROCESS                     = (PRTE_ERR_BASE - 53),
    /* PMIx equivalents for notification support */
    PRTE_ERR_DEBUGGER_RELEASE               = (PRTE_ERR_BASE - 54),
    PRTE_ERR_HANDLERS_COMPLETE              = (PRTE_ERR_BASE - 55),
    PRTE_ERR_PARTIAL_SUCCESS                = (PRTE_ERR_BASE - 56),
    PRTE_ERR_PROC_ABORTED                   = (PRTE_ERR_BASE - 57),
    PRTE_ERR_PROC_REQUESTED_ABORT           = (PRTE_ERR_BASE - 58),
    PRTE_ERR_PROC_ABORTING                  = (PRTE_ERR_BASE - 59),
    PRTE_ERR_NODE_DOWN                      = (PRTE_ERR_BASE - 60),
    PRTE_ERR_NODE_OFFLINE                   = (PRTE_ERR_BASE - 61),
    PRTE_ERR_JOB_TERMINATED                 = (PRTE_ERR_BASE - 62),
    PRTE_ERR_PROC_RESTART                   = (PRTE_ERR_BASE - 63),
    PRTE_ERR_PROC_CHECKPOINT                = (PRTE_ERR_BASE - 64),
    PRTE_ERR_PROC_MIGRATE                   = (PRTE_ERR_BASE - 65),
    PRTE_ERR_EVENT_REGISTRATION             = (PRTE_ERR_BASE - 66),
    PRTE_ERR_HEARTBEAT_ALERT                = (PRTE_ERR_BASE - 67),
    PRTE_ERR_FILE_ALERT                     = (PRTE_ERR_BASE - 68),
    PRTE_ERR_MODEL_DECLARED                 = (PRTE_ERR_BASE - 69),
    PRTE_PMIX_LAUNCH_DIRECTIVE              = (PRTE_ERR_BASE - 70),
    PRTE_PMIX_LAUNCHER_READY                = (PRTE_ERR_BASE - 71),
    PRTE_OPERATION_SUCCEEDED                = (PRTE_ERR_BASE - 72),

/* error codes specific to PRTE - don't forget to update
    src/util/error_strings.c when adding new error codes!!
    Otherwise, the error reporting system will potentially crash,
    or at the least not be able to report the new error correctly.
 */

    PRTE_ERR_RECV_LESS_THAN_POSTED          = (PRTE_ERR_SPLIT -  1),
    PRTE_ERR_RECV_MORE_THAN_POSTED          = (PRTE_ERR_SPLIT -  2),
    PRTE_ERR_NO_MATCH_YET                   = (PRTE_ERR_SPLIT -  3),
    PRTE_ERR_REQUEST                        = (PRTE_ERR_SPLIT -  4),
    PRTE_ERR_NO_CONNECTION_ALLOWED          = (PRTE_ERR_SPLIT -  5),
    PRTE_ERR_CONNECTION_REFUSED             = (PRTE_ERR_SPLIT -  6),
    PRTE_ERR_COMPARE_FAILURE                = (PRTE_ERR_SPLIT -  9),
    PRTE_ERR_COPY_FAILURE                   = (PRTE_ERR_SPLIT - 10),
    PRTE_ERR_PROC_STATE_MISSING             = (PRTE_ERR_SPLIT - 11),
    PRTE_ERR_PROC_EXIT_STATUS_MISSING       = (PRTE_ERR_SPLIT - 12),
    PRTE_ERR_INDETERMINATE_STATE_INFO       = (PRTE_ERR_SPLIT - 13),
    PRTE_ERR_NODE_FULLY_USED                = (PRTE_ERR_SPLIT - 14),
    PRTE_ERR_INVALID_NUM_PROCS              = (PRTE_ERR_SPLIT - 15),
    PRTE_ERR_ADDRESSEE_UNKNOWN              = (PRTE_ERR_SPLIT - 16),
    PRTE_ERR_SYS_LIMITS_PIPES               = (PRTE_ERR_SPLIT - 17),
    PRTE_ERR_PIPE_SETUP_FAILURE             = (PRTE_ERR_SPLIT - 18),
    PRTE_ERR_SYS_LIMITS_CHILDREN            = (PRTE_ERR_SPLIT - 19),
    PRTE_ERR_FAILED_GET_TERM_ATTRS          = (PRTE_ERR_SPLIT - 20),
    PRTE_ERR_WDIR_NOT_FOUND                 = (PRTE_ERR_SPLIT - 21),
    PRTE_ERR_EXE_NOT_FOUND                  = (PRTE_ERR_SPLIT - 22),
    PRTE_ERR_PIPE_READ_FAILURE              = (PRTE_ERR_SPLIT - 23),
    PRTE_ERR_EXE_NOT_ACCESSIBLE             = (PRTE_ERR_SPLIT - 24),
    PRTE_ERR_FAILED_TO_START                = (PRTE_ERR_SPLIT - 25),
    PRTE_ERR_FILE_NOT_EXECUTABLE            = (PRTE_ERR_SPLIT - 26),
    PRTE_ERR_HNP_COULD_NOT_START            = (PRTE_ERR_SPLIT - 27),
    PRTE_ERR_SYS_LIMITS_SOCKETS             = (PRTE_ERR_SPLIT - 28),
    PRTE_ERR_SOCKET_NOT_AVAILABLE           = (PRTE_ERR_SPLIT - 29),
    PRTE_ERR_SYSTEM_WILL_BOOTSTRAP          = (PRTE_ERR_SPLIT - 30),
    PRTE_ERR_RESTART_LIMIT_EXCEEDED         = (PRTE_ERR_SPLIT - 31),
    PRTE_ERR_INVALID_NODE_RANK              = (PRTE_ERR_SPLIT - 32),
    PRTE_ERR_INVALID_LOCAL_RANK             = (PRTE_ERR_SPLIT - 33),
    PRTE_ERR_UNRECOVERABLE                  = (PRTE_ERR_SPLIT - 34),
    PRTE_ERR_MEM_LIMIT_EXCEEDED             = (PRTE_ERR_SPLIT - 35),
    PRTE_ERR_HEARTBEAT_LOST                 = (PRTE_ERR_SPLIT - 36),
    PRTE_ERR_PROC_STALLED                   = (PRTE_ERR_SPLIT - 37),
    PRTE_ERR_NO_APP_SPECIFIED               = (PRTE_ERR_SPLIT - 38),
    PRTE_ERR_NO_EXE_SPECIFIED               = (PRTE_ERR_SPLIT - 39),
    PRTE_ERR_COMM_DISABLED                  = (PRTE_ERR_SPLIT - 40),
    PRTE_ERR_FAILED_TO_MAP                  = (PRTE_ERR_SPLIT - 41),
    PRTE_ERR_SENSOR_LIMIT_EXCEEDED          = (PRTE_ERR_SPLIT - 42),
    PRTE_ERR_ALLOCATION_PENDING             = (PRTE_ERR_SPLIT - 43),
    PRTE_ERR_NO_PATH_TO_TARGET              = (PRTE_ERR_SPLIT - 44),
    PRTE_ERR_OP_IN_PROGRESS                 = (PRTE_ERR_SPLIT - 45),
    PRTE_ERR_OPEN_CONDUIT_FAIL              = (PRTE_ERR_SPLIT - 46),
    PRTE_ERR_DUPLICATE_MSG                  = (PRTE_ERR_SPLIT - 47),
    PRTE_ERR_OUT_OF_ORDER_MSG               = (PRTE_ERR_SPLIT - 48),
    PRTE_ERR_FORCE_SELECT                   = (PRTE_ERR_SPLIT - 49),
    PRTE_ERR_JOB_CANCELLED                  = (PRTE_ERR_SPLIT - 50),
    PRTE_ERR_CONDUIT_SEND_FAIL              = (PRTE_ERR_SPLIT - 51)
};

#define PRTE_ERR_MAX                      (PRTE_ERR_SPLIT - 100)

END_C_DECLS

#endif /* PRTE_CONSTANTS_H */

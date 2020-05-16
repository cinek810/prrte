/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2004-2008 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2007 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2011-2015 Los Alamos National Security, LLC.
 *                         All rights reserved.
 * Copyright (c) 2015-2019 Intel, Inc.  All rights reserved.
 * Copyright (c) 2020      Cisco Systems, Inc.  All rights reserved
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include "prte_config.h"
#include "constants.h"

#include "src/mca/mca.h"
#include "src/mca/base/base.h"
#include "src/mca/base/prte_mca_base_component_repository.h"

#include "src/util/proc_info.h"

#include "src/mca/plm/base/plm_private.h"
#include "src/mca/plm/base/base.h"


/**
 * Function for selecting one component from all those that are
 * available.
 */

int prte_plm_base_select(void)
{
    int rc;
    prte_plm_base_component_t *best_component = NULL;
    prte_plm_base_module_t *best_module = NULL;

    /*
     * Select the best component
     */
    if (PRTE_SUCCESS == (rc = prte_mca_base_select("plm", prte_plm_base_framework.framework_output,
                                                     &prte_plm_base_framework.framework_components,
                                                     (prte_mca_base_module_t **) &best_module,
                                                     (prte_mca_base_component_t **) &best_component, NULL))) {
        /* Save the winner */
        prte_plm = *best_module;
    }

    return rc;
}

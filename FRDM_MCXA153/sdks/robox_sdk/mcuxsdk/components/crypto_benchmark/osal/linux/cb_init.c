/*
 * Copyright 2025 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <string.h>

#include <smw_osal.h>

#include "cb_types.h"

#define DEFAULT_OBJ_DB "/var/tmp/obj_db_crypto_benchmark.dat"

struct se_info se_info = { 0x50534154, 0x444546, 1000 }; // PSA, DEF

struct tee_info tee_info = { "11b5c4aa-6d20-11ea-bc55-0242ac130003" };

cb_status_t cb_osal_init_platform()
{
    return CB_STATUS_SUCCESS;
}

cb_status_t cb_osal_init_crypto()
{
    enum smw_status_code status = SMW_STATUS_OK;

    char *filepath = DEFAULT_OBJ_DB;

    status = smw_osal_set_subsystem_info(SMW_SUBSYSTEM_NAME_TEE,
                         &tee_info,
                         sizeof(tee_info));
    if (status != SMW_STATUS_OK)
    {
        return CB_STATUS_FAIL;
    }

    status = smw_osal_set_subsystem_info(SMW_SUBSYSTEM_NAME_ELE,
                         &se_info,
                         sizeof(se_info));
    if (status != SMW_STATUS_OK)
    {
        return CB_STATUS_FAIL;
    }

    status = smw_osal_set_subsystem_info(SMW_SUBSYSTEM_NAME_SECO,
                         &se_info,
                         sizeof(se_info));
    if (status != SMW_STATUS_OK)
    {
        return CB_STATUS_FAIL;
    }

    status = smw_osal_open_obj_db(filepath, strlen(filepath) + 1);
    if (status != SMW_STATUS_OK)
    {
        return CB_STATUS_FAIL;
    }

    status = smw_osal_lib_init();
    if (status != SMW_STATUS_OK &&
        status != SMW_STATUS_LIBRARY_ALREADY_INIT)
    {
        return CB_STATUS_FAIL;
    }

    return CB_STATUS_SUCCESS;
}

cb_status_t cb_osal_init_tick()
{
    return CB_STATUS_SUCCESS;
}


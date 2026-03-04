/*
 * Copyright 2022 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef ELE_H
#define ELE_H

#include "cb_osal.h"

typedef struct
{
    sss_sscp_key_store_t keyStore;
    sss_sscp_session_t sssSession;
    sscp_context_t sscpContext;
    sss_sscp_rng_t rngctx;

    bool is_fw_loaded;

} ele_s2xx_ctx_t;


#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif /* ELE_H */

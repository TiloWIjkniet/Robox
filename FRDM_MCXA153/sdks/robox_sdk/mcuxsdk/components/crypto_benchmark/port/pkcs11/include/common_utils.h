/*
 * Copyright 2025 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**
 * @file  common_utils.h
 * @brief Declarations for common utility functions for the PKCS11 layer.
 */

#ifndef CB_PKCS11_PORT_COMMON_UTILS_H
#define CB_PKCS11_PORT_COMMON_UTILS_H

/*
 * Define the platform-specific macros required by the pkcs11 headers
 * (refer to pkcs11.h)
 * Those macros must be defined before including the pkcs11.h
 */
#define CK_PTR *
#define CK_DECLARE_FUNCTION(returnType, name)                                  \
    returnType __attribute__((visibility("default"))) name
#define CK_DECLARE_FUNCTION_POINTER(returnType, name) returnType(*name)
#define CK_CALLBACK_FUNCTION(returnType, name)        returnType(*name)

#ifndef NULL_PTR
#define NULL_PTR NULL
#endif

#include <pkcs11.h>

#include "cb_types.h"

typedef struct _session_ctx_pkcs11
{
    void *lib_hdl;
    CK_FUNCTION_LIST_3_0_PTR pfunc;
    CK_SESSION_HANDLE sess;
} session_ctx_pkcs11_t;

#define SET_ATTRIBUTE(_attr, _class, _value)                                  \
    (_attr).type = _class;                                                    \
    (_attr).pValue = &(_value);                                               \
    (_attr).ulValueLen = sizeof(_value);

cb_status_t open_pkcs11_session(session_ctx_pkcs11_t *session_ctx);
void close_pkcs11_session(session_ctx_pkcs11_t *session_ctx);

#endif /* CB_PKCS11_PORT_COMMON_UTILS_H */

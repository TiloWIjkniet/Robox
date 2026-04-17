/*
 * Copyright 2025 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**
 * @file  common_utils.c
 * @brief Definitions for common utility functions for the PKCS11 layer.
 */

#include <dlfcn.h>
#include <stdlib.h>
#include <pthread.h>

#include "cb_osal.h"

#include "include/config.h"
#include "include/common_utils.h"

#define CK_FUNCTION_PTR(name) CK_DECLARE_FUNCTION_POINTER(CK_RV, name)

static CK_FUNCTION_LIST_3_0_PTR get_pkcs11_func_list(void *handle)
{
    CK_RV ret = CKR_GENERAL_ERROR;
    CK_VERSION version = { .major = 3, .minor = 1 };
    CK_FUNCTION_LIST_3_0_PTR pfunc = NULL_PTR;

    CK_FUNCTION_PTR(C_GetInterface)
    (CK_UTF8CHAR_PTR pInterfaceName, CK_VERSION_PTR pVersion,
     CK_INTERFACE_PTR_PTR ppInterface, CK_FLAGS flags) = NULL_PTR;
    CK_INTERFACE_PTR ifs = NULL_PTR;

    C_GetInterface = dlsym(handle, "C_GetInterface");
    if (C_GetInterface) {
        ret = C_GetInterface(NULL_PTR, &version, &ifs, 0);
        if (ret == CKR_OK)
            pfunc = ifs->pFunctionList;
    }

    return pfunc;
}

static CK_RV mutex_create(CK_VOID_PTR_PTR mutex)
{
    void *mutex_new = NULL;

    mutex_new = calloc(1, sizeof(pthread_mutex_t));
    if (!mutex_new)
        return CKR_HOST_MEMORY;

    if (pthread_mutex_init(mutex_new, PTHREAD_MUTEX_NORMAL)) {
        free(mutex_new);
        return CKR_GENERAL_ERROR;
    }

    *mutex = mutex_new;
    return CKR_OK;
}

static CK_RV mutex_destroy(CK_VOID_PTR mutex)
{
    if (!mutex)
        return CKR_MUTEX_BAD;

    if (pthread_mutex_destroy(mutex))
        return CKR_GENERAL_ERROR;

    free(mutex);

    return CKR_OK;
}

static CK_RV mutex_lock(CK_VOID_PTR mutex)
{
    CK_RV ret = CKR_OK;

    if (!mutex) {
        ret = CKR_MUTEX_BAD;
        goto end;
    }

    if (pthread_mutex_lock(mutex))
        ret = CKR_MUTEX_BAD;

end:
    return ret;
}

static CK_RV mutex_unlock(CK_VOID_PTR mutex)
{
    CK_RV ret = CKR_OK;

    if (!mutex) {
        ret = CKR_MUTEX_BAD;
        goto end;
    }

    if (pthread_mutex_unlock(mutex))
        ret = CKR_MUTEX_NOT_LOCKED;

end:
    return ret;
}

static int open_pkcs11_rw_session(CK_FUNCTION_LIST_3_0_PTR pfunc, CK_SLOT_ID p11_slot,
            CK_SESSION_HANDLE_PTR sess)
{
    CK_RV ret = CKR_OK;
    CK_ULONG nb_slots = 0;
    CK_SLOT_ID_PTR slots = NULL_PTR;
    const char *slot_label = NULL;
    CK_ULONG i = 0;

    ret = pfunc->C_GetSlotList(CK_TRUE, NULL_PTR, &nb_slots);
    if (ret != CKR_OK)
        goto end;

    slots = malloc(nb_slots * sizeof(CK_SLOT_ID));
    if (!slots) {
        ret = CKR_HOST_MEMORY;
        goto end;
    }

    ret = pfunc->C_GetSlotList(CK_TRUE, slots, &nb_slots);
    if (ret != CKR_OK)
        goto end;

    /* Check if requested slot is present */
    for (; i < nb_slots; i++) {
        if (slots[i] == p11_slot)
            break;
    }

    if (i >= nb_slots) {
        ret = CKR_SLOT_ID_INVALID;
        goto end;
    }

    ret = pfunc->C_OpenSession(p11_slot,
                   CKF_SERIAL_SESSION | CKF_RW_SESSION,
                   NULL_PTR, NULL_PTR, sess);

end:
    if (slots)
        free(slots);

    return ret;
}

cb_status_t open_pkcs11_session(session_ctx_pkcs11_t *session_ctx)
{
    CK_RV ret = CKR_OK;
    void *lib_hdl = NULL;
    CK_FUNCTION_LIST_3_0_PTR pfunc = NULL_PTR;
    CK_SESSION_HANDLE sess = 0;

    CK_C_INITIALIZE_ARGS init = { 0 };

    if (!session_ctx)
        return CB_STATUS_FAIL;

    lib_hdl = dlopen(DEFAULT_PKCS11_LIB, RTLD_LAZY);
    if (!lib_hdl)
        return CB_STATUS_FAIL;

    pfunc = get_pkcs11_func_list(lib_hdl);
    if (!pfunc) {
        ret = CB_STATUS_FAIL;
        goto func_list_fail;
    }

    init.CreateMutex = mutex_create;
    init.DestroyMutex = mutex_destroy;
    init.LockMutex = mutex_lock;
    init.UnlockMutex = mutex_unlock;

    ret = pfunc->C_Initialize(&init);
    if (ret != CKR_OK)
        goto init_fail;

    ret = open_pkcs11_rw_session(pfunc, 0, &sess);
    if (ret != CKR_OK)
        goto open_session_fail;

    ret = pfunc->C_Login(sess, CKU_USER, NULL_PTR, 0);
    if (ret != CKR_OK)
        (void)pfunc->C_CloseSession(sess);

open_session_fail:
    if (ret != CKR_OK)
        (void)pfunc->C_Finalize(NULL_PTR);

init_fail:
func_list_fail:
    if (ret != CKR_OK)
        dlclose(lib_hdl);

    if (ret == CKR_OK) {
        session_ctx->lib_hdl = lib_hdl;
        session_ctx->pfunc = pfunc;
        session_ctx->sess = sess;
    }

    return (ret == CKR_OK) ? CB_STATUS_SUCCESS : CB_STATUS_FAIL;
}

void close_pkcs11_session(session_ctx_pkcs11_t *session_ctx)
{
    if (!session_ctx)
        return;

    if (session_ctx->sess && session_ctx->pfunc)
        (void)session_ctx->pfunc->C_CloseSession(session_ctx->sess);

    if (session_ctx->pfunc)
        (void)session_ctx->pfunc->C_Finalize(NULL_PTR);

    if (session_ctx->lib_hdl)
        dlclose(session_ctx->lib_hdl);
}

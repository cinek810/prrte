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
 * Copyright (c) 2014-2019 Intel, Inc.  All rights reserved.
 * Copyright (c) 2015-2017 Research Organization for Information Science
 *                         and Technology (RIST). All rights reserved.
 * Copyright (c) 2018      IBM Corporation.  All rights reserved.
 * Copyright (c) 2020      Cisco Systems, Inc.  All rights reserved
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

/*
 * DSS Buffer Operations
 */
#include "prte_config.h"

#include "src/util/error.h"

#include "src/dss/dss_internal.h"


int prte_dss_unload(prte_buffer_t *buffer, void **payload,
                    int32_t *bytes_used)
{
    /* check that buffer is not null */
    if (!buffer) {
        return PRTE_ERR_BAD_PARAM;
    }

    /* were we given someplace to point to the payload */
    if (NULL == payload) {
        return PRTE_ERR_BAD_PARAM;
    }

    /* anything in the buffer - if not, nothing to do */
    if (NULL == buffer->base_ptr || 0 == buffer->bytes_used) {
        *payload = NULL;
        *bytes_used = 0;
        return PRTE_SUCCESS;
    }

    /* if nothing has been unpacked, we can pass the entire
     * region back and protect it - no need to copy. This is
     * an optimization */
    if (buffer->unpack_ptr == buffer->base_ptr) {
        *payload = buffer->base_ptr;
        *bytes_used = buffer->bytes_used;
        buffer->base_ptr = NULL;
        buffer->bytes_used = 0;
        goto cleanup;
    }

    /* okay, we have something to provide - pass it back */
    *bytes_used = buffer->bytes_used - (buffer->unpack_ptr - buffer->base_ptr);
    if (0 == (*bytes_used)) {
        *payload = NULL;
    } else {
        /* we cannot just set the pointer as it might be
         * partway in a malloc'd region */
        *payload = (void*)malloc(*bytes_used);
        memcpy(*payload, buffer->unpack_ptr, *bytes_used);
    }

  cleanup:
    /* All done - reset the buffer */
    PRTE_DESTRUCT(buffer);
    PRTE_CONSTRUCT(buffer, prte_buffer_t);
    return PRTE_SUCCESS;
}


int prte_dss_load(prte_buffer_t *buffer, void *payload,
                  int32_t bytes_used)
{
    /* check to see if the buffer has been initialized */
    if (NULL == buffer) {
        return PRTE_ERR_BAD_PARAM;
    }

    /* check if buffer already has payload - free it if so */
    PRTE_DESTRUCT(buffer);
    PRTE_CONSTRUCT(buffer, prte_buffer_t);

    /* if it's a NULL payload, just set things and return */
    if (NULL == payload) {
        return PRTE_SUCCESS;
    }

    /* populate the buffer */
    buffer->base_ptr = (char*)payload;

    /* set pack/unpack pointers */
    buffer->pack_ptr = ((char*)buffer->base_ptr) + bytes_used;
    buffer->unpack_ptr = buffer->base_ptr;

    /* set counts for size and space */
    buffer->bytes_allocated = buffer->bytes_used = bytes_used;

    /* All done */

    return PRTE_SUCCESS;
}


/* Copy the UNPACKED portion of a source buffer into a destination buffer
 * The complete contents of the src buffer are NOT copied - only that
 * portion that has not been previously unpacked is copied.
 */
int prte_dss_copy_payload(prte_buffer_t *dest, prte_buffer_t *src)
{
    char *dst_ptr;
    int32_t bytes_left;

    /* ensure we have valid source and destination */
    if (NULL == dest || NULL == src) {
        return PRTE_ERR_BAD_PARAM;
    }

    /* if the dest is already populated, check to ensure that both
     * source and dest are of the same buffer type
     */
    if (0 != dest->bytes_used) {
        if (dest->type != src->type) {
            return PRTE_ERR_BUFFER;
        }
    }

    /* either the dest was empty or the two types already match -
     * either way, just ensure the two types DO match
     */
    dest->type = src->type;

    /* compute how much of the src buffer remains unpacked
     * buffer->bytes_used is the total number of bytes in the buffer that
     * have been packed. However, we may have already unpacked some of
     * that data. We only want to unload what remains unpacked. This
     * means we have to look at how much of the buffer remains "used"
     * beyond the unpack_ptr
     */
    bytes_left = src->bytes_used - (src->unpack_ptr - src->base_ptr);

    /* if nothing is left, then nothing to do */
    if (0 == bytes_left) {
        return PRTE_SUCCESS;
    }

    /* add room to the dest for the src buffer's payload */
    if (NULL == (dst_ptr = prte_dss_buffer_extend(dest, bytes_left))) {
        return PRTE_ERR_OUT_OF_RESOURCE;
    }

    /* copy the src payload to the specified location in dest */
    memcpy(dst_ptr, src->unpack_ptr, bytes_left);

    /* adjust the dest buffer's bookkeeping */
    dest->bytes_used += bytes_left;
    dest->pack_ptr = ((char*)dest->pack_ptr) + bytes_left;

    return PRTE_SUCCESS;
}

int prte_value_load(prte_value_t *kv,
                    void *data, prte_data_type_t type)
{
    prte_byte_object_t *boptr;
    struct timeval *tv;

    kv->type = type;
    if (NULL == data && PRTE_STRING != type && PRTE_BYTE_OBJECT != type) {
        /* just set the fields to zero */
        memset(&kv->data, 0, sizeof(kv->data));
        return PRTE_SUCCESS;
    }

    switch (type) {
    case PRTE_BOOL:
        kv->data.flag = *(bool*)(data);
        break;
    case PRTE_BYTE:
        kv->data.byte = *(uint8_t*)(data);
        break;
    case PRTE_STRING:
        if (NULL != kv->data.string) {
            free(kv->data.string);
        }
        if (NULL != data) {
            kv->data.string = strdup( (const char *) data);
        } else {
            kv->data.string = NULL;
        }
        break;
    case PRTE_SIZE:
        kv->data.size = *(size_t*)(data);
        break;
    case PRTE_PID:
        kv->data.pid = *(pid_t*)(data);
        break;

    case PRTE_INT:
        kv->data.integer = *(int*)(data);
        break;
    case PRTE_INT8:
        kv->data.int8 = *(int8_t*)(data);
        break;
    case PRTE_INT16:
        kv->data.int16 = *(int16_t*)(data);
        break;
    case PRTE_INT32:
        kv->data.int32 = *(int32_t*)(data);
        break;
    case PRTE_INT64:
        kv->data.int64 = *(int64_t*)(data);
        break;

    case PRTE_UINT:
        kv->data.uint = *(unsigned int*)(data);
        break;
    case PRTE_UINT8:
        kv->data.uint8 = *(uint8_t*)(data);
        break;
    case PRTE_UINT16:
        kv->data.uint16 = *(uint16_t*)(data);
        break;
    case PRTE_UINT32:
        kv->data.uint32 = *(uint32_t*)data;
        break;
    case PRTE_UINT64:
        kv->data.uint64 = *(uint64_t*)(data);
        break;

    case PRTE_BYTE_OBJECT:
        if (NULL != kv->data.bo.bytes) {
            free(kv->data.bo.bytes);
        }
        boptr = (prte_byte_object_t*)data;
        if (NULL != boptr && NULL != boptr->bytes && 0 < boptr->size) {
            kv->data.bo.bytes = (uint8_t *) malloc(boptr->size);
            memcpy(kv->data.bo.bytes, boptr->bytes, boptr->size);
            kv->data.bo.size = boptr->size;
        } else {
            kv->data.bo.bytes = NULL;
            kv->data.bo.size = 0;
        }
        break;

    case PRTE_FLOAT:
        kv->data.fval = *(float*)(data);
        break;

    case PRTE_TIMEVAL:
        tv = (struct timeval*)data;
        kv->data.tv.tv_sec = tv->tv_sec;
        kv->data.tv.tv_usec = tv->tv_usec;
        break;

    case PRTE_PTR:
        kv->data.ptr = data;
        break;

    default:
        PRTE_ERROR_LOG(PRTE_ERR_NOT_SUPPORTED);
        return PRTE_ERR_NOT_SUPPORTED;
    }
    return PRTE_SUCCESS;
}

int prte_value_unload(prte_value_t *kv,
                      void **data, prte_data_type_t type)
{
    prte_byte_object_t *boptr;

    if (type != kv->type) {
        return PRTE_ERR_TYPE_MISMATCH;
    }
    if (NULL == data ||
        (PRTE_STRING != type && PRTE_BYTE_OBJECT != type && NULL == *data)) {
        PRTE_ERROR_LOG(PRTE_ERR_BAD_PARAM);
        return PRTE_ERR_BAD_PARAM;
    }

    switch (type) {
    case PRTE_BOOL:
        memcpy(*data, &kv->data.flag, sizeof(bool));
        break;
    case PRTE_BYTE:
        memcpy(*data, &kv->data.byte, sizeof(uint8_t));
        break;
    case PRTE_STRING:
        if (NULL != kv->data.string) {
            *data = strdup(kv->data.string);
        } else {
            *data = NULL;
        }
        break;
    case PRTE_SIZE:
        memcpy(*data, &kv->data.size, sizeof(size_t));
        break;
    case PRTE_PID:
        memcpy(*data, &kv->data.pid, sizeof(pid_t));
        break;

    case PRTE_INT:
        memcpy(*data, &kv->data.integer, sizeof(int));
        break;
    case PRTE_INT8:
        memcpy(*data, &kv->data.int8, sizeof(int8_t));
        break;
    case PRTE_INT16:
        memcpy(*data, &kv->data.int16, sizeof(int16_t));
        break;
    case PRTE_INT32:
        memcpy(*data, &kv->data.int32, sizeof(int32_t));
        break;
    case PRTE_INT64:
        memcpy(*data, &kv->data.int64, sizeof(int64_t));
        break;

    case PRTE_UINT:
        memcpy(*data, &kv->data.uint, sizeof(unsigned int));
        break;
    case PRTE_UINT8:
        memcpy(*data, &kv->data.uint8, 1);
        break;
    case PRTE_UINT16:
        memcpy(*data, &kv->data.uint16, 2);
        break;
    case PRTE_UINT32:
        memcpy(*data, &kv->data.uint32, 4);
        break;
    case PRTE_UINT64:
        memcpy(*data, &kv->data.uint64, 8);
        break;

    case PRTE_BYTE_OBJECT:
        boptr = (prte_byte_object_t*)malloc(sizeof(prte_byte_object_t));
        if (NULL != kv->data.bo.bytes && 0 < kv->data.bo.size) {
            boptr->bytes = (uint8_t *) malloc(kv->data.bo.size);
            memcpy(boptr->bytes, kv->data.bo.bytes, kv->data.bo.size);
            boptr->size = kv->data.bo.size;
        } else {
            boptr->bytes = NULL;
            boptr->size = 0;
        }
        *data = boptr;
        break;

    case PRTE_FLOAT:
        memcpy(*data, &kv->data.fval, sizeof(float));
        break;

    case PRTE_TIMEVAL:
        memcpy(*data, &kv->data.tv, sizeof(struct timeval));
        break;

    case PRTE_PTR:
        *data = kv->data.ptr;
        break;

    case PRTE_VPID:
        memcpy(*data, &kv->data.name.vpid, sizeof(prte_vpid_t));
        break;

    default:
        PRTE_ERROR_LOG(PRTE_ERR_NOT_SUPPORTED);
        return PRTE_ERR_NOT_SUPPORTED;
    }
    return PRTE_SUCCESS;
}

int prte_value_xfer(prte_value_t *dest,
                    prte_value_t *src)
{
    prte_byte_object_t *boptr;

    if (NULL != src->key) {
        dest->key = strdup(src->key);
    }
    dest->type = src->type;

    switch (src->type) {
    case PRTE_BOOL:
        dest->data.flag = src->data.flag;
        break;
    case PRTE_BYTE:
        dest->data.byte = src->data.byte;
        break;
    case PRTE_STRING:
        if (NULL != dest->data.string) {
            free(dest->data.string);
        }
        if (NULL != src->data.string) {
            dest->data.string = strdup(src->data.string);
        } else {
            dest->data.string = NULL;
        }
        break;
    case PRTE_SIZE:
        dest->data.size = src->data.size;
        break;
    case PRTE_PID:
        dest->data.pid = src->data.pid;
        break;

    case PRTE_INT:
        dest->data.integer = src->data.integer;
        break;
    case PRTE_INT8:
        dest->data.int8 = src->data.int8;
        break;
    case PRTE_INT16:
        dest->data.int16 = src->data.int16;
        break;
    case PRTE_INT32:
        dest->data.int32 = src->data.int32;
        break;
    case PRTE_INT64:
        dest->data.int64 = src->data.int64;
        break;

    case PRTE_UINT:
        dest->data.uint = src->data.uint;
        break;
    case PRTE_UINT8:
        dest->data.uint8 = src->data.uint8;
        break;
    case PRTE_UINT16:
        dest->data.uint16 = src->data.uint16;
        break;
    case PRTE_UINT32:
        dest->data.uint32 = src->data.uint32;
        break;
    case PRTE_UINT64:
        dest->data.uint64 = src->data.uint64;
        break;

    case PRTE_BYTE_OBJECT:
        if (NULL != dest->data.bo.bytes) {
            free(dest->data.bo.bytes);
        }
        boptr = &src->data.bo;
        if (NULL != boptr && NULL != boptr->bytes && 0 < boptr->size) {
            dest->data.bo.bytes = (uint8_t *) malloc(boptr->size);
            memcpy(dest->data.bo.bytes, boptr->bytes, boptr->size);
            dest->data.bo.size = boptr->size;
        } else {
            dest->data.bo.bytes = NULL;
            dest->data.bo.size = 0;
        }
        break;

    case PRTE_FLOAT:
        dest->data.fval = src->data.fval;
        break;

    case PRTE_TIMEVAL:
        dest->data.tv.tv_sec = src->data.tv.tv_sec;
        dest->data.tv.tv_usec = src->data.tv.tv_usec;
        break;

    case PRTE_PTR:
        dest->data.ptr = src->data.ptr;
        break;

    default:
        PRTE_ERROR_LOG(PRTE_ERR_NOT_SUPPORTED);
        return PRTE_ERR_NOT_SUPPORTED;
    }
    return PRTE_SUCCESS;
}

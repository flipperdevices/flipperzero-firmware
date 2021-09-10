#include "validation.h"
#include "malloc_wrappers.h"
#include <pb_common.h>
#include <assert.h>

void validate_static(pb_field_iter_t *iter)
{
    pb_size_t count = 1;
    pb_size_t i;
    bool truebool = true;
    bool falsebool = false;

    if (PB_HTYPE(iter->type) == PB_HTYPE_REPEATED && iter->pSize)
    {
        /* Array count must be between 0 and statically allocated size */
        count = *(pb_size_t*)iter->pSize;
        assert(count <= iter->array_size);
    }
    else if (PB_HTYPE(iter->type) == PB_HTYPE_OPTIONAL && iter->pSize)
    {
        /* Boolean has_ field must have a valid value */
        assert(memcmp(iter->pSize, &truebool, sizeof(bool)) == 0 ||
               memcmp(iter->pSize, &falsebool, sizeof(bool)) == 0);
    }
    else if (PB_HTYPE(iter->type) == PB_HTYPE_ONEOF)
    {
        if (*(pb_size_t*)iter->pSize != iter->tag)
        {
            /* Some different field in oneof */
            return;
        }
    }

    for (i = 0; i < count; i++)
    {
        void *pData = (char*)iter->pData + iter->data_size * i;

        if (PB_LTYPE(iter->type) == PB_LTYPE_STRING)
        {
            /* String length must be at most statically allocated size */
            assert(strlen(pData) + 1 <= iter->data_size);
        }
        else if (PB_LTYPE(iter->type) == PB_LTYPE_BYTES)
        {
            /* Bytes length must be at most statically allocated size */
            pb_bytes_array_t *bytes = pData;
            assert(PB_BYTES_ARRAY_T_ALLOCSIZE(bytes->size) <= iter->data_size);
        }
        else if (PB_LTYPE(iter->type) == PB_LTYPE_BOOL)
        {
            /* Bool fields must have valid value */
            assert(memcmp(pData, &truebool, sizeof(bool)) == 0 ||
                   memcmp(pData, &falsebool, sizeof(bool)) == 0);
        }
        else if (PB_LTYPE_IS_SUBMSG(iter->type))
        {
            validate_message(pData, 0, iter->submsg_desc);
        }
    }
}

void validate_pointer(pb_field_iter_t *iter)
{
    pb_size_t count = 1;
    pb_size_t i;
    bool truebool = true;
    bool falsebool = false;

    if (PB_HTYPE(iter->type) == PB_HTYPE_ONEOF)
    {
        if (*(pb_size_t*)iter->pSize != iter->tag)
        {
            /* Some different field in oneof */
            return;
        }
    }
    else if (!iter->pData)
    {
        /* Nothing allocated */
        if (PB_HTYPE(iter->type) == PB_HTYPE_REPEATED && iter->pSize != &iter->array_size)
        {
            assert(*(pb_size_t*)iter->pSize == 0);
        }
        return;
    }

    if (PB_HTYPE(iter->type) == PB_HTYPE_REPEATED)
    {
        /* Check that enough memory has been allocated for array */
        size_t allocated_size = get_allocation_size(iter->pData);
        count = *(pb_size_t*)iter->pSize;
        assert(allocated_size >= count * iter->data_size);
    }
    else if (PB_LTYPE(iter->type) != PB_LTYPE_STRING && PB_LTYPE(iter->type) != PB_LTYPE_BYTES)
    {
        size_t allocated_size = get_allocation_size(iter->pData);
        assert(allocated_size >= iter->data_size);
    }

    for (i = 0; i < count; i++)
    {
        void *pData = (char*)iter->pData + iter->data_size * i;

        if (PB_LTYPE(iter->type) == PB_LTYPE_STRING)
        {
            /* Check that enough memory is allocated for string and that
               the string is properly terminated. */
            const char *str = pData;

            if (PB_HTYPE(iter->type) == PB_HTYPE_REPEATED)
            {
                /* String arrays are stored as array of pointers */
                str = ((const char**)iter->pData)[i];
            }

            assert(strlen(str) + 1 <= get_allocation_size(str));
        }
        else if (PB_LTYPE(iter->type) == PB_LTYPE_BYTES)
        {
            /* Bytes length must be at most statically allocated size */
            const pb_bytes_array_t *bytes = pData;

            if (PB_HTYPE(iter->type) == PB_HTYPE_REPEATED)
            {
                /* Bytes arrays are stored as array of pointers */
                bytes = ((const pb_bytes_array_t**)iter->pData)[i];
            }

            assert(PB_BYTES_ARRAY_T_ALLOCSIZE(bytes->size) <= get_allocation_size(bytes));
        }
        else if (PB_LTYPE(iter->type) == PB_LTYPE_BOOL)
        {
            /* Bool fields must have valid value */
            assert(memcmp(pData, &truebool, sizeof(bool)) == 0 ||
                   memcmp(pData, &falsebool, sizeof(bool)) == 0);
        }
        else if (PB_LTYPE_IS_SUBMSG(iter->type))
        {
            validate_message(pData, 0, iter->submsg_desc);
        }
    }
}

void validate_message(const void *msg, size_t structsize, const pb_msgdesc_t *msgtype)
{
    pb_field_iter_t iter;

    if (pb_field_iter_begin_const(&iter, msgtype, msg))
    {
        do
        {
            if (PB_ATYPE(iter.type) == PB_ATYPE_STATIC)
            {
                validate_static(&iter);
            }
            else if (PB_ATYPE(iter.type) == PB_ATYPE_POINTER)
            {
                validate_pointer(&iter);
            }
        } while (pb_field_iter_next(&iter));
    }
}


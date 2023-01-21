/* Copyright (C) 2022-2023 Salvatore Sanfilippo -- All Rights Reserved
 * See the LICENSE file for information about the license.
 *
 * Protocol fields implementation. */

#include "app.h"

/* Create a new field of the specified type. Without populating its
 * type-specific value. */
static ProtoViewField* field_new(ProtoViewFieldType type, const char* name) {
    ProtoViewField* f = malloc(sizeof(*f));
    f->type = type;
    f->name = strdup(name);
    return f;
}

/* Free a field an associated data. */
static void field_free(ProtoViewField* f) {
    free(f->name);
    switch(f->type) {
    case FieldTypeStr:
        free(f->str);
        break;
    case FieldTypeBytes:
        free(f->bytes);
        break;
    default:
        break; // Nothing to free for other types.
    }
    free(f);
}

/* Free a field set and its contained fields. */
void fieldset_free(ProtoViewFieldSet* fs) {
    for(uint32_t j = 0; j < fs->numfields; j++) field_free(fs->fields[j]);
    free(fs->fields);
    free(fs);
}

/* Allocate and init an empty field set. */
ProtoViewFieldSet* fieldset_new(void) {
    ProtoViewFieldSet* fs = malloc(sizeof(*fs));
    fs->numfields = 0;
    fs->fields = NULL;
    return fs;
}

/* Append an already allocated field at the end of the specified field set. */
static void fieldset_add_field(ProtoViewFieldSet* fs, ProtoViewField* field) {
    fs->numfields++;
    fs->fields = realloc(fs->fields, sizeof(ProtoViewField*) * fs->numfields);
    fs->fields[fs->numfields - 1] = field;
}

/* Allocate and append an integer field. */
void fieldset_add_int(ProtoViewFieldSet* fs, const char* name, int64_t val, uint8_t bits) {
    ProtoViewField* f = field_new(FieldTypeSignedInt, name);
    f->value = val;
    f->len = bits;
    fieldset_add_field(fs, f);
}

/* Allocate and append an unsigned field. */
void fieldset_add_uint(ProtoViewFieldSet* fs, const char* name, uint64_t uval, uint8_t bits) {
    ProtoViewField* f = field_new(FieldTypeUnsignedInt, name);
    f->uvalue = uval;
    f->len = bits;
    fieldset_add_field(fs, f);
}

/* Allocate and append a hex field. This is an unsigned number but
 * with an hex representation. */
void fieldset_add_hex(ProtoViewFieldSet* fs, const char* name, uint64_t uval, uint8_t bits) {
    ProtoViewField* f = field_new(FieldTypeHex, name);
    f->uvalue = uval;
    f->len = bits;
    fieldset_add_field(fs, f);
}

/* Allocate and append a bin field. This is an unsigned number but
 * with a binary representation. */
void fieldset_add_bin(ProtoViewFieldSet* fs, const char* name, uint64_t uval, uint8_t bits) {
    ProtoViewField* f = field_new(FieldTypeBinary, name);
    f->uvalue = uval;
    f->len = bits;
    fieldset_add_field(fs, f);
}

/* Allocate and append a string field. */
void fieldset_add_str(ProtoViewFieldSet* fs, const char* name, const char* s) {
    ProtoViewField* f = field_new(FieldTypeStr, name);
    f->str = strdup(s);
    f->len = strlen(s);
    fieldset_add_field(fs, f);
}

/* Allocate and append a bytes field. Note that 'count' is specified in
 * nibbles (bytes*2). */
void fieldset_add_bytes(
    ProtoViewFieldSet* fs,
    const char* name,
    const uint8_t* bytes,
    uint32_t count_nibbles) {
    ProtoViewField* f = field_new(FieldTypeBytes, name);
    f->bytes = malloc(count_nibbles / 2);
    memcpy(f->bytes, bytes, count_nibbles / 2);
    f->len = count_nibbles;
    fieldset_add_field(fs, f);
}

/* Allocate and append a float field. */
void fieldset_add_float(
    ProtoViewFieldSet* fs,
    const char* name,
    float val,
    uint32_t digits_after_dot) {
    ProtoViewField* f = field_new(FieldTypeFloat, name);
    f->fvalue = val;
    f->len = digits_after_dot;
    fieldset_add_field(fs, f);
}

// SPDX-License-Identifier: GPL-3.0-or-later
//
// VB Lab Migration Assistant for Flipper Zero
// Copyright (C) 2022  cyanic
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "vb_tag.h"

#define VB_NAME_VBDM "VB Digital Monster"
#define VB_NAME_VBV "VB Digivice -V-"
#define VB_NAME_VBC "VB Characters"
#define VB_NAME_VH "Vital Hero"

#define VB_NAME_VBDM_SHORT "VBDM"
#define VB_NAME_VBV_SHORT "VBV"
#define VB_NAME_VBC_SHORT "VBC"
#define VB_NAME_VH_SHORT "VH"

static const VbTagProduct vb_tag_valid_products[] = {
    {.item_id = 0x0200,
     .item_no = 0x0100,
     .name = VB_NAME_VBDM,
     .short_name = VB_NAME_VBDM_SHORT,
     .type = VbTagTypeVBDM},
    {.item_id = 0x0200,
     .item_no = 0x0200,
     .name = VB_NAME_VBDM,
     .short_name = VB_NAME_VBDM_SHORT,
     .type = VbTagTypeVBDM},
    {.item_id = 0x0200,
     .item_no = 0x0300,
     .name = VB_NAME_VBDM,
     .short_name = VB_NAME_VBDM_SHORT,
     .type = VbTagTypeVBDM},
    {.item_id = 0x0200,
     .item_no = 0x0400,
     .name = VB_NAME_VBV,
     .short_name = VB_NAME_VBV_SHORT,
     .type = VbTagTypeVBV},
    {.item_id = 0x0200,
     .item_no = 0x0500,
     .name = VB_NAME_VBV,
     .short_name = VB_NAME_VBV_SHORT,
     .type = VbTagTypeVBV},
    {.item_id = 0x0200,
     .item_no = 0x0600,
     .name = VB_NAME_VH,
     .short_name = VB_NAME_VH_SHORT,
     .type = VbTagTypeVH},
    {.item_id = 0x0300,
     .item_no = 0x0100,
     .name = VB_NAME_VBC,
     .short_name = VB_NAME_VBC_SHORT,
     .type = VbTagTypeVBC},
};

static const char* vb_tag_type_names[] = {
    "Unknown",
    VB_NAME_VBDM_SHORT,
    VB_NAME_VBV_SHORT,
    VB_NAME_VBC_SHORT,
    VB_NAME_VH_SHORT,
};

BantBlock* vb_tag_get_bant_block(NfcDeviceData* dev) {
    return (BantBlock*)&dev->mf_ul_data.data[16];
}

const VbTagProduct* vb_tag_find_product(const BantBlock* bant) {
    for(size_t i = 0; i < COUNT_OF(vb_tag_valid_products); ++i) {
        const VbTagProduct* product = &vb_tag_valid_products[i];
        if(bant->item_id == product->item_id && bant->item_no == product->item_no) return product;
    }

    return NULL;
}

bool vb_tag_validate_product(NfcDeviceData* dev) {
    // Must be NTAG I2C Plus 1k
    if(dev->protocol != NfcDeviceProtocolMifareUl) return false;
    if(dev->mf_ul_data.type != MfUltralightTypeNTAGI2CPlus1K) return false;
    // Must match one of the known product IDs
    BantBlock* bant = vb_tag_get_bant_block(dev);
    if(bant->magic != BANT_MAGIC) return false;
    return vb_tag_find_product(bant) != NULL;
}

VbTagStatus vb_tag_get_status(const BantBlock* bant) {
    return bant->status;
}

void vb_tag_set_status(BantBlock* bant, VbTagStatus status) {
    bant->status = status;
}

VbTagOperation vb_tag_get_operation(const BantBlock* bant) {
    return bant->operation;
}

void vb_tag_set_operation(BantBlock* bant, VbTagOperation operation) {
    bant->operation = operation;
}

const VbTagProduct* vb_tag_get_default_product(VbTagType type) {
    // IMPORTANT: Update when vb_tag_valid_products changes
    switch(type) {
    case VbTagTypeVBDM:
        return &vb_tag_valid_products[2];
    case VbTagTypeVBV:
        return &vb_tag_valid_products[4];
    case VbTagTypeVBC:
        return &vb_tag_valid_products[6];
    case VbTagTypeVH:
        return &vb_tag_valid_products[5];

    default:
        return NULL;
    }
}

void vb_tag_set_item_id_no(BantBlock* bant, const VbTagProduct* product) {
    bant->item_id = product->item_id;
    bant->item_no = product->item_no;
}

const char* vb_tag_get_tag_type_name(VbTagType type) {
    if(type < VbTagTypeMax) {
        return vb_tag_type_names[type];
    } else {
        return NULL;
    }
}

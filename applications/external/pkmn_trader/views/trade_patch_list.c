#include "../pokemon_app.h"
#include "trade_patch_list.h"

struct patch_list* plist_alloc(void) {
    struct patch_list* plist = NULL;

    plist = malloc(sizeof(struct patch_list));
    plist->index = 0;
    plist->next = NULL;
    return plist;
}

void plist_append(struct patch_list* plist, uint8_t index) {
    furi_assert(plist);

    for(;;) {
        if(plist->next == NULL) break;
        plist = plist->next;
    }
    plist->index = index;
    plist->next = plist_alloc();
}

void plist_free(struct patch_list* plist) {
    struct patch_list* plist_next = NULL;

    while(plist != NULL) {
        plist_next = plist->next;
        free(plist);
        plist = plist_next;
    }
}

/* Returns the index value at offset member of the list. If offset is beyond
 * the length of the allocated list, it will just return 0.
 */
uint8_t plist_index_get(struct patch_list* plist, int offset) {
    furi_assert(plist);
    int i;

    for(i = 0; i < offset; i++) {
        if(plist->next == NULL) break;
        plist = plist->next;
    }

    return plist->index;
}

void plist_create(struct patch_list** plist, TradeBlock* trade_block) {
    furi_assert(trade_block);
    uint8_t* trade_party_flat = (uint8_t*)trade_block->party;
    int i;

    /* XXX: HACK: Set up our patch list now. Note that, this will cause weird
     * problems if a pokemon with a patched index is traded to the flipper with
     * a pokemon without a patched index, or the other way around. Need to implement
     * a way to update the patch list after we get traded a pokemon.
     *
     * Can maybe use the furi timer queue callback thing
     */

    /* If plist is non-NULL that means its already been created. Tear it down
     * first.
     */
    if(*plist != NULL) {
        plist_free(*plist);
        *plist = NULL;
    }

    *plist = plist_alloc();
    /* NOTE: 264 magic number is the length of the party block, 44 * 6 */
    /* The first half of the patch list covers offsets 0x00 - 0xfb, which
     * is expressed as 0x01 - 0xfc. An 0xFF byte is added to signify the
     * end of the first part. The second half of the patch list covers
     * offsets 0xfc - 0x107. Which is expressed as 0x01 - 0xc. A 0xFF byte
     * is added to signify the end of the second part/
     */
    for(i = 0; i < 264; i++) {
        if(i == 0xFC) plist_append(*plist, 0xFF);

        if(trade_party_flat[i] == 0xFE) {
            plist_append(*plist, (i % 0xfc) + 1);
            trade_party_flat[i] = 0xFF;
        }
    }
    plist_append(*plist, 0xFF);
}

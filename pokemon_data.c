#include <pokemon_icons.h>

#include <storage/storage.h>

#include "pokemon_data.h"
#include "pokemon_data_i.h"
#include "pokemon_app.h"
#include "pokemon_char_encode.h"

#include "pokemon_table.h"

#include <named_list.h>
#include <item_nl.h>
#include <stat_nl.h>
#include <type_nl.h>
#include <move_nl.h>

#include <missingno_i.h>

#define RECALC_NONE 0x00
#define RECALC_EXP 0x01
#define RECALC_EVS 0x02
#define RECALC_IVS 0x04
#define RECALC_STATS 0x08
#define RECALC_NICKNAME 0x10
#define RECALC_MOVES 0x20
#define RECALC_TYPES 0x40
#define RECALC_ALL 0xFF

#define FXBM_SPRITE_SIZE 404 // Each 56x56 sprite

/* Text lookups to make debug output cleaner and easier to parse as a human */
static char* stat_text_get(DataStat stat) {
    switch(stat) {
    case STAT_ATK:
        return "ATK";
    case STAT_DEF:
        return "DEF";
    case STAT_SPD:
        return "SPD";
    case STAT_SPC:
        return "SPC";
    case STAT_SPC_ATK:
        return "SPC_ATK";
    case STAT_SPC_DEF:
        return "SPC_DEF";
    case STAT_HP:
        return "HP";
    case STAT_TYPE:
        return "Type";
    case STAT_MOVE:
        return "Move";
    case STAT_ATK_EV:
        return "ATK_EV";
    case STAT_DEF_EV:
        return "DEF_EV";
    case STAT_SPD_EV:
        return "SPD_EV";
    case STAT_SPC_ATK_EV:
    case STAT_SPC_DEF_EV:
    case STAT_SPC_EV:
        return "SPC_EV";
    case STAT_HP_EV:
        return "HP_EV";
    case STAT_IV:
        return "IV";
    case STAT_ATK_IV:
        return "ATK_IV";
    case STAT_DEF_IV:
        return "DEF_IV";
    case STAT_SPD_IV:
        return "SPD_IV";
    case STAT_SPC_ATK_IV:
    case STAT_SPC_DEF_IV:
    case STAT_SPC_IV:
        return "SPC_IV";
    case STAT_HP_IV:
        return "HP_IV";
    case STAT_LEVEL:
        return "Lvl.";
    case STAT_INDEX:
        return "Idx.";
    case STAT_NUM:
        return "Num.";
    case STAT_CONDITION:
        return "Cond.";
    case STAT_NICKNAME:
        return "Nick.";
    case STAT_OT_NAME:
        return "OT Name";
    case STAT_OT_ID:
        return "OT ID";
    case STAT_TRAINER_NAME:
        return "Trainer Name";
    case STAT_SEL:
        return "EV/IV Sel."; // which EV/IV calc to use
    case STAT_EXP:
        return "Exp.";
    case STAT_HELD_ITEM:
        return "Held Item";
    case STAT_POKERUS:
        return "Pokerus";
    default:
        return "UNKNOWN STAT";
    }
}

/* Allocates a chunk of memory for the trade data block and sets up some
 * default values.
 */
PokemonData* pokemon_data_alloc(uint8_t gen) {
    PokemonData* pdata;

    pdata = malloc(sizeof(PokemonData));
    pdata->gen = gen;

    /* Set up lists */
    pdata->move_list = move_list;
    pdata->type_list = type_list;
    pdata->stat_list = stat_list;
    pdata->item_list = item_list;
    pdata->pokemon_table = table_pointer_get();

    pdata->storage = furi_record_open(RECORD_STORAGE);
    pdata->asset_path = furi_string_alloc_set(APP_ASSETS_PATH());
    storage_common_resolve_path_and_ensure_app_directory(pdata->storage, pdata->asset_path);

    switch(gen) {
    case GEN_I:
        /* Allocate trade block and set its size for the trade view to use */
        pdata->trade_block_sz = sizeof(TradeBlockGenI);
        pdata->party_sz = sizeof(PokemonPartyGenI) * 6;
        pdata->trade_block = malloc(pdata->trade_block_sz);

        /* The party_members element needs to be 0xff for unused */
        memset(
            ((TradeBlockGenI*)pdata->trade_block)->party_members,
            0xFF,
            sizeof(((TradeBlockGenI*)pdata->trade_block)->party_members));

        pdata->party = ((TradeBlockGenI*)pdata->trade_block)->party;

        /* Set party count to 1 */
        ((TradeBlockGenI*)pdata->trade_block)->party_cnt = 1;

        /* Set the max pokedex number, 0 indexed */
        pdata->dex_max = 150;
        break;
    case GEN_II:
        /* Allocate trade block and set its size for the trade view to use */
        pdata->trade_block_sz = sizeof(TradeBlockGenII);
        pdata->party_sz = sizeof(PokemonPartyGenII) * 6;
        pdata->trade_block = malloc(pdata->trade_block_sz);

        /* The party_members element needs to be 0xff for unused */
        memset(
            ((TradeBlockGenII*)pdata->trade_block)->party_members,
            0xFF,
            sizeof(((TradeBlockGenII*)pdata->trade_block)->party_members));

        pdata->party = ((TradeBlockGenII*)pdata->trade_block)->party;

        /* Set party count to 1 */
        ((TradeBlockGenII*)pdata->trade_block)->party_cnt = 1;

        /* Set the max pokedex number, 0 indexed */
        pdata->dex_max = 250;
        break;
    default:
        furi_crash("Invalid Gen");
        break;
    }

    /* Trainer/OT name, not to exceed 7 characters! */
    pokemon_name_set(pdata, STAT_TRAINER_NAME, "Flipper");
    pokemon_name_set(pdata, STAT_OT_NAME, "Flipper");

    /* OT trainer ID# */
    pokemon_stat_set(pdata, STAT_OT_ID, NONE, 42069);

    /* Notes:
     * Move pp isn't explicitly set up, should be fine
     * Catch/held isn't explicitly set up, should be okay for only Gen I support now
     * Status condition isn't explicity let up, would you ever want to?
     */

    /* Set up initial pokemon and level */
    /* This causes all other stats to be recalculated */
    pokemon_stat_set(pdata, STAT_NUM, NONE, 0); // First Pokemon
    pokemon_stat_set(pdata, STAT_LEVEL, NONE, 2); // Minimum level of 2

    return pdata;
}

void pokemon_data_free(PokemonData* pdata) {
    furi_record_close(RECORD_STORAGE);
    free(pdata->trade_block);
    if(pdata->bitmap && pdata->bitmap_num != 0) free(pdata->bitmap);
    furi_string_free(pdata->asset_path);
    free(pdata);
}

/* Recalculate values and stats based on their dependencies.
 * The order of the if statements are in order of dependence from
 * depending on no other value, to dpeneding on multiple other values.
 *
 * level:	depends on:	none
 * iv: 		depends on: 	none (only what the EV/IV general setting is, which recalculates EV/IV at time of set)
 * ev:		depends on: 	level (sometimes)
 * exp:		depends on:	level, index
 * moves:	depends on:	index
 * types:	depends on:	index
 * nickname:	depends on:	index
 * atk/def/etc:	depends on:	level, iv, ev, index
 */
void pokemon_recalculate(PokemonData* pdata, uint8_t recalc) {
    furi_assert(pdata);
    int i;

    if(recalc == RECALC_NONE) return;

    /* Ordered in order of priority for calculating other stats */
    if(recalc & RECALC_NICKNAME) pokemon_default_nickname_set(NULL, pdata, 0);

    if(recalc & RECALC_MOVES) {
        for(i = MOVE_0; i <= MOVE_3; i++) {
            pokemon_stat_set(
                pdata,
                STAT_MOVE,
                i,
                table_stat_base_get(
                    pdata->pokemon_table,
                    pokemon_stat_get(pdata, STAT_NUM, NONE),
                    STAT_BASE_MOVE,
                    i));
        }
    }

    if(recalc & RECALC_TYPES) {
        for(i = TYPE_0; i <= TYPE_1; i++) {
            pokemon_stat_set(
                pdata,
                STAT_TYPE,
                i,
                table_stat_base_get(
                    pdata->pokemon_table,
                    pokemon_stat_get(pdata, STAT_NUM, NONE),
                    STAT_BASE_TYPE,
                    i));
        }
    }

    if(recalc & RECALC_EXP) pokemon_exp_calc(pdata);

    if(recalc & RECALC_EVS) pokemon_stat_ev_calc(pdata, pdata->stat_sel);

    /* This just rerolls the IVs, nothing really to calculate */
    if(recalc & RECALC_IVS) pokemon_stat_iv_calc(pdata, pdata->stat_sel);

    /* Note: This will still end up calculating spc_def on gen i pokemon.
     * However, the way the accessors are set up the calculated value will
     * never be written anywhere. This is just wasted CPU time.
     */
    if(recalc & RECALC_STATS) {
        for(i = STAT; i < STAT_END; i++) {
            pokemon_stat_calc(pdata, i);
        }
    }
}

/* This needs to convert to encoded characters */
void pokemon_name_set(PokemonData* pdata, DataStat stat, char* name) {
    furi_assert(pdata);
    size_t len;
    uint8_t gen = pdata->gen;
    uint8_t* ptr = NULL;

    switch(stat) {
    case STAT_NICKNAME:
        if(gen == GEN_I) ptr = ((TradeBlockGenI*)pdata->trade_block)->nickname[0].str;
        if(gen == GEN_II) ptr = ((TradeBlockGenII*)pdata->trade_block)->nickname[0].str;
        len = 10;
        break;
    case STAT_OT_NAME:
        if(gen == GEN_I) ptr = ((TradeBlockGenI*)pdata->trade_block)->ot_name[0].str;
        if(gen == GEN_II) ptr = ((TradeBlockGenII*)pdata->trade_block)->ot_name[0].str;
        len = 7;
        break;
    case STAT_TRAINER_NAME:
        if(gen == GEN_I) ptr = ((TradeBlockGenI*)pdata->trade_block)->trainer_name.str;
        if(gen == GEN_II) ptr = ((TradeBlockGenII*)pdata->trade_block)->trainer_name.str;
        len = 7;
        break;
    default:
        furi_crash("name");
        break;
    }

    /* Clear the buffer with TERM character */
    memset(ptr, TERM_, LEN_NAME_BUF);

    /* Set the encoded name in the buffer */
    pokemon_str_to_encoded_array(ptr, name, len);
    FURI_LOG_D(TAG, "[data] %s name set to %s", stat_text_get(stat), name);
}

void pokemon_name_get(PokemonData* pdata, DataStat stat, char* dest, size_t len) {
    furi_assert(pdata);
    uint8_t* ptr = NULL;
    uint8_t gen = pdata->gen;

    switch(stat) {
    case STAT_NICKNAME:
        if(gen == GEN_I) ptr = ((TradeBlockGenI*)pdata->trade_block)->nickname[0].str;
        if(gen == GEN_II) ptr = ((TradeBlockGenII*)pdata->trade_block)->nickname[0].str;
        break;
    case STAT_OT_NAME:
        if(gen == GEN_I) ptr = ((TradeBlockGenI*)pdata->trade_block)->ot_name[0].str;
        if(gen == GEN_II) ptr = ((TradeBlockGenII*)pdata->trade_block)->ot_name[0].str;
        break;
    default:
        furi_crash("name_get invalid");
        break;
    }

    pokemon_encoded_array_to_str(dest, ptr, len);
}

/* If dest is not NULL, a copy of the default name is written to it as well */
void pokemon_default_nickname_set(char* dest, PokemonData* pdata, size_t n) {
    furi_assert(pdata);
    unsigned int i;
    char buf[LEN_NAME_BUF];

    /* First, get the default name */
    strncpy(
        buf,
        table_stat_name_get(pdata->pokemon_table, pokemon_stat_get(pdata, STAT_NUM, NONE)),
        sizeof(buf));

    /* Next, walk through and toupper() each character */
    for(i = 0; i < sizeof(buf); i++) {
        buf[i] = toupper(buf[i]);
    }

    pokemon_name_set(pdata, STAT_NICKNAME, buf);
    FURI_LOG_D(TAG, "[data] Set default nickname");

    if(dest != NULL) {
        strncpy(dest, buf, n);
    }
}

/* Each sprite 56x56 is 404 bytes long */
uint8_t* pokemon_icon_get(PokemonData* pdata, int num) {
    furi_assert(pdata);
    File* file;
    FuriString* path;
    uint32_t size;
    bool is_error = true;

    if(pdata->bitmap_num != num) {
        if(pdata->bitmap) {
            free(pdata->bitmap);
            pdata->bitmap = NULL;
        }

        file = storage_file_alloc(pdata->storage);
        path = furi_string_alloc_set(pdata->asset_path);
        furi_string_cat_printf(path, "all_sprites.fxbm");

        if(storage_file_open(file, furi_string_get_cstr(path), FSAM_READ, FSOM_OPEN_EXISTING)) {
            storage_file_seek(file, (num - 1) * FXBM_SPRITE_SIZE, true);
            if(storage_file_read(file, &size, sizeof(size)) == sizeof(size)) {
                pdata->bitmap = malloc(size);
                if(storage_file_read(file, pdata->bitmap, size) ==
                   FXBM_SPRITE_SIZE - sizeof(size)) {
                    FURI_LOG_D(TAG, "Opened file \'%s\'", furi_string_get_cstr(path));
                    is_error = false;
                } else {
                    free(pdata->bitmap);
                }
            }
        }

        if(is_error) {
            FURI_LOG_E(
                TAG, "Failed to open \'%s\' or access sprite data", furi_string_get_cstr(path));
            pdata->bitmap = (struct fxbm_sprite*)((uint8_t*)(__000_fxbm) + sizeof(size));
            num = 0;
        }

        storage_file_free(file);
        furi_string_free(path);

        pdata->bitmap_num = num;
    }

    return (uint8_t*)pdata->bitmap;
}

uint16_t pokemon_stat_get(PokemonData* pdata, DataStat stat, DataStatSub which) {
    furi_assert(pdata);
    void* party = pdata->party;
    int gen = pdata->gen;
    uint16_t val = 0;
    uint8_t hp_iv = 0;

    switch(stat) {
    case STAT_ATK:
        if(gen == GEN_I) val = ((PokemonPartyGenI*)party)->atk;
        if(gen == GEN_II) val = ((PokemonPartyGenII*)party)->atk;
        break;
    case STAT_DEF:
        if(gen == GEN_I) val = ((PokemonPartyGenI*)party)->def;
        if(gen == GEN_II) val = ((PokemonPartyGenII*)party)->def;
        break;
    case STAT_SPD:
        if(gen == GEN_I) val = ((PokemonPartyGenI*)party)->spd;
        if(gen == GEN_II) val = ((PokemonPartyGenII*)party)->spd;
        break;
    case STAT_SPC:
        if(gen == GEN_I) val = ((PokemonPartyGenI*)party)->spc;
        break;
    case STAT_SPC_ATK:
        if(gen == GEN_II) val = ((PokemonPartyGenII*)party)->spc_atk;
        break;
    case STAT_SPC_DEF:
        if(gen == GEN_II) val = ((PokemonPartyGenII*)party)->spc_def;
        break;
    case STAT_HP:
        if(gen == GEN_I) val = ((PokemonPartyGenI*)party)->hp;
        if(gen == GEN_II) val = ((PokemonPartyGenII*)party)->hp;
        break;
    case STAT_ATK_EV:
        if(gen == GEN_I) val = ((PokemonPartyGenI*)party)->atk_ev;
        if(gen == GEN_II) val = ((PokemonPartyGenII*)party)->atk_ev;
        break;
    case STAT_DEF_EV:
        if(gen == GEN_I) val = ((PokemonPartyGenI*)party)->def_ev;
        if(gen == GEN_II) val = ((PokemonPartyGenII*)party)->def_ev;
        break;
    case STAT_SPD_EV:
        if(gen == GEN_I) val = ((PokemonPartyGenI*)party)->spd_ev;
        if(gen == GEN_II) val = ((PokemonPartyGenII*)party)->spd_ev;
        break;
    case STAT_SPC_EV:
    case STAT_SPC_ATK_EV:
    case STAT_SPC_DEF_EV:
        if(gen == GEN_I) val = ((PokemonPartyGenI*)party)->spc_ev;
        if(gen == GEN_II) val = ((PokemonPartyGenII*)party)->spc_ev;
        break;
    case STAT_HP_EV:
        if(gen == GEN_I) val = ((PokemonPartyGenI*)party)->hp_ev;
        if(gen == GEN_II) val = ((PokemonPartyGenII*)party)->hp_ev;
        break;
    case STAT_IV:
        if(gen == GEN_I) val = ((PokemonPartyGenI*)party)->iv;
        if(gen == GEN_II) val = ((PokemonPartyGenII*)party)->iv;
        break;
    /* The IVs in GB byte order, are always:
     * atk, def, spd, spc
     * Like every other 16 bit quantity that the Flipper acts on, we need to
     * bytw swap them normally. However, the below accessors for individual
     * IV nibbles directly pull from the data structures which will always
     * be in GB endianness.
     */
    case STAT_SPD_IV:
        if(gen == GEN_I) return (((PokemonPartyGenI*)party)->iv >> 12) & 0x0F;
        if(gen == GEN_II) return (((PokemonPartyGenII*)party)->iv >> 12) & 0x0F;
        break;
    /* In order to line up all of the dynamic stat accessors used as part of the
     * stat calculation loop, we need to overload the SPC IV accessor to allow
     * accessing SPC, SPC_ATK, and SPC_DEF. Note that only SPC exists, the ATK
     * and DEF are the overloaded values. This is so when, for example, gen i
     * calculates its SPC value, or gen ii calculates is SPC_DEF value, it will
     * always grab the same IV nibble.
     */
    case STAT_SPC_IV:
    case STAT_SPC_ATK_IV:
    case STAT_SPC_DEF_IV:
        if(gen == GEN_I) return (((PokemonPartyGenI*)party)->iv >> 8) & 0x0F;
        if(gen == GEN_II) return (((PokemonPartyGenII*)party)->iv >> 8) & 0x0F;
        break;
    case STAT_ATK_IV:
        if(gen == GEN_I) return (((PokemonPartyGenI*)party)->iv >> 4) & 0x0F;
        if(gen == GEN_II) return (((PokemonPartyGenII*)party)->iv >> 4) & 0x0F;
        break;
    case STAT_DEF_IV:
        if(gen == GEN_I) return ((PokemonPartyGenI*)party)->iv & 0x0F;
        if(gen == GEN_II) return ((PokemonPartyGenII*)party)->iv & 0x0F;
        break;
    case STAT_HP_IV:
        /* NOTE:
	 * HP IV is calculated as the LSB of each other IV, assembled in the
	 * same bit order down to a single nibble.
	 */
        if(gen == GEN_I) val = (((PokemonPartyGenI*)party)->iv);
        if(gen == GEN_II) val = (((PokemonPartyGenII*)party)->iv);
        /* NOTE:
	 * As noted above, we store the IV in the trade struct in the byte order
	 * of the gameboy which is swapped from the Flipper's byte order.
	 */
        hp_iv |= ((val & 0x0010) >> 1); // ATK IV, MSbit of the hp_iv nibble
        hp_iv |= ((val & 0x0001) << 2); // DEF IV, right of ATK IV in hp_iv nibble
        hp_iv |= ((val & 0x1000) >> 11); // SPD IV, right of DEF IV in hp_iv nibble
        hp_iv |= ((val & 0x0100) >> 8); // SPC IV, right of SPD IV in hp_iv nibble
        return hp_iv;
        break;
    case STAT_LEVEL:
        if(gen == GEN_I) return ((PokemonPartyGenI*)party)->level;
        if(gen == GEN_II) return ((PokemonPartyGenII*)party)->level;
        break;
    case STAT_INDEX:
        if(gen == GEN_I) return ((PokemonPartyGenI*)party)->index;
        if(gen == GEN_II) return ((PokemonPartyGenII*)party)->index - 1;
        break;
    /* In Gen I, index is not relative at all to dex num.
     * In Gen II, index is the same as the dex num.
     */
    case STAT_NUM:
        if(gen == GEN_I) {
            val = ((PokemonPartyGenI*)party)->index;
            return table_pokemon_pos_get(pdata->pokemon_table, val);
        }
        if(gen == GEN_II) return ((PokemonPartyGenII*)party)->index - 1;
        break;
    case STAT_MOVE:
        if(gen == GEN_I) return ((PokemonPartyGenI*)party)->move[which];
        if(gen == GEN_II) return ((PokemonPartyGenII*)party)->move[which];
        break;
    case STAT_TYPE:
        if(gen == GEN_I) return ((PokemonPartyGenI*)party)->type[which];
        break;
    case STAT_OT_ID:
        if(gen == GEN_I) val = ((PokemonPartyGenI*)party)->ot_id;
        if(gen == GEN_II) val = ((PokemonPartyGenII*)party)->ot_id;
        break;
    case STAT_POKERUS:
        if(gen == GEN_II) return ((PokemonPartyGenII*)party)->pokerus;
        break;
    case STAT_SEL:
        if(gen == GEN_I) return pdata->stat_sel;
        if(gen == GEN_II) return pdata->stat_sel;
        break;
    case STAT_CONDITION:
        if(gen == GEN_I) return ((PokemonPartyGenI*)party)->status_condition = val;
        if(gen == GEN_II) return ((PokemonPartyGenII*)party)->status_condition = val;
        break;
    case STAT_HELD_ITEM:
        if(gen == GEN_II) return ((PokemonPartyGenII*)party)->held_item;
        break;
    default:
        furi_crash("STAT_GET: invalid stat");
        break;
    }

    return __builtin_bswap16(val);
}

void pokemon_stat_set(PokemonData* pdata, DataStat stat, DataStatSub which, uint16_t val) {
    furi_assert(pdata);
    void* party = pdata->party;
    int gen = pdata->gen;
    uint8_t recalc = 0;
    uint16_t val_swap = __builtin_bswap16(val);

    switch(stat) {
    case STAT_ATK:
        if(gen == GEN_I) ((PokemonPartyGenI*)party)->atk = val_swap;
        if(gen == GEN_II) ((PokemonPartyGenII*)party)->atk = val_swap;
        break;
    case STAT_DEF:
        if(gen == GEN_I) ((PokemonPartyGenI*)party)->def = val_swap;
        if(gen == GEN_II) ((PokemonPartyGenII*)party)->def = val_swap;
        break;
    case STAT_SPD:
        if(gen == GEN_I) ((PokemonPartyGenI*)party)->spd = val_swap;
        if(gen == GEN_II) ((PokemonPartyGenII*)party)->spd = val_swap;
        break;
    case STAT_SPC:
        if(gen == GEN_I) ((PokemonPartyGenI*)party)->spc = val_swap;
        break;
    case STAT_SPC_ATK:
        if(gen == GEN_II) ((PokemonPartyGenII*)party)->spc_atk = val_swap;
        break;
    case STAT_SPC_DEF:
        if(gen == GEN_II) ((PokemonPartyGenII*)party)->spc_def = val_swap;
        break;
    case STAT_HP:
        if(gen == GEN_I) {
            ((PokemonPartyGenI*)party)->hp = val_swap;
            ((PokemonPartyGenI*)party)->max_hp = val_swap;
        }
        if(gen == GEN_II) {
            ((PokemonPartyGenII*)party)->hp = val_swap;
            ((PokemonPartyGenII*)party)->max_hp = val_swap;
        }
        break;
    case STAT_ATK_EV:
        if(gen == GEN_I) ((PokemonPartyGenI*)party)->atk_ev = val_swap;
        if(gen == GEN_II) ((PokemonPartyGenII*)party)->atk_ev = val_swap;
        break;
    case STAT_DEF_EV:
        if(gen == GEN_I) ((PokemonPartyGenI*)party)->def_ev = val_swap;
        if(gen == GEN_II) ((PokemonPartyGenII*)party)->def_ev = val_swap;
        break;
    case STAT_SPD_EV:
        if(gen == GEN_I) ((PokemonPartyGenI*)party)->spd_ev = val_swap;
        if(gen == GEN_II) ((PokemonPartyGenII*)party)->spd_ev = val_swap;
        break;
    /* The SPC ATK/DEF EVs are not real values, we just pretend they are */
    case STAT_SPC_EV:
    case STAT_SPC_ATK_EV:
    case STAT_SPC_DEF_EV:
        if(gen == GEN_I) ((PokemonPartyGenI*)party)->spc_ev = val_swap;
        if(gen == GEN_II) ((PokemonPartyGenII*)party)->spc_ev = val_swap;
        break;
    case STAT_HP_EV:
        if(gen == GEN_I) ((PokemonPartyGenI*)party)->hp_ev = val_swap;
        if(gen == GEN_II) ((PokemonPartyGenII*)party)->hp_ev = val_swap;
        break;
    case STAT_IV:
        if(gen == GEN_I) ((PokemonPartyGenI*)party)->iv = val_swap;
        if(gen == GEN_II) ((PokemonPartyGenII*)party)->iv = val_swap;
        break;
    /* The IVs in GB byte order, are always:
     * atk, def, spd, spc
     * Like every other 16 bit quantity that the Flipper acts on, we need to
     * bytw swap them normally. However, the below accessors for individual
     * IV nibbles directly manipulate the data structures which will always
     * be in GB endianness.
     */
    case STAT_SPD_IV:
        if(gen == GEN_I) {
            ((PokemonPartyGenI*)party)->iv &= ~(0x0F << 12);
            ((PokemonPartyGenI*)party)->iv |= ((val & 0x0F) << 12);
        }
        if(gen == GEN_II) {
            ((PokemonPartyGenII*)party)->iv &= ~(0x0F << 12);
            ((PokemonPartyGenII*)party)->iv |= ((val & 0x0F) << 12);
        }
        break;
    /* In order to line up all of the dynamic stat accessors used as part of the
     * stat calculation loop, we need to overload the SPC IV accessor to allow
     * accessing SPC, SPC_ATK, and SPC_DEF. Note that only SPC exists, the ATK
     * and DEF are the overloaded values. This is so when, for example, gen i
     * calculates its SPC value, or gen ii calculates is SPC_DEF value, it will
     * always grab the same IV nibble.
     */
    case STAT_SPC_IV:
    case STAT_SPC_ATK_IV:
    case STAT_SPC_DEF_IV:
        if(gen == GEN_I) {
            ((PokemonPartyGenI*)party)->iv &= ~(0x0F << 8);
            ((PokemonPartyGenI*)party)->iv |= ((val & 0x0F) << 8);
        }
        if(gen == GEN_II) {
            ((PokemonPartyGenII*)party)->iv &= ~(0x0F << 8);
            ((PokemonPartyGenII*)party)->iv |= ((val & 0x0F) << 8);
        }
        break;
    case STAT_ATK_IV:
        if(gen == GEN_I) {
            ((PokemonPartyGenI*)party)->iv &= ~(0x0F << 4);
            ((PokemonPartyGenI*)party)->iv |= ((val & 0x0F) << 4);
        }
        if(gen == GEN_II) {
            ((PokemonPartyGenII*)party)->iv &= ~(0x0F << 4);
            ((PokemonPartyGenII*)party)->iv |= ((val & 0x0F) << 4);
        }
        break;
    case STAT_DEF_IV:
        if(gen == GEN_I) {
            ((PokemonPartyGenI*)party)->iv &= ~(0x0F);
            ((PokemonPartyGenI*)party)->iv |= (val & 0x0F);
        }
        if(gen == GEN_II) {
            ((PokemonPartyGenII*)party)->iv &= ~(0x0F);
            ((PokemonPartyGenII*)party)->iv |= (val & 0x0F);
        }
        break;
    case STAT_MOVE:
        if(gen == GEN_I) ((PokemonPartyGenI*)party)->move[which] = val;
        if(gen == GEN_II) ((PokemonPartyGenII*)party)->move[which] = val;
        break;
    case STAT_TYPE:
        /* Gen II doesn't have type assignment */
        if(gen == GEN_I) ((PokemonPartyGenI*)party)->type[which] = val;
        break;
    case STAT_LEVEL:
        if(gen == GEN_I) {
            ((PokemonPartyGenI*)party)->level = val;
            ((PokemonPartyGenI*)party)->level_again = val;
        }
        if(gen == GEN_II) ((PokemonPartyGenII*)party)->level = val;
        recalc = (RECALC_STATS | RECALC_EXP | RECALC_EVS);
        break;
    /* In Gen I, index is not relative at all to dex num.
     * In Gen II, index is the same as the dex num.
     */
    case STAT_INDEX:
        if(gen == GEN_I) {
            ((PokemonPartyGenI*)party)->index = val;
            ((TradeBlockGenI*)pdata->trade_block)->party_members[0] = val;
        }
        if(gen == GEN_II) {
            ((PokemonPartyGenII*)party)->index = val + 1;
            ((TradeBlockGenII*)pdata->trade_block)->party_members[0] = val + 1;
        }
        recalc = RECALC_ALL; // Always recalculate everything if we selected a different pokemon
        break;
    case STAT_NUM:
        if(gen == GEN_I)
            pokemon_stat_set(
                pdata,
                STAT_INDEX,
                NONE,
                table_stat_base_get(pdata->pokemon_table, val, STAT_BASE_INDEX, NONE));
        if(gen == GEN_II) pokemon_stat_set(pdata, STAT_INDEX, NONE, val);
        break;
    case STAT_OT_ID:
        if(gen == GEN_I) ((PokemonPartyGenI*)party)->ot_id = val_swap;
        if(gen == GEN_II) ((PokemonPartyGenII*)party)->ot_id = val_swap;
        break;
    case STAT_POKERUS:
        if(gen == GEN_II) ((PokemonPartyGenII*)party)->pokerus = val;
        break;
    case STAT_SEL:
        pdata->stat_sel = val;
        recalc = (RECALC_EVS | RECALC_IVS | RECALC_STATS);
        break;
    case STAT_EXP:
        if(gen == GEN_I) ((PokemonPartyGenI*)party)->exp[which] = val;
        if(gen == GEN_II) ((PokemonPartyGenII*)party)->exp[which] = val;
        break;
    case STAT_CONDITION:
        if(gen == GEN_I) ((PokemonPartyGenI*)party)->status_condition = val;
        if(gen == GEN_II) ((PokemonPartyGenII*)party)->status_condition = val;
        break;
    case STAT_HELD_ITEM:
        if(gen == GEN_II) ((PokemonPartyGenII*)party)->held_item = val;
        break;
    default:
        furi_crash("STAT_SET: invalid stat");
        break;
    }
    FURI_LOG_D(TAG, "[data] stat %s:%d set to 0x%X", stat_text_get(stat), which, val);
    pokemon_recalculate(pdata, recalc);
}

static void pokemon_stat_ev_calc(PokemonData* pdata, EvIv val) {
    furi_assert(pdata);
    int level;
    uint16_t ev;
    DataStat i;

    level = pokemon_stat_get(pdata, STAT_LEVEL, NONE);

    /* Generate STATEXP */
    switch(val) {
    case RANDIV_LEVELEV:
    case MAXIV_LEVELEV:
        ev = (0xffff / 100) * level;
        break;
    case RANDIV_MAXEV:
    case MAXIV_MAXEV:
        ev = 0xffff;
        break;
    default:
        ev = 0;
        break;
    }

    for(i = STAT_EV; i < STAT_EV_END; i++) {
        pokemon_stat_set(pdata, i, NONE, ev);
    }
}

static void pokemon_stat_iv_calc(PokemonData* pdata, EvIv val) {
    furi_assert(pdata);
    uint16_t iv;

    /* Set up IVs */
    switch(val) {
    case RANDIV_ZEROEV:
    case RANDIV_LEVELEV:
    case RANDIV_MAXEV:
        iv = (uint16_t)rand();
        break;
    default:
        iv = 0xFFFF;
        break;
    }

    pokemon_stat_set(pdata, STAT_IV, NONE, iv);
}

#define UINT32_TO_EXP(input, output_array)                     \
    do {                                                       \
        (output_array)[2] = (uint8_t)((input) & 0xFF);         \
        (output_array)[1] = (uint8_t)(((input) >> 8) & 0xFF);  \
        (output_array)[0] = (uint8_t)(((input) >> 16) & 0xFF); \
    } while(0)

void pokemon_exp_set(PokemonData* pdata, uint32_t exp) {
    furi_assert(pdata);
    uint8_t exp_tmp[3];
    int i;

    UINT32_TO_EXP(exp, exp_tmp);

    for(i = EXP_0; i <= EXP_2; i++) {
        pokemon_stat_set(pdata, STAT_EXP, i, exp_tmp[i]);
    }

    FURI_LOG_D(TAG, "[data] Set pkmn exp %d", (int)exp);
}

void pokemon_exp_calc(PokemonData* pdata) {
    furi_assert(pdata);
    int level;
    uint32_t exp;
    uint8_t growth = table_stat_base_get(
        pdata->pokemon_table, pokemon_stat_get(pdata, STAT_NUM, NONE), STAT_BASE_GROWTH, NONE);

    level = (int)pokemon_stat_get(pdata, STAT_LEVEL, NONE);
    /* Calculate exp */
    switch(growth) {
    case GROWTH_FAST:
        // https://bulbapedia.bulbagarden.net/wiki/Experience#Fast
        exp = (4 * level * level * level) / 5;
        break;
    case GROWTH_MEDIUM_FAST:
        // https://bulbapedia.bulbagarden.net/wiki/Experience#Medium_Fast
        exp = (level * level * level);
        break;
    case GROWTH_MEDIUM_SLOW:
        // https://bulbapedia.bulbagarden.net/wiki/Experience#Medium_Slow
        exp = (((level * level * level) * 6 / 5) - (15 * level * level) + (100 * level) - 140);
        break;
    case GROWTH_SLOW:
        // https://bulbapedia.bulbagarden.net/wiki/Experience#Slow
        exp = (5 * level * level * level) / 4;
        break;
    default:
        furi_crash("incorrect growth val");
        break;
    }

    pokemon_exp_set(pdata, exp);
}

/* Calculates stat from current level */
void pokemon_stat_calc(PokemonData* pdata, DataStat stat) {
    furi_assert(pdata);
    uint8_t iv;
    uint16_t ev;
    uint8_t base;
    uint8_t level;
    uint16_t calc;

    level = pokemon_stat_get(pdata, STAT_LEVEL, NONE);
    base = table_stat_base_get(
        pdata->pokemon_table, pokemon_stat_get(pdata, STAT_NUM, NONE), stat, NONE);
    ev = pokemon_stat_get(pdata, stat + STAT_EV_OFFS, NONE);
    iv = pokemon_stat_get(pdata, stat + STAT_IV_OFFS, NONE);

    /* Gen I and II calculation */
    // https://bulbapedia.bulbagarden.net/wiki/Stat#Generations_I_and_II
    calc = floor((((2 * (base + iv)) + floor(sqrt(ev) / 4)) * level) / 100);

    if(stat == STAT_HP)
        calc += (level + 10);
    else
        calc += 5;

    pokemon_stat_set(pdata, stat, NONE, calc);
}

/* Copy the traded-in Pokemon's main data to our struct */
void pokemon_stat_memcpy(PokemonData* dst, PokemonData* src, uint8_t which) {
    if(dst->gen == GEN_I) {
        ((TradeBlockGenI*)dst->trade_block)->party_members[0] =
            ((TradeBlockGenI*)src->trade_block)->party_members[which];
        memcpy(
            &(((TradeBlockGenI*)dst->trade_block)->party[0]),
            &(((TradeBlockGenI*)src->trade_block)->party[which]),
            sizeof(PokemonPartyGenI));
        memcpy(
            &(((TradeBlockGenI*)dst->trade_block)->nickname[0]),
            &(((TradeBlockGenI*)src->trade_block)->nickname[which]),
            sizeof(struct name));
        memcpy(
            &(((TradeBlockGenI*)dst->trade_block)->ot_name[0]),
            &(((TradeBlockGenI*)src->trade_block)->ot_name[which]),
            sizeof(struct name));
    } else if(dst->gen == GEN_II) {
        ((TradeBlockGenI*)dst->trade_block)->party_members[0] =
            ((TradeBlockGenI*)src->trade_block)->party_members[which];
        memcpy(
            &(((TradeBlockGenII*)dst->trade_block)->party[0]),
            &(((TradeBlockGenII*)src->trade_block)->party[which]),
            sizeof(PokemonPartyGenI));
        memcpy(
            &(((TradeBlockGenII*)dst->trade_block)->nickname[0]),
            &(((TradeBlockGenII*)src->trade_block)->nickname[which]),
            sizeof(struct name));
        memcpy(
            &(((TradeBlockGenII*)dst->trade_block)->ot_name[0]),
            &(((TradeBlockGenII*)src->trade_block)->ot_name[which]),
            sizeof(struct name));
    }
}

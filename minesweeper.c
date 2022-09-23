#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>

#include <notification/notification_messages.h>

#include "assets.h"

#define PLAYFIELD_WIDTH 16
#define PLAYFIELD_HEIGHT 7
#define TILE_WIDTH 8
#define TILE_HEIGHT 8

#define MINECOUNT 27

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} PluginEvent;

typedef enum {
  TileType0, // this HAS to be in order, then 
  TileType1,
  TileType2,
  TileType3,
  TileType4,
  TileType5,
  TileType6,
  TileType7,
  TileType8,
  TileTypeUncleared,
  TileTypeFlag,
  TileTypeMine
} TileType;

typedef enum {
    FieldEmpty, // <-- same goes for this
    FieldMine
} Field;

typedef struct {
  Field minefield[PLAYFIELD_WIDTH][PLAYFIELD_HEIGHT];
  TileType playfield[PLAYFIELD_WIDTH][PLAYFIELD_HEIGHT];
  int cursor_x;
  int cursor_y;
  bool game_started;
} Minesweeper;

static void input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue); 

    PluginEvent event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static void render_callback(Canvas* const canvas, void* ctx) {
    const Minesweeper* minesweeper_state = acquire_mutex((ValueMutex*)ctx, 25);
    if (minesweeper_state == NULL) {
      return;
    }
    canvas_set_font(canvas, FontPrimary);
    for (int y = 0; y < PLAYFIELD_HEIGHT; y++) {
      for (int x = 0; x < PLAYFIELD_WIDTH; x++) {
        if ( x == minesweeper_state->cursor_x && y == minesweeper_state->cursor_y) {
          canvas_invert_color(canvas);
        }
        switch (minesweeper_state->playfield[x][y]) {
          case TileType0:
            canvas_draw_xbm(
                canvas,
                x*TILE_HEIGHT, // x
                8 + (y * TILE_WIDTH), // y
                TILE_WIDTH,
                TILE_HEIGHT, 
                tile_0_bits);
            break;
          case TileType1:
            canvas_draw_xbm(
                canvas,
                x*TILE_HEIGHT, // x
                8 + (y * TILE_WIDTH), // y
                TILE_WIDTH,
                TILE_HEIGHT, 
                tile_1_bits);
            break;
          case TileType2:
            canvas_draw_xbm(
                canvas,
                x*TILE_HEIGHT, // x
                8 + (y * TILE_WIDTH), // y
                TILE_WIDTH,
                TILE_HEIGHT, 
                tile_2_bits);
            break;
          case TileType3:
            canvas_draw_xbm(
                canvas,
                x*TILE_HEIGHT, // x
                8 + (y * TILE_WIDTH), // y
                TILE_WIDTH,
                TILE_HEIGHT, 
                tile_3_bits);
            break;
          case TileType4:
            canvas_draw_xbm(
                canvas,
                x*TILE_HEIGHT, // x
                8 + (y * TILE_WIDTH), // y
                TILE_WIDTH,
                TILE_HEIGHT, 
                tile_4_bits);
            break;
          case TileType5:
            canvas_draw_xbm(
                canvas,
                x*TILE_HEIGHT, // x
                8 + (y * TILE_WIDTH), // y
                TILE_WIDTH,
                TILE_HEIGHT, 
                tile_5_bits);
            break;
          case TileType6:
            canvas_draw_xbm(
                canvas,
                x*TILE_HEIGHT, // x
                8 + (y * TILE_WIDTH), // y
                TILE_WIDTH,
                TILE_HEIGHT, 
                tile_6_bits);
            break;
          case TileType7:
            canvas_draw_xbm(
                canvas,
                x*TILE_HEIGHT, // x
                8 + (y * TILE_WIDTH), // y
                TILE_WIDTH,
                TILE_HEIGHT, 
                tile_7_bits);
            break;
          case TileType8:
            canvas_draw_xbm(
                canvas,
                x*TILE_HEIGHT, // x
                8 + (y * TILE_WIDTH), // y
                TILE_WIDTH,
                TILE_HEIGHT, 
                tile_8_bits);
            break;
          case TileTypeFlag:
            canvas_draw_xbm(
                canvas,
                x*TILE_HEIGHT, // x
                8 + (y * TILE_WIDTH), // y
                TILE_WIDTH,
                TILE_HEIGHT, 
                tile_flag_bits);
            break;
          case TileTypeUncleared:
            canvas_draw_xbm(
                canvas,
                x*TILE_HEIGHT, // x
                8 + (y * TILE_WIDTH), // y
                TILE_WIDTH,
                TILE_HEIGHT, 
                tile_uncleared_bits);
            break;
          case TileTypeMine:
            canvas_draw_xbm(
                canvas,
                x*TILE_HEIGHT, // x
                8 + (y * TILE_WIDTH), // y
                TILE_WIDTH,
                TILE_HEIGHT, 
                tile_mine_bits);
            break;
        }
        if ( x == minesweeper_state->cursor_x && y == minesweeper_state->cursor_y) {
          canvas_invert_color(canvas);
        }
      }
    }
    release_mutex((ValueMutex*)ctx, minesweeper_state);
}

static void setup_playfield(Minesweeper* minesweeper_state) {
  int mines_left = MINECOUNT;
  while(mines_left > 0) {
    int rand_x = rand() % PLAYFIELD_WIDTH;
    int rand_y = rand() % PLAYFIELD_HEIGHT;
    // make sure first guess isn't a mine
    if (minesweeper_state->minefield[rand_x][rand_y] == FieldEmpty &&
       (minesweeper_state->cursor_x != rand_x && minesweeper_state->cursor_y != rand_y )) { 
       minesweeper_state->minefield[rand_x][rand_y] = FieldMine;
       mines_left--;
    }
  }
}

static void place_flag(Minesweeper* minesweeper_state) {
  if (minesweeper_state->playfield[minesweeper_state->cursor_x][minesweeper_state->cursor_y] == TileTypeUncleared) {
    minesweeper_state->playfield[minesweeper_state->cursor_x][minesweeper_state->cursor_y] = TileTypeFlag;
  } else if (minesweeper_state->playfield[minesweeper_state->cursor_x][minesweeper_state->cursor_y] == TileTypeFlag) {
    minesweeper_state->playfield[minesweeper_state->cursor_x][minesweeper_state->cursor_y] = TileTypeUncleared;
  }
}

static void game_lost() {
   NotificationApp* notifications = furi_record_open(RECORD_NOTIFICATION); 
   notification_message(notifications, &sequence_set_vibro_on);
   furi_delay_ms(200);
   notification_message(notifications, &sequence_reset_vibro);
   furi_record_close(RECORD_NOTIFICATION);
}

static void play_move(Minesweeper* minesweeper_state, int cursor_x, int cursor_y) {
  if (minesweeper_state->playfield[cursor_x][cursor_y] != TileTypeUncleared) {
      // we're on an already uncovered field
      return;
  }
  if (minesweeper_state->minefield[cursor_x][cursor_y] == FieldMine) {
      // TODO: player loses!
      minesweeper_state->playfield[cursor_x][cursor_y] = TileTypeMine;
      game_lost();
      return; 
  } else {
    // get number of surrounding mines.
    int hint = 0;
    for (int y = cursor_y-1; y <= cursor_y+1; y++) {
      for (int x = cursor_x-1; x <= cursor_x+1; x++) {
        if ( x == cursor_x && y == cursor_y ) {
          // we're on the cell the user selected, so ignore.
          continue;
        }
        // make sure we don't go OOB
        if ( x >= 0 && x < PLAYFIELD_WIDTH && y >= 0 && y < PLAYFIELD_HEIGHT) {
          if(minesweeper_state->minefield[x][y] == FieldMine) {
              hint ++;
          }
        }
      }
    }
    // 〜(￣▽￣〜) don't judge me (〜￣▽￣)〜
    minesweeper_state->playfield[cursor_x][cursor_y] = hint;
    FURI_LOG_D("Minesweeper", "Setting %d,%d to %d", cursor_x, cursor_y, hint);
    if (hint == 0) {
      // auto open surrounding fields.
      for (int auto_y = cursor_y-1; auto_y <= cursor_y+1; auto_y++) {
        for (int auto_x = cursor_x-1; auto_x <= cursor_x+1; auto_x++) {
          if ( auto_x == cursor_x && auto_y == cursor_y ) {
            continue;
          }
          if ( auto_x >= 0 && auto_x < PLAYFIELD_WIDTH && auto_y >= 0 && auto_y < PLAYFIELD_HEIGHT) {
            if (minesweeper_state->playfield[auto_x][auto_y] == TileTypeUncleared) {
              play_move(minesweeper_state, auto_x, auto_y);
            }
          }
        }
      }
    }
  }
}

static void minesweeper_state_init(Minesweeper* const plugin_state) {
    plugin_state->cursor_x = plugin_state->cursor_y = 0;  
    plugin_state->game_started = false;
    for (int y = 0; y < PLAYFIELD_HEIGHT; y++) {
      for (int x = 0; x < PLAYFIELD_WIDTH; x++){
          plugin_state->minefield[x][y] = FieldEmpty;
          plugin_state->playfield[x][y] = TileTypeUncleared;
      }
    }
}

int32_t minesweeper_app(void* p) {
  UNUSED(p);

  FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(PluginEvent));
  
  Minesweeper* minesweeper_state = malloc(sizeof(Minesweeper));
  // setup
  minesweeper_state_init(minesweeper_state);
  
  ValueMutex state_mutex;
  if (!init_mutex(&state_mutex, minesweeper_state, sizeof(minesweeper_state))) {
      FURI_LOG_E("Minesweeper", "cannot create mutex\r\n");
      free(minesweeper_state);
      return 255;
  }
  // BEGIN IMPLEMENTATION

  // Set system callbacks
  ViewPort* view_port = view_port_alloc(); 
  view_port_draw_callback_set(view_port, render_callback, &state_mutex);
  view_port_input_callback_set(view_port, input_callback, event_queue);
  
  // Open GUI and register view_port
  Gui* gui = furi_record_open("gui"); 
  gui_add_view_port(gui, view_port, GuiLayerFullscreen);

  PluginEvent event;
  for (bool processing = true; processing;) {
    FuriStatus event_status = furi_message_queue_get(event_queue, &event, 100);
    Minesweeper* minesweeper_state = (Minesweeper*)acquire_mutex_block(&state_mutex);
    if(event_status == FuriStatusOk) {
      // press events
      if(event.type == EventTypeKey) {
        if(event.input.type == InputTypeShort) {  
          switch(event.input.key) {
            case InputKeyUp:
              minesweeper_state->cursor_y--;
              if(minesweeper_state->cursor_y < 0) {
                 minesweeper_state->cursor_y = 0;
              }
              break;
            case InputKeyDown:
              minesweeper_state->cursor_y++;
              if(minesweeper_state->cursor_y >= PLAYFIELD_HEIGHT) {
                 minesweeper_state->cursor_y = PLAYFIELD_HEIGHT-1;
              }
              break;
            case InputKeyRight:
              minesweeper_state->cursor_x++;
              if(minesweeper_state->cursor_x >= PLAYFIELD_WIDTH) {
                 minesweeper_state->cursor_x = PLAYFIELD_WIDTH-1;
              }
              break;
            case InputKeyLeft:
              minesweeper_state->cursor_x--;
              if(minesweeper_state->cursor_x < 0) {
                 minesweeper_state->cursor_x = 0;
              }
              break;
            case InputKeyOk:
              if (!minesweeper_state->game_started) {
                setup_playfield(minesweeper_state);
                minesweeper_state->game_started = true;
              }
              play_move(
                minesweeper_state,
                minesweeper_state->cursor_x,
                minesweeper_state->cursor_y);
              break;
            case InputKeyBack:
              // Exit the plugin
              processing = false;
              break;
          }
        } else if (event.input.type == InputTypeLong) {
          // hold events
          FURI_LOG_D("Minesweeper", "Got a long press!");
          switch(event.input.key) {
            case InputKeyUp:
            case InputKeyDown:
            case InputKeyRight:
            case InputKeyLeft:
              break;
            case InputKeyOk:
              FURI_LOG_D("Minesweeper", "Toggling flag");
              place_flag(minesweeper_state);
              break;
            case InputKeyBack:
              processing = false;
              break;
          } 
        }
      } 
    } else {
      // event timeout
      ;
    }
    view_port_update(view_port);
    release_mutex(&state_mutex, minesweeper_state);
  }
  view_port_enabled_set(view_port, false);
  gui_remove_view_port(gui, view_port);
  furi_record_close("gui");
  view_port_free(view_port);
  furi_message_queue_free(event_queue);
  delete_mutex(&state_mutex);
  free(minesweeper_state);

  return 0;
}


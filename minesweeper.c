#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>

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
    TileTypeUncleared, // this HAS to be the first element so it gets assigned 0 for easier init :)
    TileTypeCleared,
    TileType0,
    TileType1,
    TileType2,
    TileType3,
    TileType4,
    TileType5,
    TileType6,
    TileType7,
    TileType8,
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
  int cursor_cell_x;
  int cursor_cell_y;
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
        if ( x == minesweeper_state->cursor_cell_x && y == minesweeper_state->cursor_cell_y) {
          canvas_invert_color(canvas);
        }
        if (minesweeper_state->minefield[x][y] == FieldMine) {
          canvas_draw_xbm(
              canvas,
              x*TILE_HEIGHT, // x
              8 + (y * TILE_WIDTH), // y
              TILE_WIDTH,
              TILE_HEIGHT, 
              tile_mine_bits);
        } else {
          canvas_draw_xbm(
              canvas,
              x*TILE_HEIGHT, // x
              8 + (y * TILE_WIDTH), // y
              TILE_WIDTH,
              TILE_HEIGHT, 
              tile_uncleared_bits);
        }
        if ( x == minesweeper_state->cursor_cell_x && y == minesweeper_state->cursor_cell_y) {
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
       (minesweeper_state->cursor_cell_x != rand_x && minesweeper_state->cursor_cell_y != rand_y )) { 
       minesweeper_state->minefield[rand_x][rand_y] = FieldMine;
       mines_left--;
    }
  }
}

static void minesweeper_state_init(Minesweeper* const plugin_state) {
    plugin_state->cursor_cell_x = plugin_state->cursor_cell_y = 0;  
    plugin_state->game_started = false;
    for (int y = 0; y < PLAYFIELD_HEIGHT; y++) {
      for (int x = 0; x < PLAYFIELD_WIDTH; x++){
          plugin_state->minefield[x][y] = FieldEmpty;
          plugin_state->playfield[x][y] = TileTypeUncleared;
      }
    }
    //plugin_state->minefield = {0};
    //plugin_state->playfield = {0};
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
        if(event.input.type == InputTypePress) {  
          switch(event.input.key) {
            case InputKeyUp:
              minesweeper_state->cursor_cell_y--;
              if(minesweeper_state->cursor_cell_y < 0) {
                 minesweeper_state->cursor_cell_y = 0;
              }
              break;
            case InputKeyDown:
              minesweeper_state->cursor_cell_y++;
              if(minesweeper_state->cursor_cell_y > PLAYFIELD_HEIGHT) {
                 minesweeper_state->cursor_cell_y = PLAYFIELD_HEIGHT;
              }
              break;
            case InputKeyRight:
              minesweeper_state->cursor_cell_x++;
              if(minesweeper_state->cursor_cell_x > PLAYFIELD_WIDTH) {
                 minesweeper_state->cursor_cell_x = PLAYFIELD_WIDTH;
              }
              break;
            case InputKeyLeft:
              minesweeper_state->cursor_cell_x--;
              if(minesweeper_state->cursor_cell_x < 0) {
                 minesweeper_state->cursor_cell_x = 0;
              }
              break;
            case InputKeyOk:
              if (!minesweeper_state->game_started) {
                setup_playfield(minesweeper_state);
                minesweeper_state->game_started = true;
              }
              break;
            case InputKeyBack:
              // Exit the plugin
              processing = false;
              break;
          }
        }
      } 
    } else {
      FURI_LOG_D("Minesweeper", "FuriMessageQueue: event timeout");
    // event timeout
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


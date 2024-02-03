# Flipper helper functions

A collection of methods my games are using. 

---

## UI

Extra methods for graphics. It can read/write/copy anything from the screen, create a buffer that you can swap with the one inside the canvas object and every flipper draw call should work with it.

Call this at the end of your program
```
ui_cleanup();
```

### How to use it

* To test if a pixel is black or not: ``test_pixel(canvas_buffer, pixel_x, pixel_y, screen_width)``
* Convert Flippers icon to the same type to u8g2: ``uint8_t * result=image_data(canvas, icon);``
* Flatten X/Y pixel coordinate to array index ``uint32_t result=pixel_index(x, y)``
* Draw icon region with overlay/invert support: ``draw_icon_clip(canvas, icon, image_x, image_y, screen_x, screen_y, width, height, draw_mode)``
* Draw flipped region icon with overlay/invert support: ``draw_icon_clip_flipped(canvas, icon, image_x, image_y, screen_x, screen_y, width, height, draw_mode)``
* Draw rounded box: ``draw_rounded_box(canvas, x, y, width, heigth, draw_mode)``
* Draw rounded box frame: ``draw_rounded_box_frame(canvas, x, y, width, heigth, draw_mode)``
* Draw rectangle: ``draw_rectangle(canvas, x, y, width, heigth, draw_mode)``
* Invert screen colors in a rectangle: ``invert_rectangle(canvas, x, y, width, height)``
* Invert screen colors in a shape: ``invert_shape(canvas, image, x, y, width, height)``
* Read pixel: ``read_pixel(canvas, x, y)``
* Set pixel: ``set_pixel(canvas, x, y, draw_mode)``
* Draw line: ``set_pixel(canvas, x1, y1, x2, y1, draw_mode)``
* Test coordinate if it is in the screen: ``in_screen(x, y)``
* Test coordinate if it is in the screen: ``in_screen(x, y)``
* Get screen buffer: ``get_buffer(canvas)``
* Create a new buffer: ``make_buffer()``
* Clone buffer: ``clone_buffer(source_buffer, result_buffer)``

---
## Math (dml)

Some extra math functionality.

### Vector math

* Vector type
* add 2 vectors together ``Vector result = vector_add(a, b);``
* divide 2 vectors components from each other ``Vector result = vector_sub(a, b);``
* Multiplying vector components together ``Vector result = vector_mul_components(a, b);``
* Dividing vector components ``Vector result = vector_div_components(a, b);``
* Get the magnitude of a vector ``float result = vector_magnitude(a);``
* Normalize a vector (components add up to one) ``Vector result = vector_normalized(a);``
* Distance between vectors ``Vector result = vector_distance(a, b);``
* Get dot product ``float result = vector_distance(a, b);``

### Linear Interpolation

* Interpolate a single value ``float result = lerp(start_value, end_value, time);``
* Interpolate Vector ``Vector result = lerp(start_vector, end_vector, time);``
* Quadratic interpolation ``Vector result = quadratic_2d(start_vector, control_vector, end_vector, time);``


### Extra

* min macro ``min(a,b);``
* max macro ``max(a,b);``
* abs macro ``abs(value);``

---
## Queue

A basic queue functionality. Useful to chain your methods in order, so they can follow each other. Mainly used to state switching an animation state handling in my games.

Currently it only supports one queue chain.
### How to use it
* Store a queue handler in your application state
  ```c
  typedef struct {
      //...your game states
      QueueState queue_state;
  } AppState;
  ```
* To add a new queue item
  ```c
  //Needed functions
  void done_callback(void *ctx) {
     //Called when the item is being removed from the queue
  }
  void start_callback(void *ctx) {
     //Called every tick when it is active
  }
  void render_callback(const void *ctx, Canvas *const canvas) {
     //Called while rendering in case you want to render something
  }
  
  //In your code somewhere
  //Not needed callbacks can be set to NULL
  enqueue(&(app_state->queue_state), app_state, done_callback, start_callback, render_callback, queue_run_duration)
  ```
* Required to run
  ```c
  //Put this in a place that runs every tick
  bool queue_ran = run_queue(&(app_state->queue_state), app_state);
  
  //Put this in your render function
  render_queue(&(app_state->queue_state), app_state, canvas);
  ```
* Clean up at the end of your program
  ```c
  queue_clear(&(app_state->queue_state));
  ```

---
## Card

### Types
* Card
  - Stores data about one card from the deck
  - Properties
    - pip: the symbol (0: spades, 1: hearths, 2: diamonds, 3: clubs)
    - character: the letter for the card (2 is 0, A is 12)
    - disabled: can be used freely. I used it to hide the card during animation
    - flipped: if this is set to true, instead of the card graphics the backside will be rendered
* Deck
  - Store a whole deck
  - Properties
    - deck_count: How many decks are added into this
    - cards: contains the cards
    - card_count: how many cards are in the deck
    - index: useful to track your current index in the deck
* Hand
  - Store cards that are given for example to the player
  - Properties
    - Cards: cards in the hand
    - index: to keep track of where we are in the hand
    - max: how many cards can be in the hand

### Usage

Before you can use it, you have to load the card_graphics.png file in your games assets
```c
    set_card_graphics(&I_card_graphics);
```

- Draw a single card: ``draw_card_at(pos_x, pos_y, pip, character, canvas)``
- Draw single card with invert support: ``draw_card_at_colored(pos_x, pos_y, pip, character, inverted, canvas)``
- Draw the blackjack deck in the bottom right corner (it can stack up to 21 cards): ``draw_deck(cards, count, canvas)``
- Draw the backside of the card: ``draw_card_back_at(pos_x, pos_y, canvas)``
- Generate a deck: ``generate_deck(deck_pointer, how_many_decks)``
- Blackjack hand calculation: ``hand_count(cards_array, how_many_cards)``
- Draw card animation state: ``draw_card_animation(card, from_vector, control_vector, to_vector, current_phase, extra_time_margin, canvas)``
- Create hand pointer: ``init_hand(hand_pointer, card_count)``
- Free hand pointer: ``free_hand(hand_pointer)``
- Add card to the hand: ``add_to_hand(hand_pointer, card)``
- Draw an empty space (like the one in solitaire): ``draw_card_space(pos_x, pos_y, is_highlighted, canvas)``
- Draw cards in a column: ``draw_hand_column(hand, pos_x, pos_y, selected_card, canvas)``
- Remove card from the deck: ``remove_from_deck(index_to_remove, deck_pointer)``
- Get first non flipped card index in a hand: ``first_non_flipped_card(hand)``
- Move cards from one hand to another starting at index: ``extract_hand_region(from_hand, to_hand, start_index)``lipped card index in a hand: ``first_non_flipped_card(hand)``
- Move the whole hand into another hand: ``add_hand_region(from_hand, to_hand)``

## Menu

Do not use it, it barely works and only used in blackjack.

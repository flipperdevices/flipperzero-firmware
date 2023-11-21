#include <Arduino.h>
#include "SPI.h"
#include "emulator_sdl.h"

#define GAMEBOY_HEIGHT 160
#define GAMEBOY_WIDTH 144

#define SCREEN_HEIGHT 64
#define SCREEN_WIDTH 128


byte pixels[GAMEBOY_HEIGHT * GAMEBOY_WIDTH / 4];
static int button_start, button_select, button_a, button_b, button_down, button_up, button_left, button_right;
static bool resize = true;
static int initial_render_position_x = 0;
static int initial_render_position_y = 0;


byte getColorIndexFromFrameBuffer(int x, int y) {
  int offset = x + y * 160;
  return (pixels[offset >> 2] >> ((offset & 3) << 1)) & 3;
}
void SDL_Flip() {
    const int gb_pal_colors[4] = {0, 1, 1, 1};

    int x, y;
    float scaleX = (float)GAMEBOY_WIDTH / SCREEN_WIDTH;
    float scaleY = (float)GAMEBOY_HEIGHT / SCREEN_HEIGHT;

    for (y = 0; y < SCREEN_HEIGHT; y++) {
        Serial1.print("Y:");
        Serial1.print((char)y);
        for(x = 0; x < SCREEN_WIDTH; x += 8){

            char c = 0;
            for (uint8_t i = 0; i < 8; i++) {
                //  Sin escalar
                
                int gbX = x + i + initial_render_position_x; // Invierte la dirección sin escalar
                int gbY = y + initial_render_position_y;
                if (gbX < 0) gbX = 0; // Ajusta los límites
                if (gbY < 0) gbY = 0;
                if(resize) {
                  gbX = (int)((x + i) * scaleX + 0.5); // Invierte la dirección
                  gbY = (int)(y * scaleY +  0.5);
                }
                if (gbX >= GAMEBOY_WIDTH) gbX = GAMEBOY_WIDTH - 1;
                if (gbY >= GAMEBOY_HEIGHT) gbY = GAMEBOY_HEIGHT - 1;
                

                int color = getColorIndexFromFrameBuffer(gbX, gbY);
                if (color != 0 && color != 2) {
                    c |= 1 << (7 - i);
                } else {
                    c &= ~(1 << ( 7- i));
                }
            }
            Serial1.print(c);
        }
        Serial1.flush();
    }
}
void sdl_init(void)
{
}
int sdl_update(void){
  if (Serial.available() > 0) {
    char r = Serial.read();
    switch (r) {
      case 'S':
        resize = !resize;
      break;
      case 'U':
        initial_render_position_y--;
      break;
      case 'D':
       initial_render_position_y++;
      break;
      case '<':
        initial_render_position_x--;
      break;
      case '>':
       initial_render_position_x++;
      break;
    }
  }
  sdl_frame();
	return 0;
}
byte* sdl_get_framebuffer(void)
{
	return pixels;
}
void sdl_frame(void)
{
	SDL_Flip();
}



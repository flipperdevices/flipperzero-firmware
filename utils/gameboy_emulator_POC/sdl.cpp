#include <Arduino.h>
#include "SPI.h"
#include "sdl.h"
// #include "Adafruit_GFX.h"

#define GAMEBOY_HEIGHT 160
#define GAMEBOY_WIDTH 144

#define SCREEN_HEIGHT 64
#define SCREEN_WIDTH 128


byte pixels[GAMEBOY_HEIGHT * GAMEBOY_WIDTH / 4];
// byte colorData[SCREEN_HEIGHT][SCREEN_WIDTH];
static int button_start, button_select, button_a, button_b, button_down, button_up, button_left, button_right;
static bool resize = true;
static int initial_render_position_x = 0;
static int initial_render_position_y = 0;

/*
byte getColorIndexFromFrameBuffer(int x, int y) {
  int offset = x + y * 160;
  return (pixels[offset >> 2] >> ((offset & 3) << 1)) & 3;
}
void SDL_Flip(byte *screen){
  int i,j;
  for(i = 0;i<GAMEBOY_WIDTH;i++){
    for(j = 0;j<GAMEBOY_HEIGHT;j++){
        Serial.print(getColorIndexFromFrameBuffer(j,i));
    }
    Serial.println("\n");
  }
  Serial.println("frame printed B)\n");
}*/
// void ditherImage(byte *pixels) {
//   for (int y = 0; y < GAMEBOY_HEIGHT; ++y) {
//     for (int x = 0; x < GAMEBOY_WIDTH; ++x) {
//       int current = y * GAMEBOY_WIDTH + x;
//       byte oldPixel = pixels[current];
//       byte newPixel = oldPixel >= 128 ? 255 : 0;
//       pixels[current] = newPixel;
//       int quant_error = oldPixel - newPixel;

//       if (x < GAMEBOY_WIDTH - 1) {
//         if (current + 1 < GAMEBOY_WIDTH * GAMEBOY_HEIGHT) {
//           pixels[current + 1] = pixels[current + 1] + quant_error * 7 / 16;
//         }
//       }
//       if (x > 0 && y < GAMEBOY_HEIGHT - 1) {
//         if (current + GAMEBOY_WIDTH - 1 < GAMEBOY_WIDTH * GAMEBOY_HEIGHT) {
//           pixels[current + GAMEBOY_WIDTH - 1] = pixels[current + GAMEBOY_WIDTH - 1] + quant_error * 3 / 16;
//         }
//       }
//       if (y < GAMEBOY_HEIGHT - 1) {
//         if (current + GAMEBOY_WIDTH < GAMEBOY_WIDTH * GAMEBOY_HEIGHT) {
//           pixels[current + GAMEBOY_WIDTH] = pixels[current + GAMEBOY_WIDTH] + quant_error * 5 / 16;
//         }
//       }
//       if (x < GAMEBOY_WIDTH - 1 && y < GAMEBOY_HEIGHT - 1) {
//         if (current + GAMEBOY_WIDTH + 1 < GAMEBOY_WIDTH * GAMEBOY_HEIGHT) {
//           pixels[current + GAMEBOY_WIDTH + 1] = pixels[current + GAMEBOY_WIDTH + 1] + quant_error * 1 / 16;
//         }
//       }
//     }
//   }
// }

byte getColorIndexFromFrameBuffer(int x, int y) {
  int offset = x + y * 160;
  return (pixels[offset >> 2] >> ((offset & 3) << 1)) & 3;
}
/*
void SDL_Flip(byte *screen) {
    const int gb_pal_colors[4] = {0, 1, 1, 1};
    
    int x, y;
    float scaleX = (float)GAMEBOY_WIDTH / SCREEN_WIDTH;
    float scaleY = (float)GAMEBOY_HEIGHT / SCREEN_HEIGHT;

    for(y = 0; y < SCREEN_HEIGHT; y++){
     Serial1.print("Y:");
      Serial1.print((char)y);
      for(x = 0; x < SCREEN_WIDTH; x += 8){

        char c = 0;
        for (uint8_t i = 0; i < 8; ++i)
        {
          // int gbX = (int)((x + i) * scaleX);
          // int gbY = (int)(y * scaleY);
           int gbX = (int)((x+i) * scaleX + 0.5); // Redondear al entero más cercano
           int gbY = (int)(y * scaleY + 0.5); // Redondear al entero más cercano

          // Ajustar los límites para asegurarse de no exceder las dimensiones del Gameboy
          if (gbX >= GAMEBOY_WIDTH) gbX = GAMEBOY_WIDTH - 1;
          if (gbY >= GAMEBOY_HEIGHT) gbY = GAMEBOY_HEIGHT - 1;

          int color = getColorIndexFromFrameBuffer(gbX, gbY);
          // Check if the color is 2 or 3
          if (color != 0 && color != 2)
          {
              c |= 1 << i;
          }
          else
          {
              c &= ~(1 << i); // Establece el bit j en 0
          }
        }
        // Serial.print(gb_pal_colors[getColorIndexFromFrameBuffer(gbX, gbY)]);
        Serial1.print(c);
      }
      // ++y;
      // Serial.println("");
      Serial1.flush();
    }
    // Serial.println("frame printed B)\n");
}*/
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
/*
void SDL_Flip(byte *screen) {
    const int gb_pal_colors[4] = {1, 1,0,0};
    // byte colorData[SCREEN_HEIGHT * SCREEN_WIDTH]; // Arreglo unidimensional para almacenar colores


    int x, y;
    float scaleX = (float)GAMEBOY_WIDTH / SCREEN_WIDTH;
    float scaleY = (float)GAMEBOY_HEIGHT / SCREEN_HEIGHT;

    // Obtener los colores y almacenarlos en la matriz
    for(y = 0; y < SCREEN_HEIGHT; y++) {
        Serial1.print("Y:");
        Serial1.print((char)y);
        for(x = 0; x < SCREEN_WIDTH; x+=8) {

          byte byteData = 0;
            for (uint8_t j = 0; j < 8; ++j) {

              int gbX = (int)((x + j) * scaleX);
              int gbY = (int)(y * scaleY);

              if (gbX >= GAMEBOY_WIDTH) gbX = GAMEBOY_WIDTH - 1;
              if (gbY >= GAMEBOY_HEIGHT) gbY = GAMEBOY_HEIGHT - 1;

              int color = getColorIndexFromFrameBuffer(gbX, gbY);
              
              if (color != 0) {
                  byteData |= 1 << j; // Invertir el orden para leer de izquierda a derecha
              } else {
                  byteData &= ~(1 << j); // Establecer el bit j en 0
              }
            }
            // Imprimir el byte convertido
            Serial1.print(byteData);
            
        }
        Serial1.flush();
    }
}*/
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
  // ditherImage(pixels); // Aplicar dithering al frame original
	SDL_Flip();
}



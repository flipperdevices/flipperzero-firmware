#include <Arduino.h>
#include "emulator_lcd.h"
#include "emulator_cpu.h"
#include "emulator_interrupt.h"
#include "emulator_sdl.h"
#include "emulator_mem.h"


static int lcd_line;
static int lcd_ly_compare;


/* LCD STAT */
static int ly_int;	/* LYC = LY coincidence interrupt enable */
static int mode2_oam_int;
static int mode1_vblank_int;
static int mode0_hblank_int;
static int ly_int_flag;
static int lcd_mode;

/* LCD Control */
static int lcd_enabled;
static int window_tilemap_select;
static int window_enabled;
static int tilemap_select;
static int bg_tiledata_select;
static int sprite_size;
static int sprites_enabled;
static int bg_enabled;
static int scroll_x, scroll_y;
static int window_x, window_y;

static byte bgpalette[] = {3, 2, 1, 0};
static byte sprpalette1[] = {0, 1, 2, 3};
static byte sprpalette2[] = {0, 1, 2, 3};
static unsigned long colours[4] = {0xFFFFFF, 0xC0C0C0, 0x808080, 0x000000};

struct sprite {
	int y, x, tile, flags;
};

enum {
	PRIO  = 0x80,
	VFLIP = 0x40,
	HFLIP = 0x20,
	PNUM  = 0x10
};

unsigned char lcd_get_stat(void)
{

	return (ly_int)<<6 | lcd_mode;
}

void lcd_write_bg_palette(unsigned char n)
{
	bgpalette[0] = (n>>0)&3;
	bgpalette[1] = (n>>2)&3;
	bgpalette[2] = (n>>4)&3;
	bgpalette[3] = (n>>6)&3;
}

void lcd_write_spr_palette1(unsigned char n)
{
	sprpalette1[0] = 0;
	sprpalette1[1] = (n>>2)&3;
	sprpalette1[2] = (n>>4)&3;
	sprpalette1[3] = (n>>6)&3;
}

void lcd_write_spr_palette2(unsigned char n)
{
	sprpalette2[0] = 0;
	sprpalette2[1] = (n>>2)&3;
	sprpalette2[2] = (n>>4)&3;
	sprpalette2[3] = (n>>6)&3;
}

void lcd_write_scroll_x(unsigned char n)
{
//	printf("x scroll changed to %02x\n", n);
	scroll_x = n;
}

void lcd_write_scroll_y(unsigned char n)
{
	scroll_y = n;
}

int lcd_get_line(void)
{
	return lcd_line;
}

void lcd_write_stat(unsigned char c)
{
	ly_int = !!(c&0x40);
}

void lcd_write_control(unsigned char c)
{
//	printf("LCDC set to %02x\n", c);
//	cpu_print_debug();
	bg_enabled            = !!(c & 0x01);
	sprites_enabled       = !!(c & 0x02);
	sprite_size           = !!(c & 0x04);
	tilemap_select        = !!(c & 0x08);
	bg_tiledata_select    = !!(c & 0x10);
	window_enabled        = !!(c & 0x20);
	window_tilemap_select = !!(c & 0x40);
	lcd_enabled           = !!(c & 0x80);
}

void lcd_set_ly_compare(unsigned char c)
{
	lcd_ly_compare = c;
}

void lcd_set_window_y(unsigned char n) {
	window_y = n;
}

void lcd_set_window_x(unsigned char n) {
	window_x = n;
}

static void swap(struct sprite *a, struct sprite *b)
{
	struct sprite c;

	 c = *a;
	*a = *b;
	*b =  c;
}

static void sort_sprites(struct sprite *s, int n)
{
	int swapped, i;

	do
	{
		swapped = 0;
		for(i = 0; i < n-1; i++)
		{
			if(s[i].x < s[i+1].x)
			{
				swap(&s[i], &s[i+1]);
				swapped = 1;
			}
		}
	}
	while(swapped);
}

void drawColorIndexToFrameBuffer(int x, int y, byte idx, byte *b) {
  int screen_pixel_offset = x + y * 160;
  int pixel_byte_id = screen_pixel_offset >> 2;
  int bit_pixel_offset = (screen_pixel_offset & 3) << 1;
  int clear_mask = ~(3 << bit_pixel_offset);
  b[pixel_byte_id] = (b[pixel_byte_id] & clear_mask) | (idx << bit_pixel_offset);
}

static void draw_bg_and_window(byte *b, int line, const unsigned char *raw_mem)
{
	int x;

	for(x = 0; x < 160; x++)
	{
		unsigned int map_select, map_offset, tile_num, tile_addr, xm, ym;
		unsigned char b1, b2, mask, colour;

		/* Convert LCD x,y into full 256*256 style internal coords */
		if(line >= window_y && window_enabled && line - window_y < 144)
		{
			xm = x;
			ym = line - window_y;
			map_select = window_tilemap_select;
		}
		else {
			if(!bg_enabled)
			{
				//b[line*640 + x] = 0;
        drawColorIndexToFrameBuffer(x,line,0,b);
				return;
			}
			xm = (x + scroll_x)%256;
			ym = (line + scroll_y)%256;
			map_select = tilemap_select;
		}

		/* Which pixel is this tile on? Find its offset. */
		/* (y/8)*32 calculates the offset of the row the y coordinate is on.
		 * As 256/32 is 8, divide by 8 to map one to the other, this is the row number.
		 * Then multiply the row number by the width of a row, 32, to find the offset.
		 * Finally, add x/(256/32) to find the offset within that row. 
		 */
		map_offset = (ym/8)*32 + xm/8;

		tile_num = raw_mem[0x9800 + map_select*0x400 + map_offset];
		if(bg_tiledata_select)
			tile_addr = 0x8000 + tile_num*16;
		else
			tile_addr = 0x9000 + ((signed char)tile_num)*16;

		b1 = raw_mem[tile_addr+(ym%8)*2];
		b2 = raw_mem[tile_addr+(ym%8)*2+1];
		mask = 128>>(xm%8);
		colour = (!!(b2&mask)<<1) | !!(b1&mask);
		//b[line*640 + x] = colours[bgpalette[colour]];
	  drawColorIndexToFrameBuffer(x,line,bgpalette[colour],b);
	}
}

static void draw_sprites(byte *b, int line, int nsprites, struct sprite *s, const unsigned char *raw_mem)
{
	int i;

	for(i = 0; i < nsprites; i++)
	{
		unsigned int b1, b2, tile_addr, sprite_line, x;

		/* Sprite is offscreen */
		if(s[i].x < -7)
			continue;

		/* Which line of the sprite (0-7) are we rendering */
		sprite_line = s[i].flags & VFLIP ? (sprite_size ? 15 : 7)-(line - s[i].y) : line - s[i].y;

		/* Address of the tile data for this sprite line */
		tile_addr = 0x8000 + (s[i].tile*16) + sprite_line*2;

		/* The two bytes of data holding the palette entries */
		b1 = raw_mem[tile_addr];
		b2 = raw_mem[tile_addr+1];

		/* For each pixel in the line, draw it */
		for(x = 0; x < 8; x++)
		{
			unsigned char mask, colour;
			byte *pal;

			if((s[i].x + x) >= 160)
				continue;

			mask = s[i].flags & HFLIP ? 128>>(7-x) : 128>>x;
			colour = ((!!(b2&mask))<<1) | !!(b1&mask);
			if(colour == 0)
				continue;


			pal = (s[i].flags & PNUM) ? sprpalette2 : sprpalette1;
			/* Sprite is behind BG, only render over palette entry 0 */
			if(s[i].flags & PRIO)
			{
				unsigned int temp = b[line*640+(x + s[i].x)];
				if(temp != colours[bgpalette[0]])
					continue;
			}
			//b[line*640+(x + s[i].x)] = colours[pal[colour]];
		  drawColorIndexToFrameBuffer(x + s[i].x,line,pal[colour],b);
		}
	}
}

static void render_line(int line)
{
  const unsigned char *raw_mem = mem_get_raw();
	int i, c = 0;

	struct sprite s[10];
	byte *b = sdl_get_framebuffer();

	for(i = 0; i<40; i++)
	{
		int y;

		y = raw_mem[0xFE00 + (i*4)] - 16;
		if(line < y || line >= y + 8+(sprite_size*8))
			continue;

		s[c].y     = y;
		s[c].x     = raw_mem[0xFE00 + (i*4) + 1]-8;
		s[c].tile  = raw_mem[0xFE00 + (i*4) + 2];
		s[c].flags = raw_mem[0xFE00 + (i*4) + 3];
		c++;

		if(c == 10)
			break;
	}

	if(c)
		sort_sprites(s, c);

	/* Draw the background layer */
	draw_bg_and_window(b, line, raw_mem);

	draw_sprites(b, line, c, s, raw_mem);


}

// static void draw_stuff(void)
// {
//   byte *b = sdl_get_framebuffer();
// 	int y, tx, ty;

// 	for(ty = 0; ty < 24; ty++)
// 	{
// 	for(tx = 0; tx < 16; tx++)
// 	{
// 	for(y = 0; y<8; y++)
// 	{
// 		unsigned char b1, b2;
// 		int tileaddr = 0x8000 +  ty*0x100 + tx*16 + y*2;

// 		b1 = mem_get_raw(tileaddr);
// 		b2 = mem_get_raw(tileaddr+1);
//    /*
// 		b[(ty*640*8)+(tx*8) + (y*640) + 0 + 0x1F400] = colours[(!!(b1&0x80))<<1 | !!(b2&0x80)];
// 		b[(ty*640*8)+(tx*8) + (y*640) + 1 + 0x1F400] = colours[(!!(b1&0x40))<<1 | !!(b2&0x40)];
// 		b[(ty*640*8)+(tx*8) + (y*640) + 2 + 0x1F400] = colours[(!!(b1&0x20))<<1 | !!(b2&0x20)];
// 		b[(ty*640*8)+(tx*8) + (y*640) + 3 + 0x1F400] = colours[(!!(b1&0x10))<<1 | !!(b2&0x10)];
// 		b[(ty*640*8)+(tx*8) + (y*640) + 4 + 0x1F400] = colours[(!!(b1&0x8))<<1 | !!(b2&0x8)];
// 		b[(ty*640*8)+(tx*8) + (y*640) + 5 + 0x1F400] = colours[(!!(b1&0x4))<<1 | !!(b2&0x4)];
// 		b[(ty*640*8)+(tx*8) + (y*640) + 6 + 0x1F400] = colours[(!!(b1&0x2))<<1 | !!(b2&0x2)];
// 		b[(ty*640*8)+(tx*8) + (y*640) + 7 + 0x1F400] = colours[(!!(b1&0x1))<<1 | !!(b2&0x1)];
//    */
// 	}
// 	}
// 	}
// }
bool lcd_cycle(unsigned int cycles)
{
  static int this_frame = 0;
  static int prev_cycles = 0;

  this_frame += cycles - prev_cycles;
  if (this_frame >= 70224/4)
    this_frame -= 70224/4;
  prev_cycles = cycles;

  if (this_frame < 456/4) {
    if (this_frame < 204/4)
      lcd_mode = 2;
    else if (this_frame < 284/4)
      lcd_mode = 3;
    else {
      lcd_mode = 0;
      lcd_line = 0;
    }
    return false;
  }

  static int sub_line = 0;
  static int prev_frame = 0;

  sub_line += this_frame - prev_frame;

  if (sub_line >= 456/4) {
    sub_line -= 456/4;

    if (lcd_line < 144)
      render_line(lcd_line);

    lcd_line += 1;

    if (lcd_line >= 144)
      lcd_mode = 1;

    if (ly_int && lcd_line == lcd_ly_compare)
      interrupt(INTR_LCDSTAT);

    if (lcd_line == 144)
    {
      // sdl_update();
      interrupt(INTR_VBLANK);
      // sdl_frame();
      return true;
    }
  }
  return false;
}


All display operations based on u8g2 library.

API available as struct, contains u8g2 functions, instance and fonts:

```C
typedef struct {
	MutexValue display; /// MutexValue<u8g2_t*>
	void (*u8g2_SetFont)(u8g2_t *u8g2, const uint8_t  *font);
	void (*u8g2_SetDrawColor)(u8g2_t *u8g2, uint8_t color);
	void (*u8g2_SetFontMode)(u8g2_t *u8g2, uint8_t is_transparent);
	u8g2_uint_t (*u8g2_DrawStr)(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, const char *str);

	U8g2Fonts fonts;
} DisplayU8g2;

typedef struct {
	const uint8_t* u8g2_font_6x10_mf;
} U8g2Fonts;
```

First of all you can open display API instance by calling `open_display_u8g2`

```C
/// Get display instance and API
inline DisplayU8g2* open_display_u8g2(const char* name) {
    return furi_open(name);
}
```

Default display name is `/dev/u8g2_display`.

For draw something to display you can get display instance pointer by calling `take_display_u8g2`, do something and commit your changes by calling `commit_display_u8g2`:

```C
/// return pointer in case off success, NULL otherwise
inline u8g2_t* take_display_u8g2(MutexValue api, uint32_t timeout) {
	return (u8g2_t*)take_mutex(api, timeout);
}

inline void commit_display_u8g2(MutexValue api, u8g2_t* display) {
	commit_mutex(api, display);
}
```

## Usage example

```C
void u8g2_example(void* p) {
    DisplayU8g2* display_api = open_display_u8g2("/dev/u8g2_display");
    if(display_api == NULL) furiac_exit(NULL); // display not available, critical error

    u8g2_t* display = take_display_u8g2(display_api);
    if(display != NULL) {
        display_api->u8g2_SetFont(display, display_api->fonts.u8g2_font_6x10_mf);
        display_api->u8g2_SetDrawColor(display, 1);
        display_api->u8g2_SetFontMode(display, 1);
        display_api->u8g2_DrawStr(display, 2, 12, "hello world!");
    }
    commit_display_u8g2(display_api, display);
}
```
#include <pb_encode.h>
#include <pb_decode.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "stackusage.pb.h"

static uint8_t g_msgbuf[256];
static size_t g_msglen;

/* This is a hacky way to measure actual stack usage of functions.
 * It works by copying the stack to a global variable, and then
 * finding the lowest location that has been modified.
 * Currently this assumes that the platform uses a descending stack.
 */
#define MAX_STACK_ENTRIES 1024
static uint32_t g_stackbuf[MAX_STACK_ENTRIES];
static volatile uint32_t *g_stackptr;

void start_stack_measuring()
{
    uint32_t i = 0;
    g_stackptr = (volatile uint32_t*)((uintptr_t)&i - MAX_STACK_ENTRIES * sizeof(uint32_t));
    for (i = 0; i < MAX_STACK_ENTRIES; i++)
    {
        g_stackbuf[i] = g_stackptr[i];
    }
}

int end_stack_measuring()
{
    uint32_t i = 0;
    for (i = 0; i < MAX_STACK_ENTRIES; i++)
    {
        if (g_stackbuf[i] != g_stackptr[i])
        {
            return (MAX_STACK_ENTRIES - i) * sizeof(uint32_t);
        }
    }
    assert(false);
    return 0;
}

void do_encode()
{
    pb_ostream_t stream = pb_ostream_from_buffer(g_msgbuf, sizeof(g_msgbuf));
    SettingsGroup msg = SettingsGroup_init_zero;
    bool status;

    msg.has_settings = true;
    msg.settings.id = 1;
    strcpy(msg.settings.name, "abcd");
    msg.settings.en = true;
    msg.settings.has_begin = true;
    msg.settings.begin.label = 1234;
    msg.settings.begin.properties_count = 1;
    msg.settings.begin.properties[0].which_field = Property_DeviceA_Mode_tag;
    msg.settings.begin.properties[0].field.DeviceA_Mode = 2;

    status = pb_encode(&stream, SettingsGroup_fields, &msg);
    g_msglen = stream.bytes_written;
    assert(status);
    assert(g_msglen > 10);
}

void do_decode()
{
    pb_istream_t stream = pb_istream_from_buffer(g_msgbuf, g_msglen);
    SettingsGroup msg = SettingsGroup_init_zero;
    bool status;

    status = pb_decode(&stream, SettingsGroup_fields, &msg);
    assert(status);
    assert(msg.settings.begin.properties[0].field.DeviceA_Mode == 2);
}

int main()
{
    int stack_encode, stack_decode;

    start_stack_measuring();
    do_encode();
    stack_encode = end_stack_measuring();

    start_stack_measuring();
    do_decode();
    stack_decode = end_stack_measuring();

    /* Print machine-readable to stdout and user-readable to stderr */
    printf("%d %d\n", stack_encode, stack_decode);
    fprintf(stderr, "Stack usage: encode %d bytes, decode %d bytes\n",
            stack_encode, stack_decode);
    return 0;
}

#include "kstd.h"
#include "console.h"

#define BUFFER_START    0xB8000
#define BUFFER_END      0xBFD00 // 0xB8000 + 25 rows * 80 columns * 16 bits

s_cons_attrs cons =
{
    0,
    0,
    0x0F
};
// \n \r \t
static void init_cons(void)
{
    cons.attrs = 0x0F;
    cons.line = 0;
    cons.column = 0;
}

static void clear_screen(void)
{
    char* c = (void*) BUFFER_START;
    for (int i = 0; i < (BUFFER_END - BUFFER_START) / 8; ++i)
    {
       c[i] = 0; 
    }
    init_cons();
}

static void set_cons_color(const char color)
{
    cons.attrs = color;
}

static char cons_putc(const char c)
{
    char* buf = (void*) BUFFER_START;
    if (c == '\n')
        ++cons.line;
    else if (c == '\t')
    {
        cons.column = cons.column < 71 ? cons.column + 1 : 0;
        if (cons.column == 0)
            ++(cons.line);
    }
    else if (c == '\r')
        cons.column = 0;
    else
    {
        buf[(cons.line * 80 + cons.column) * 2] = c;
        buf[((cons.line * 80 + cons.column) * 2) + 1] = cons.attrs;
        cons.column = cons.column < 79 ? cons.column + 1 : 0;
        if (cons.column == 0)
            ++(cons.line);
    }

    return cons.line == 25;
}

static int get_attrs(const char* s, int remaining)
{
    int pos = 0;
    if ((unsigned char) s[pos++] != CONS_ESCAPE)
        return 0;
    else if (remaining == 0) // Only a cons_escape, nothing else behind
        return 1;
    unsigned char act = s[pos];
    if (act == CONS_CLEAR)
    {
        clear_screen();
        return 2;
    }
    else if (act == CONS_COLOR && remaining >= 2)
    {
        set_cons_color(s[pos + 1]);
        return 3;
    }
    else if (act == CONS_SETX && remaining >= 2)
    {
        cons.column = s[pos + 1];
        return 3;
    }
    else if (act == CONS_SETY && remaining >= 2)
    {
        cons.line = s[pos + 1];
        return 3;
    }
    else
        return 1;
}

int write(const char* s, size_t length)
{
    int remove = 0;
    for (unsigned int i = 0; i < length; ++i)
    {
        int move = get_attrs(s + i, length - i - 1);
        remove += move;
        i += move;
        if (i >= length)
            break;
        if (cons_putc(s[i]) == 1)
            return i;
    }

    return length - remove;
}

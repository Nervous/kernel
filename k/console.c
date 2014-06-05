#include "kstd.h"
#include "serial.h"
#include "console.h"

#define BEGIN_BUFF 0xB8000

console_info c_i = { 0, 0, 0x0F };

typedef struct
{
  unsigned char charact;
  unsigned char color;
} s_cell;


void init_console(void)
{
  c_i.x_pos = 0;
  c_i.y_pos = 0;
  c_i.color = 0x0F;
}

void clear_screen(void)
{
  char* buf = (void*)0xB8000;
  c_i.x_pos = 0;
  c_i.y_pos = 0;
  for (int i = 0; i < 3999; i++)
  {
    buf[i] = 0;
  }
  init_console();
}

void set_color(char color)
{
  c_i.color = color;
}

void set_x(char x)
{
  if (x < 80)
    c_i.x_pos = x;
}

void set_y(char y)
{
  if (y < 25)
    c_i.y_pos = y;
}

char write_char(char c)
{
  if (c_i.x_pos + c_i.y_pos * 80 >= 2000)
    return 0;
  if (c == '\n')
  {
    c_i.y_pos++;
    c_i.x_pos = 0;
    return 0;
  }
  if (c == '\r')
  {
    c_i.x_pos = 0;
    return 0;
  }
  if (c == '\t')
  {
    c_i.x_pos += 8;
    c_i.y_pos = (c_i.x_pos) > 79 ? c_i.y_pos + 1 : c_i.y_pos;
    c_i.x_pos = c_i.x_pos % 80;
    return 0;
  }
  s_cell* buf = (void*)0xB8000;
  buf[c_i.x_pos + c_i.y_pos * 80].color = c_i.color;
  buf[c_i.x_pos + c_i.y_pos * 80].charact = c;
  if (c_i.x_pos == 79)
  {
    c_i.x_pos = 0;
    c_i.y_pos++;
  }
  else
  {
    c_i.x_pos++;
  }
  return 1;
}

int write_string(char* s, size_t length)
{
  for (size_t i = 0; i < length; i++)
  {
    if (!write_char(s[i]))
      return i + 1;
  }
  return length;
}

int write(const char *buf, size_t count)
{
  const void* data = buf;
  c_send(0x3f8, data, count);
  int ret = 0;
  for (size_t i = 0; i < count; i++)
  {
    if ((unsigned char)buf[i] == CONS_ESCAPE)
    {
      if (i != count - 1)
        switch (buf[i + 1])
        {
          case CONS_CLEAR :
            clear_screen();
            i += 1;
            break;
          case CONS_COLOR :
            if (i < count - 2)
              set_color(buf[i + 2]);
            i += 2;
            break;
          case CONS_SETX :
            if (i < count - 2)
              set_x(buf[i + 2]);
            break;
          case CONS_SETY :
            if (i < count - 2)
              set_y(buf[i + 2]);
            break;
          default:
            i++;
            break;
        }
    }
    else
    {
      write_char(buf[i]);
      ret++;
    }
  }
  return ret;
}

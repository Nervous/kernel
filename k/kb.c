#include "kb.h"
#include "stdio.h"

static t_uint8 inb(t_uint16 port)
{
    t_uint8 b = 0;
    __asm__ volatile("inb %1, %0\n\t"
                     : "=&a" (b)
                     : "d" ((t_uint16) port));
    return b;
}

void keyboard_handler(void)
{
  unsigned char i;
  static int lshift_enable;
  static int rshift_enable;

  do {
    i = inb(0x64);
  } while ((i & 0x01) == 0);

  i = inb(0x60);
  i--;

  if (i < 0x80) {		/* touche enfoncee */
    switch (i) {
      case 0x29:
        lshift_enable = 1;
        break;
      case 0x35:
        rshift_enable = 1;
        break;
      default:
        write((const char *)&kbdmap[i * 4 + (lshift_enable || rshift_enable)], 1);
    }
  } else {		/* touche relachee */
    i -= 0x80;
    switch (i) {
      case 0x29:
        lshift_enable = 0;
        break;
      case 0x35:
        rshift_enable = 0;
        break;
    }
  }
}

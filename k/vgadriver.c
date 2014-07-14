#include "libvga.h"
#include "kstd.h"

int setvideo(int mode)
{
  if (mode == VIDEO_TEXT)
    libvga_switch_mode3h();
  else if (mode == VIDEO_GRAPHIC)
    libvga_switch_mode13h();
  else
    return -1;
  return 0;

}

void swap_frontbuffer(const void *buffer)
{
  const char* buf = buffer;
  char* tmp = libvga_get_framebuffer();
  for (int i = 0; i < 320 * 200; i++)
  {
    tmp[i] = buf[i];
  }
}

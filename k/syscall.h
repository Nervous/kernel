#ifndef SYSCALL_H
# define SYSCALL_H

# include "kstd.h"

void* (*syscall_table[])() =
{
  (void*)write,
  /*
  (void*)sbrk,
  (void*)getkey,
  (void*)gettick,
  (void*)open,
  (void*)read,
  (void*)seek,
  (void*)close,
  (void*)setvideo,
  (void*)swap_frontbuffer,
  (void*)getpalette,
  (void*)setpalette,
  (void*)playsound,
  (void*)getmouse,
  (void*)send,
  (void*)recv
  */
};


#endif

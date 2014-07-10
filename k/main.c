/*
* Copyright (c) LSE
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
* * THIS SOFTWARE IS PROVIDED BY LSE AS IS AND ANY * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL LSE BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "multiboot.h"
#include "kstd.h"
#include "stdio.h"
#include "idt.h"

#include "console.h"
#include "serial.h"
#include "gdt.h"

#include "kfs.h"

#include "timer.h"

void test_syscall(char *buf, size_t count);

char test[4];

void	k_main(unsigned long		magic,
	       multiboot_info_t*	info)
{
    (void) magic;
    (void) info;
    {
        set_gdt();
        init_uart();
        init_idt();
        int kfs = init_kfs(info);
        printf("%c%c", CONS_ESCAPE, CONS_CLEAR);
        if (kfs != 0)
        {
            printf("Failed to init KFS.");
            while (1)
                ;
        }
        /*int ret = open("test.txt", O_RDONLY);
        char str[256] = {'\0'};

        read(ret, str, 256);
        printf("%s\n", str); // KFS TESTING */
        test[0] = 't';
        test[1] = 'e';
        test[2] = 's';
        test[3] = 't';
        printf("%c%c", CONS_ESCAPE, CONS_CLEAR);
        test_syscall(test, 4);
        while(1)
        {
        }
    }

  return;
}

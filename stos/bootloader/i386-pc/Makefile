CUR_DIR		:= bootloader/i386-pc/

PROG		:= stos.grub
OBJS		:= start.o loader.o pagination.o minilibc.o
INSTALL_DIR	:= root/boot

$(PROG)_CFLAGS	:= $(stos_CFLAGS)
$(PROG)_ASFLAGS	:= $(stos_ASFLAGS)
$(PROG)_LDFLAGS	:= -Wl,-T$O$(CUR_DIR)linker.ld

PROGSUBDIRS	:=

BOOTFILES	:= root/boot/$(PROG)

include $Omk/prog.mk

$(PROG): $O$(CUR_DIR)linker.ld

GRUBBINS	:= root/boot/grub/stage1 root/boot/grub/stage2 \
		   root/boot/grub/e2fs_stage1_5

BOOTFILES	:= $(GRUBBINS) root/boot/grub/menu.lst

grub-0.97-i386-pc.tar.gz:
	$(call run,WGET,wget -q ftp://alpha.gnu.org/gnu/grub/$@)

grub-0.97-i386-pc/: grub-0.97-i386-pc.tar.gz
	$(call run,UNTAR,tar xmf $^)

commandline: $O$(CUR_DIR)commandline
	$(call run,CP,cp $< $@)

root/boot/grub/menu.lst: commandline $(MODULES) root/kernel/stos root/boot/stos.grub | root/boot/grub/
	$(call run,MENULST,)
	@echo "default 0" > $@
	@echo "timeout 0" >> $@
	@echo "title stos" >> $@
	@echo "root (hd0,0)" >> $@
	@echo -n "kernel /boot/stos.grub " >> $@
	@cat commandline >> $@
	@echo "module /kernel/stos" >> $@
	@echo "module /kernel/initramfs.img" >> $@
	@for mod in $(notdir $(MODULES)); \
	do \
		echo "module /kernel/modules/$$mod" >> $@; \
	done

root/boot/grub/%: grub-0.97-i386-pc/ | root/boot/grub/
	$(call run,COPY,cp $^/boot/grub/$(notdir $@) $@)

bootsector: $(GRUBBINS)
	$(call run,MKBOOTSECTOR,)
	@dd if=/dev/zero of=$@ bs=512 count=2048 > /dev/null 2>&1
	@dd if=root/boot/grub/stage1 of=$@ conv=notrunc status=noxfer > /dev/null 2>&1
	@dd if=root/boot/grub/stage2 of=$@ conv=notrunc seek=1 obs=512 status=noxfer > /dev/null 2>&1

bootable: bootsector | stos-i386-pc.boot
	@(sleep 1; echo 'root (hd0,0)'; sleep 1; echo 'setup (hd0)'; sleep 3; kill `cat qemu-pid`) | $(QEMU) -display curses -pidfile qemu-pid stos-i386-pc.boot
	@dd if=stos-i386-pc.boot of=bootsector bs=512 count=2048 > /dev/null 2>&1
	$(call run,TOUCH,touch bootable)

stos-i386-pc.boot: include/kernel/arch grub-0.97-i386-pc/ bootsector rootfs.img
	$(call run,MK,cat bootsector rootfs.img > $@)
	$(call run,SFDISK,echo "stos-i386-pc.boot1 : start= 2048$(COMMA) size= 20000$(COMMA) Id=83" | sfdisk -qf -u S $@ > /dev/null 2>&1)

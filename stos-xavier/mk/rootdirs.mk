# FIXME: This file should be dynamic.

ROOTDIRS	:= root/kernel root/kernel/modules
ROOTDIRS	+= root/boot root/boot/grub

$(addsuffix /,$(ROOTDIRS)) root/:
	$(call run,MKDIR,mkdir $@)

$(foreach D,$(ROOTDIRS),$(eval $(D)/: | $(dir $(D))))

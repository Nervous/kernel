# FIXME: This file should be dynamic.

INITRAMFSDIRS	:= initramfs/bin

$(addsuffix /,$(INITRAMFSDIRS)) initramfs/:
	$(call run,MKDIR,mkdir $@)

$(foreach D,$(INITRAMFSDIRS),$(eval $(D)/: | $(dir $(D))))

ifdef MOD-K
# The module should be built-in.
PROGSUBDIRS	:=
include $Omk/progsubdirs.mk

else
ifdef MOD-M

_MOD-M		:= $(MOD-M)
MOD-M		:= $(addprefix $(CUR_DIR),$(MOD-M))
MODULES		+= $(MOD-M)
MOBJS		:= $(addprefix $(CUR_DIR),$(OBJS))
$(MOD-M): $(MOBJS)

$(MOD-M): CFLAGS	:= $(stos_CFLAGS)
$(MOD-M): LDFLAGS	:=
$(MOD-M): ASFLAGS	:= $(stos_ASFLAGS)

root/kernel/modules/$(_MOD-M): $(MOD-M) | root/kernel/modules/
	$(call run,CP,cp $< $@)
	$(call run,STRIP,$(STRIP) --strip-unneeded $@)

MODULES_INSTALL	:= $(MODULES_INSTALL) root/kernel/modules/$(_MOD-M)

include $Omk/deps.mk

endif
endif

MOD-K :=
MOD-M :=

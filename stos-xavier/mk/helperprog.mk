_HELPER		:= $(addprefix $(CUR_DIR),$(HELPER))

$(HELPER)_OBJS	:=

include $Omk/helpersubdirs.mk

$(HELPER)_CFLAGS	?=
$(HELPER)_ASFLAGS	?=
LIBS			?=

$(_HELPER): CC		:= $(HOST_CC)
$(_HELPER): CFLAGS	:= $(HOST_CFLAGS) $($(HELPER)_CFLAGS)
$(_HELPER): ASFLAGS	:= $(HOST_ASFLAGS) $($(HELPER)_ASFLAGS)

$(_HELPER): $($(HELPER)_OBJS)

$(HELPER)_LDFLAGS	?=
$(_HELPER)_LDFLAGS	:= $($(HELPER)_LDFLAGS)
$(_HELPER)_OBJS		:= $($(HELPER)_OBJS)

$(_HELPER):
	$(call run,BIN,$(HOST_LD) -o $@ $($@_OBJS) $($@_LDFLAGS))

HELPER		:= $(_HELPER)

ALL_PROGS	:= $(ALL_PROGS) $(HELPER)

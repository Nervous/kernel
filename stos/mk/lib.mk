_LIB		:= $(addprefix $(CUR_DIR),$(LIB))

$(LIB)_OBJS	:=

include $Omk/libsubdirs.mk

$(LIB)_CFLAGS			?=
$(LIB)_ASFLAGS			?=

# Tricky thing: define the variable CFLAGS in $(_LIB).a and in all its
# dependancies to be equal to $($(LIB)_CFLAGS)
$(_LIB).a: CFLAGS := $($(LIB)_CFLAGS)
$(_LIB).a: ASFLAGS := $($(LIB)_ASFLAGS)
$(_LIB).a: $($(LIB)_OBJS)

$(LIB)_FULL_PATH	:= $(_LIB)
$(LIB)_LINKFLAGS	:= -L$(dir $($(LIB)_FULL_PATH)) $(subst lib,-l,$(LIB))

LIB		:= $(_LIB)

ALL_LIBS	:= $(ALL_LIBS) $(LIB)

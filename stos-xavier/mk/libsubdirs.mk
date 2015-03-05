$(LIB)_OBJS	+= $(addprefix $(CUR_DIR),$(OBJS))

include $Omk/deps.mk

SUBDIRS		:= $(LIBSUBDIRS)

include $Omk/subdirs.mk

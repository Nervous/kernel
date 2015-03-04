$(HELPER)_OBJS	+= $(addprefix $(CUR_DIR),$(OBJS))

include $Omk/deps.mk

SUBDIRS		:= $(HELPERSUBDIRS)

include $Omk/subdirs.mk

CUR_DIR	:= bin/dash-0.5.6/src/

SRC	:= alias.c arith_yacc.c arith_yylex.c cd.c error.c eval.c exec.c \
	expand.c histedit.c input.c jobs.c mail.c main.c memalloc.c \
	miscbltin.c mystring.c options.c output.c parser.c redir.c show.c \
	system.c trap.c var.c

GEN_SRC	:= builtins.c init.c nodes.c syntax.c
GEN_HDR	:= builtins.h nodes.h syntax.h token.h

HELPER		:= mkinit
OBJS		:= mkinit.o
HELPERSUBDIRS	:=
include $Omk/helperprog.mk

$(CUR_DIR)init.c: $(HELPER) $(addprefix $O$(CUR_DIR),$(SRC))
	$(call run,MKINIT,./$^)

HELPER		:= mksyntax
OBJS		:= mksyntax.o
HELPERSUBDIRS	:=
include $Omk/helperprog.mk

$(CUR_DIR)syntax.c $(CUR_DIR)syntax.h: $(HELPER)
	$(call run,MKSYNTAX,./$^)

HELPER		:= mknodes
OBJS		:= mknodes.o
HELPERSUBDIRS	:=
include $Omk/helperprog.mk

$(CUR_DIR)nodes.c $(CUR_DIR)nodes.h: $(HELPER) $O$(CUR_DIR)nodetypes $O$(CUR_DIR)nodes.c.pat
	$(call run,MKNODES,./$^)

$(CUR_DIR)token.h: $O$(CUR_DIR)mktokens
	$(call run,MKTOKENS,sh $^ $@)

$(CUR_DIR)builtins.def: $O$(CUR_DIR)builtins.def.in $O$(CUR_DIR)/../config.h
	$(call run,HOST_CC,$(HOST_CC) $(dash_CFLAGS) -E -x c -o $@ $<)

$(CUR_DIR)builtins.c $(CUR_DIR)builtins.h: $(CUR_DIR)mkbuiltins $(CUR_DIR)builtins.def
	$(call run,MKBUILTINS,sh $^)

$(CUR_DIR)builtins.o: $(CUR_DIR)builtins.h

OBJS	:= $(SRC:.c=.o) $(GEN_SRC:.c=.o)

# Less than perfect, but I don't want to track the dependencies by hand.
$(addprefix $(CUR_DIR),$(SRC)): $(addprefix $(CUR_DIR),$(GEN_HDR))

PROGSUBDIRS	:= bltin/

include $Omk/progsubdirs.mk

/*
 * This file was generated by the mkbuiltins program.
 */

#define ALIASCMD (builtincmd + 3)
#define BREAKCMD (builtincmd + 4)
#define CDCMD (builtincmd + 5)
#define COMMANDCMD (builtincmd + 7)
#define DOTCMD (builtincmd + 0)
#define ECHOCMD (builtincmd + 9)
#define EVALCMD (builtincmd + 10)
#define EXECCMD (builtincmd + 11)
#define EXITCMD (builtincmd + 12)
#define EXPORTCMD (builtincmd + 13)
#define FALSECMD (builtincmd + 14)
#define GETOPTSCMD (builtincmd + 15)
#define HASHCMD (builtincmd + 16)
#define JOBSCMD (builtincmd + 17)
#define LOCALCMD (builtincmd + 18)
#define PRINTFCMD (builtincmd + 19)
#define PWDCMD (builtincmd + 20)
#define READCMD (builtincmd + 21)
#define RETURNCMD (builtincmd + 23)
#define SETCMD (builtincmd + 24)
#define SHIFTCMD (builtincmd + 25)
#define TESTCMD (builtincmd + 2)
#define TRAPCMD (builtincmd + 27)
#define TRUECMD (builtincmd + 1)
#define TYPECMD (builtincmd + 29)
#define UNALIASCMD (builtincmd + 30)
#define UNSETCMD (builtincmd + 31)
#define WAITCMD (builtincmd + 32)

#define NUMBUILTINS 33

#define BUILTIN_SPECIAL 0x1
#define BUILTIN_REGULAR 0x2
#define BUILTIN_ASSIGN 0x4

struct builtincmd {
	const char *name;
	int (*builtin)(int, char **);
	unsigned flags;
};

extern const struct builtincmd builtincmd[];
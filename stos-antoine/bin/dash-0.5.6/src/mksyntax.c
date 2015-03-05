/*-
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 * Copyright (c) 1997-2005
 *	Herbert Xu <herbert@gondor.apana.org.au>.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Kenneth Almquist.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * This program creates syntax.h and syntax.c.
 */

#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "parser.h"


struct synclass {
	char *name;
	char *comment;
};

/* Syntax classes */
struct synclass synclass[] = {
	{ "CWORD",	"character is nothing special" },
	{ "CNL",	"newline character" },
	{ "CBACK",	"a backslash character" },
	{ "CSQUOTE",	"single quote" },
	{ "CDQUOTE",	"double quote" },
	{ "CENDQUOTE",	"a terminating quote" },
	{ "CBQUOTE",	"backwards single quote" },
	{ "CVAR",	"a dollar sign" },
	{ "CENDVAR",	"a '}' character" },
	{ "CLP",	"a left paren in arithmetic" },
	{ "CRP",	"a right paren in arithmetic" },
	{ "CEOF",	"end of file" },
	{ "CCTL",	"like CWORD, except it must be escaped" },
	{ "CSPCL",	"these terminate a word" },
	{ "CIGN",	"character should be ignored" },
	{ NULL,		NULL }
};


/*
 * Syntax classes for is_ functions.  Warning:  if you add new classes
 * you may have to change the definition of the is_in_name macro.
 */
struct synclass is_entry[] = {
	{ "ISDIGIT",	"a digit" },
	{ "ISUPPER",	"an upper case letter" },
	{ "ISLOWER",	"a lower case letter" },
	{ "ISUNDER",	"an underscore" },
	{ "ISSPECL",	"the name of a special parameter" },
	{ NULL, 	NULL }
};

static char writer[] = "\
/*\n\
 * This file was generated by the mksyntax program.\n\
 */\n\
\n";


static FILE *cfile;
static FILE *hfile;
static char *syntax[513];

static void filltable(char *);
static void init(void);
static void add(char *, char *);
static void print(char *);
static void output_type_macros(void);
int main(int, char **);

int
main(int argc, char **argv)
{
	int i;
	char buf[80];
	int pos;
	char *prog_dirname;
	char *syntax_path;

	prog_dirname = dirname(argv[0]);
	syntax_path = malloc(strlen(prog_dirname) + strlen("/syntax.c") + 1);
	strcpy(syntax_path, prog_dirname);
	strcat(syntax_path, "/syntax.c");

	/* Create output files */
	if ((cfile = fopen(syntax_path, "w")) == NULL) {
		perror("syntax.c");
		exit(2);
	}
	strcpy(syntax_path, prog_dirname);
	strcat(syntax_path, "/syntax.h");
	if ((hfile = fopen(syntax_path, "w")) == NULL) {
		perror("syntax.h");
		exit(2);
	}
	free(syntax_path);
	fputs(writer, hfile);
	fputs(writer, cfile);

	fputs("#include <ctype.h>\n", hfile);
	fputs("\n", hfile);
	fputs("#ifdef CEOF\n", hfile);
	fputs("#undef CEOF\n", hfile);
	fputs("#endif\n", hfile);
	fputs("\n", hfile);

	/* Generate the #define statements in the header file */
	fputs("/* Syntax classes */\n", hfile);
	for (i = 0 ; synclass[i].name ; i++) {
		sprintf(buf, "#define %s %d", synclass[i].name, i);
		fputs(buf, hfile);
		for (pos = strlen(buf) ; pos < 32 ; pos = (pos + 8) & ~07)
			putc('\t', hfile);
		fprintf(hfile, "/* %s */\n", synclass[i].comment);
	}
	putc('\n', hfile);
	fputs("/* Syntax classes for is_ functions */\n", hfile);
	for (i = 0 ; is_entry[i].name ; i++) {
		sprintf(buf, "#define %s %#o", is_entry[i].name, 1 << i);
		fputs(buf, hfile);
		for (pos = strlen(buf) ; pos < 32 ; pos = (pos + 8) & ~07)
			putc('\t', hfile);
		fprintf(hfile, "/* %s */\n", is_entry[i].comment);
	}
	putc('\n', hfile);
	fprintf(hfile, "#define SYNBASE %d\n", 130);
	fprintf(hfile, "#define PEOF %d\n\n", -130);
	fprintf(hfile, "#define PEOA %d\n\n", -129);
	putc('\n', hfile);
	fputs("#define BASESYNTAX (basesyntax + SYNBASE)\n", hfile);
	fputs("#define DQSYNTAX (dqsyntax + SYNBASE)\n", hfile);
	fputs("#define SQSYNTAX (sqsyntax + SYNBASE)\n", hfile);
	fputs("#define ARISYNTAX (arisyntax + SYNBASE)\n", hfile);
	putc('\n', hfile);
	output_type_macros();		/* is_digit, etc. */
	putc('\n', hfile);

	/* Generate the syntax tables. */
	fputs("#include \"shell.h\"\n", cfile);
	fputs("#include \"syntax.h\"\n\n", cfile);
	init();
	fputs("/* syntax table used when not in quotes */\n", cfile);
	add("\n", "CNL");
	add("\\", "CBACK");
	add("'", "CSQUOTE");
	add("\"", "CDQUOTE");
	add("`", "CBQUOTE");
	add("$", "CVAR");
	add("}", "CENDVAR");
	add("<>();&| \t", "CSPCL");
	syntax[1] = "CSPCL";
	print("basesyntax");
	init();
	fputs("\n/* syntax table used when in double quotes */\n", cfile);
	add("\n", "CNL");
	add("\\", "CBACK");
	add("\"", "CENDQUOTE");
	add("`", "CBQUOTE");
	add("$", "CVAR");
	add("}", "CENDVAR");
	/* ':/' for tilde expansion, '-' for [a\-x] pattern ranges */
	add("!*?[=~:/-]", "CCTL");
	print("dqsyntax");
	init();
	fputs("\n/* syntax table used when in single quotes */\n", cfile);
	add("\n", "CNL");
	add("'", "CENDQUOTE");
	/* ':/' for tilde expansion, '-' for [a\-x] pattern ranges */
	add("!*?[=~:/-]\\", "CCTL");
	print("sqsyntax");
	init();
	fputs("\n/* syntax table used when in arithmetic */\n", cfile);
	add("\n", "CNL");
	add("\\", "CBACK");
	add("`", "CBQUOTE");
	add("$", "CVAR");
	add("}", "CENDVAR");
	add("(", "CLP");
	add(")", "CRP");
	print("arisyntax");
	filltable("0");
	fputs("\n/* character classification table */\n", cfile);
	add("0123456789", "ISDIGIT");
	add("abcdefghijklmnopqrstucvwxyz", "ISLOWER");
	add("ABCDEFGHIJKLMNOPQRSTUCVWXYZ", "ISUPPER");
	add("_", "ISUNDER");
	add("#?$!-*@", "ISSPECL");
	print("is_type");
	exit(0);
	/* NOTREACHED */
}



/*
 * Clear the syntax table.
 */

static void
filltable(char *dftval)
{
	int i;

	for (i = 0 ; i < 258; i++)
		syntax[i] = dftval;
}


/*
 * Initialize the syntax table with default values.
 */

static void
init(void)
{
	int ctl;

	filltable("CWORD");
	syntax[0] = "CEOF";
	syntax[1] = "CIGN";
	for (ctl = CTL_FIRST; ctl <= CTL_LAST; ctl++ )
		syntax[130 + ctl] = "CCTL";
}


/*
 * Add entries to the syntax table.
 */

static void
add(char *p, char *type)
{
	while (*p)
		syntax[(signed char)*p++ + 130] = type;
}



/*
 * Output the syntax table.
 */

static void
print(char *name)
{
	int i;
	int col;

	fprintf(hfile, "extern const char %s[];\n", name);
	fprintf(cfile, "const char %s[] = {\n", name);
	col = 0;
	for (i = 0 ; i < 258; i++) {
		if (i == 0) {
			fputs("      ", cfile);
		} else if ((i & 03) == 0) {
			fputs(",\n      ", cfile);
			col = 0;
		} else {
			putc(',', cfile);
			while (++col < 9 * (i & 03))
				putc(' ', cfile);
		}
		fputs(syntax[i], cfile);
		col += strlen(syntax[i]);
	}
	fputs("\n};\n", cfile);
}



/*
 * Output character classification macros (e.g. is_digit).  If digits are
 * contiguous, we can test for them quickly.
 */

static char *macro[] = {
	"#define is_digit(c)\t((unsigned)((c) - '0') <= 9)\n",
	"#define is_alpha(c)\tisalpha((unsigned char)(c))\n",
	"#define is_name(c)\t((c) == '_' || isalpha((unsigned char)(c)))\n",
	"#define is_in_name(c)\t((c) == '_' || isalnum((unsigned char)(c)))\n",
	"#define is_special(c)\t((is_type+SYNBASE)[(signed char)(c)] & (ISSPECL|ISDIGIT))\n",
	NULL
};

static void
output_type_macros(void)
{
	char **pp;

	for (pp = macro ; *pp ; pp++)
		fputs(*pp, hfile);
	fputs("#define digit_val(c)\t((c) - '0')\n", hfile);
}

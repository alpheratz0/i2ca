/*
	Copyright (C) 2022-2023 <alpheratz99@protonmail.com>

	This program is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License version 2 as published by the
	Free Software Foundation.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY
	WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
	FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with
	this program; if not, write to the Free Software Foundation, Inc., 59 Temple
	Place, Suite 330, Boston, MA 02111-1307 USA

*/

#define _XOPEN_SOURCE  500

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#define STB_IMAGE_IMPLEMENTATION
#include "vendor/stb_image.h"

enum case_kind {
	CASE_UPPER,
	CASE_LOWER
};

static void
die(const char *fmt, ...)
{
	va_list args;

	fputs("img2carr: ", stderr);
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	fputc('\n', stderr);
	exit(1);
}

static char *
estrdupwithcase(const char *str, enum case_kind c)
{
	char *res, *p;
	int (*convfn)(int);
	if (NULL == (res = strdup(str)))
		die("OOM");
	convfn = c == CASE_UPPER ? toupper : tolower;
	for (p = res; *p; ++p)
		*p = convfn(*p);
	return res;
}

static void
img2carr(const char *path, const char *varname)
{
	int w, h, i;
	unsigned char *px;
	char *varname_uppercase, *varname_lowercase;

	varname_lowercase = estrdupwithcase(varname, CASE_LOWER);
	varname_uppercase = estrdupwithcase(varname, CASE_UPPER);
	px = stbi_load(path, &w, &h, NULL, 4);

	if (!px)
		die("couldn't load image");

	printf("#define %s_WIDTH  (%d)\n", varname_uppercase, w);
	printf("#define %s_HEIGHT (%d)\n\n", varname_uppercase, h);
	printf("const uint32_t %s_px[%s_WIDTH * %s_HEIGHT] = {", varname_lowercase,
			varname_uppercase, varname_uppercase);

	for (i = 0; i < w * h; ++i) {
		if (i % 5 == 0)
			printf("\n\t/* (y:%3d, x:%3d) */ ", i / w, i % w);
		printf("0x%02x%02x%02x%02x, ",
				px[i*4+3], px[i*4], px[i*4+1], px[i*4+2]);
	}

	printf("\n};\n");

	free(varname_lowercase);
	free(varname_uppercase);
	stbi_image_free(px);
}

static void
usage(void)
{
	puts("usage: img2carr [-hv] [-n variable_name] [image_path]");
	exit(0);
}

static void
version(void)
{
	puts("img2carr version "VERSION);
	exit(0);
}

int
main(int argc, char **argv)
{
	const char *inpath, *varname;

	inpath = varname = NULL;

	while (++argv, --argc > 0) {
		if ((*argv)[0] == '-' && (*argv)[1] != '\0' && (*argv)[2] == '\0') {
			switch ((*argv)[1]) {
			case 'h': usage(); break;
			case 'v': version(); break;
			case 'n': --argc; varname = *++argv; break;
			default: die("invalid option %s", *argv); break;
			}
		} else {
			if (NULL != inpath)
				die("unexpected argument: %s", *argv);
			inpath = *argv;
		}
	}

	if (NULL == inpath)
		die("you must specify an input file");

	if (NULL == varname)
		varname = "image";

	img2carr(inpath, varname);

	return 0;
}

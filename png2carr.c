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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <setjmp.h>
#include <png.h>

static void
die(const char *fmt, ...)
{
	va_list args;

	fputs("png2carr: ", stderr);
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	fputc('\n', stderr);
	exit(1);
}

static void
png2carr(const char *path, const char *varname)
{
	FILE *fp;
	png_struct *png;
	png_info *pnginfo;
	png_byte **rows, bit_depth;
	int16_t y;
	int width, height;
	int i;

	if (NULL == (fp = fopen(path, "rb")))
		die("failed to open file %s: %s", path, strerror(errno));

	if (NULL == (png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL)))
		die("png_create_read_struct failed");

	if (NULL == (pnginfo = png_create_info_struct(png)))
		die("png_create_info_struct failed");

	if (setjmp(png_jmpbuf(png)) != 0)
		die("aborting due to libpng error");

	png_init_io(png, fp);
	png_read_info(png, pnginfo);
	width = png_get_image_width(png, pnginfo);
	height = png_get_image_height(png, pnginfo);

	bit_depth = png_get_bit_depth(png, pnginfo);

	png_set_interlace_handling(png);

	if (bit_depth == 16)
		png_set_strip_16(png);

	if (png_get_valid(png, pnginfo, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png);

	switch (png_get_color_type(png, pnginfo)) {
		case PNG_COLOR_TYPE_RGB:
			png_set_filler(png, 0xff, PNG_FILLER_AFTER);
			break;
		case PNG_COLOR_TYPE_PALETTE:
			png_set_palette_to_rgb(png);
			png_set_filler(png, 0xff, PNG_FILLER_AFTER);
			break;
		case PNG_COLOR_TYPE_GRAY:
			if (bit_depth < 8)
				png_set_expand_gray_1_2_4_to_8(png);
			png_set_filler(png, 0xff, PNG_FILLER_AFTER);
			png_set_gray_to_rgb(png);
			break;
		case PNG_COLOR_TYPE_GRAY_ALPHA:
			png_set_gray_to_rgb(png);
	}

	png_read_update_info(png, pnginfo);

	rows = png_malloc(png, sizeof(png_byte *) * height);

	for (y = 0; y < height; ++y)
		rows[y] = png_malloc(png, png_get_rowbytes(png, pnginfo));

	png_read_image(png, rows);

	printf("const uint32_t %s_width         = %d;\n", varname, width);
	printf("const uint32_t %s_height        = %d;\n", varname, height);
	printf("const uint32_t %s_pixels[%d*%d] = {", varname, width, height);

	for (i = 0; i < width * height; ++i) {
		if (i % 5 == 0)
			printf("\n\t");
		printf("0x%08x, ", rows[i / width][(i % width) * 4 + 3] << 24 |
		                   rows[i / width][(i % width) * 4 + 0] << 16 |
		                   rows[i / width][(i % width) * 4 + 1] <<  8 |
		                   rows[i / width][(i % width) * 4 + 2] <<  0);
	}

	printf("\n};\n");

	for (y = 0; y < height; ++y)
		png_free(png, rows[y]);

	png_free(png, rows);
	png_read_end(png, NULL);
	png_free_data(png, pnginfo, PNG_FREE_ALL, -1);
	png_destroy_info_struct(png, &pnginfo);
	png_destroy_read_struct(&png, NULL, NULL);
	fclose(fp);
}

static void
usage(void)
{
	puts("usage: png2carr [-hv] [-i input] [-n variable_name]");
	exit(0);
}

static void
version(void)
{
	puts("png2carr version "VERSION);
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
				case 'i': --argc; inpath = *++argv; break;
				case 'n': --argc; varname = *++argv; break;
				default: die("invalid option %s", *argv); break;
			}
		} else {
			die("unexpected argument: %s", *argv);
		}
	}

	if (NULL == inpath || NULL == varname)
		die("you must specify an input and a varname");

	png2carr(inpath, varname);

	return 0;
}

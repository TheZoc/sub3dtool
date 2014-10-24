/** main.c
 *
 * This file is part of Sub3dtool
 *
 * Sub3dtool is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Sub3dtool is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Sub3dtool.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <limits.h>
#include <assert.h>
#include <string.h>
#include "utils.h"
#include "subass.h"
#include "subsrt.h"
#include "subsrt_ass.h"
#include "subass3d.h"
#include "global.h"

/* global program_name */
const char * program_name;
const char * version = "0.4.2";
int		debug_mode = 0;
FILE *	error_file = NULL;
#define FORMAT_UNKNOWN	0
#define FORMAT_ASS		1
#define FORMAT_SRT		2
#define CODE_INVALID_ARGS		1
#define CODE_INPUT_ERROR		10
#define CODE_INPFORMAT_ERROR	11
#define CODE_SRT_PARSE_ERROR	15
#define CODE_ASS_PARSE_ERROR	16
#define CODE_OUTPUT_ERROR		20
#define CODE_FUNCTION_ERROR		30

/* print help */
void print_help (FILE * f, int code)
{
	fprintf (f, "%s - version %s\n", program_name, version);
	fprintf (f, "Usage: %s [OPTIONS]... [INPUT]...\n", program_name);
	fprintf (f, "Tool to convert subtitles to 3D format.\n");
	fprintf (f, "\nMandatory options:\n");
	fprintf (f, "  [INPUT]                  Input file, support SRT and ASS format\n");
	fprintf (f, "  -h    --help             Print help\n");
	fprintf (f, "  -o    --output FILE      Output file, support SRT and ASS format\n"); 
	fprintf (f, "                           Output format is determine by extension\n");
	fprintf (f, "                           Default is to use ASS subtitle format\n");
	fprintf (f, "        --debug            Self-test.\n");
	fprintf (f, "\n3D options:\n");
	fprintf (f, "        --3dsbs            Side-By-Side subtitle\n");
	fprintf (f, "        --3dtb             Top-Bottom subtitle\n");
	fprintf (f, "        --no3d             Reverse previously converted subtitle\n");
	fprintf (f, "\nCustomization:\n");
	fprintf (f, "        --screen WxH       Set Play Resolution (subASS). Default: 1920x1080\n");
	fprintf (f, "        --font FONT        Set font name. Default: Freesans\n");
	fprintf (f, "        --fontsize SIZE    Set font size. Default: 64\n");
	fprintf (f, "\nThe following commands set colors of subtitles\n");
	fprintf (f, "  -c    --color         0xRRGGBB   Conventional for Primary & Secondary colors\n");
	fprintf (f, "        --color-primary 0xRRGGBB   Default: 0xFFFFFA\n");
	fprintf (f, "        --color-2nd     0xRRGGBB   Default: 0xFFFFC8\n");
	fprintf (f, "        --color-outline 0xRRGGBB   Default: 0x000000\n");
	fprintf (f, "        --color-back    0xRRGGBB   Default: 0x000000\n");
	fprintf (f, "        --color-force      Ignore colors in input file\n");
	fprintf (f, "\n");
	fprintf (f, "        --border-shadow    Subtitles have outline and drop shadow (default)\n");
	fprintf (f, "        --border-box       Subtitles have blackbox as background\n");
	fprintf (f, "                           Doesn't work well with 3D\n\n");	
	fprintf (f, "        --outline [0-4]    Outline width\n");	
	fprintf (f, "        --shadow [0-4]     Shadow width\n");	
	fprintf (f, "\n");
	fprintf (f, "        --align-left         Set alignments\n");
	fprintf (f, "        --align-center       Default\n");
	fprintf (f, "        --align-right\n");
	fprintf (f, "        --align-bottom       Default\n");
	fprintf (f, "        --align-middle\n");
	fprintf (f, "        --align-top\n");
	fprintf (f, "        --margin-left N      Left margin\n");
	fprintf (f, "        --margin-right N     Right margin\n");
	fprintf (f, "        --margin-vertical N  Vertical margin\n");
	fprintf (f, "\n");
	exit (code);
}

int format_test (const char * path)
{
#define CHECKSIZE 64 
	ZnFile * file = znfile_open (path);
	if (file == NULL)
		return FORMAT_UNKNOWN;
	int scores		[] = { 0, 0 };
	int scores_eqv	[] = { FORMAT_SRT, FORMAT_ASS };
	const char * curline = NULL;
	int i = 0;
	while (i < CHECKSIZE
			&& (curline = znfile_linenext (file)) != NULL)
	{
		if (strstr (curline, "-->") != NULL)
			scores[0] += 16;
		else if (zn_stristr (curline, "Style:") != NULL)
			scores[1] += 16;
		else if (zn_stristr (curline, "Dialogue:") != NULL)
			scores[1] += 16;
		else if (zn_stristr (curline, "[Script Info]") != NULL)
		{
			scores[1] += 64;
			i += 16;
		}
		i++;
	}
	znfile_close (file);
	if (scores[0] > scores[1])
		return scores_eqv[0];
	else
		return scores_eqv[1];
}

void func_test ()
{
	FILE * f = error_file;
	/* Special character */
	fprintf (f, "Special characters\n");
	fprintf (f, "\\n: %d, \\r: %d, \\t: %d.\n", '\n', '\r', '\t');
	fprintf (f, "\\v: %d, \\b: %d, \\f: %d.\n", '\v', '\b', '\f');
	fprintf (f, "\\a: %d, NULL: %p\n", '\a', NULL);
	
	/* subsrt.h */
	ZnsubSRT * sub1 = calloc (1, sizeof (ZnsubSRT));
	fprintf (f, "ZnsubSRT: %ld, %ld, %p, %p.\n",
			sub1->start, sub1->end, sub1->text, (void*)sub1->next);
	free (sub1);
	sub1 = NULL;
	/*const char * tmp = "1\n01:02:03,456 --> 03:02:01,789\n\
First Line.\nSecond Line.\n\n2\n03:04:05,123 --> 05:04:03,321\n\
2nd Event.\n\n3\n06:07:08,234 --> 08:09:10,007\n\
3rd Event.";
*/

	fprintf (f, "FUNCTEST ENDED\n\n\n");
}

int main (int argc, char * argv[])
{
	/* global variables */
	program_name = argv[0];
	debug_mode = 0;
	error_file = stderr;

	/* parsing arguments */
	const char * input = NULL;
	const char * output = NULL;
	int opt = 0;
	int sub3d = 0;
	long   fontsize	= -1;
	char * font		= NULL;
	long long color_primary		= -1;
	long long color_2nd			= -1;
	long long color_outline		= -1;
	long long color_back		= -1;
	int border_style = -1;
	int outline = -1;
	int shadow = -1;
	int align = -1;
	long margin_l = LONG_MIN;
	long margin_r = LONG_MIN;
	long margin_v = LONG_MIN;
	long screen_x = -1;
	long screen_y = -1;
	char * ptr;
	long znsub_srt2ass_flag = 0;
	int	 align_adjust = -1;

	const char * opts_short = "ho:c:";
	const struct option opts_long [] = {
		{ "help",			0, NULL, 'h' },
		{ "output", 		1, NULL, 'o' },
		{ "debug",			0, NULL, 1024 },
		{ "font",			1, NULL, 1030 },
		{ "fontsize",		1, NULL, 1031 },
		{ "screen", 		1, NULL, 1032 },
		{ "color",			1, NULL, 'c' },
		{ "color-primary",	1, NULL, 1041 },
		{ "color-2nd",		1, NULL, 1042 },
		{ "color-outline",	1, NULL, 1043 },
		{ "color-back",		1, NULL, 1044 },
		{ "color-force",	0, NULL, 1045 },
		{ "border-shadow",	0, &border_style, 1},
		{ "border-box",		0, &border_style, 3},
		{ "outline",		1, NULL, 1051 },
		{ "shadow",			1, NULL, 1052 },
		{ "align-left",		0, &align, 1 },
		{ "align-center",	0, &align, 2 },
		{ "align-right",	0, &align, 3 },
		{ "align-bottom",	0, &align_adjust, 0},
		{ "align-middle",	0, &align_adjust, 3},
		{ "align-top",		0, &align_adjust, 6},
		{ "margin-left",	1, NULL, 1061 },
		{ "margin-right",	1, NULL, 1062 },
		{ "margin-vertical", 1, NULL, 1063 },
		{ "3dsbs",	0, &sub3d,	ZNSUB_ASS3D_SBS },
		{ "3dtb",	0, &sub3d,	ZNSUB_ASS3D_TB },
		{ "no3d",	0, &sub3d,	ZNSUB_ASS3D_NO3D },
		{ NULL,	0, NULL, 0 }
	};
	do
	{
		opt = getopt_long (argc, argv, opts_short, opts_long, NULL);
		switch (opt)
		{
		case 'h':
			print_help (stdout, 0);
			break;
		case 'o':
			output = optarg;
			break;
		case 1024:
			debug_mode = 1;
			func_test ();
			break;
		/* font options */
		case 1030:
			zn_strset (&font, optarg);
			break;
		case 1031:
			fontsize = strtol (optarg, NULL, 10);
			break;
		case 1032:
			screen_x = strtol (optarg, &ptr, 10);
			if (ptr[0] != '\0')
			{
				ptr = &ptr[1];
				screen_y = strtol (ptr, NULL, 10);
			}
			else
				screen_x = -1;
			break;
		/* color options */
		case 'c':
			color_2nd = strtoll (optarg, NULL, 0);
			color_primary = color_2nd;
			break;
		case 1041:
			color_primary = strtoll (optarg, NULL, 0);
			break;
		case 1042:
			color_2nd = strtoll (optarg, NULL, 0);
			break;
		case 1043:
			color_outline = strtoll (optarg, NULL, 0);
			break;
		case 1044:
			color_back = strtoll (optarg, NULL, 0);
			break;
		case 1045:
			znsub_srt2ass_flag |= ZNSUB_SRT_ASS_NOCOLOR;
			break;
		/* border styles */
			/* old codes
		case 1050:
			border_style = (int)strtol (optarg, NULL, 0);
			break;
			*/
		case 1051:
			outline = (int) strtol (optarg, NULL, 0);
			break;
		case 1052:
			shadow = (int) strtol (optarg, NULL, 0);
			break;
		/* alignment */
		case 1061:
			margin_l = strtol (optarg, NULL, 0);
			break;
		case 1062:
			margin_r = strtol (optarg, NULL, 0);
			break;
		case 1063:
			margin_v = strtol (optarg, NULL, 0);
			break;
		/* default */
		case '?':
			print_help (stdout, 0);
			break;
		case 0:
		default:
			break;
		}
	} while (opt != -1);

	/* validating arguments */
	if (output == NULL)
		print_help (stdout, 0);
	if (argc - optind > 1)
	{
		fprintf (error_file, "Too much arguments.\n");
		exit (CODE_INVALID_ARGS);
	}
	else if (argc - optind < 1)
	{
		fprintf (error_file, "No input file.\n");
		exit (CODE_INVALID_ARGS);
	}
	/* input */
	input = argv[optind];
	if (input == NULL)
	{
		fprintf (error_file, "Invalid input file.\n");
		exit (CODE_INVALID_ARGS);
	}

	/* open input stream */
	ZnFile * data = znfile_open (input);
	if (data == NULL)
	{
		fprintf (error_file, "Cannot open input file.\n");
		exit (CODE_INPUT_ERROR);
	}
	/* this part check the input format and use the
	 * proper parser to parse it */
	/* parse input */
	ZnsubASS * inp_ass = NULL;
	ZnsubSRT * inp_srt = NULL;
	int		   inp_format = 0;
	inp_format = format_test (input);

	if (inp_format == FORMAT_ASS)
	{
		inp_ass = znsub_ass_parse (data);
		inp_format = FORMAT_ASS;
		if (sub3d == ZNSUB_ASS3D_NO3D)
		{
			znsub_ass3d_discard (inp_ass, 0);
		}
		if (inp_ass == NULL)
		{
			fprintf (error_file, "Parsing ASS subtitle failed.\n");
			exit (CODE_ASS_PARSE_ERROR);
		}
	}
	else if (inp_format == FORMAT_SRT)
	{
		inp_srt = znsub_srt_parse (data);
		inp_format = FORMAT_SRT;
		if (inp_srt == NULL)
		{
			fprintf (error_file, "Parsing SRT subtitle failed.\n");
			exit (CODE_SRT_PARSE_ERROR);
		}
	}
	else
	{
		znfile_close (data);
		fprintf (error_file, "Unable to determine file format.\n");
		exit (CODE_INPFORMAT_ERROR);
	}

	znfile_close (data);

	/* output format decision */
	const char * ext = strrchr (output, '.');
	if (strncmp (ext, ".srt", 4) == 0)
	{
		/* SRT FORMAT */
		ZnsubSRT * out;
		if (inp_format == FORMAT_ASS)
			out = znsub_ass2srt (inp_ass, znsub_srt2ass_flag);
		else if (inp_format == FORMAT_SRT)
			out = inp_srt;
		else
		{
			fprintf (stderr, "Error identifying input format.\n");
			exit (CODE_INPFORMAT_ERROR);
		}
		FILE * file;
#ifdef WIN32
		fopen_s (&file, output, "w");
#else
		file = fopen(output, "w");
#endif // WIN32

		if (file == NULL)
		{
			fprintf (stderr, "Error opening output file.\n");
			if (inp_ass != NULL)
				znsub_ass_free (inp_ass);
			if (inp_srt != NULL)
				znsub_srt_free (inp_srt);
			exit (CODE_OUTPUT_ERROR);
		}
		znsub_srt_tofile (out, file);
		if (inp_ass != NULL)
			znsub_ass_free (inp_ass);
		if (inp_srt != NULL)
			znsub_srt_free (inp_srt);
		if (inp_format != FORMAT_SRT)
			znsub_srt_free (out);
		inp_ass = NULL;
		inp_srt = NULL;
	}
	else {
		/* ASS FORMAT */
		/* convert to ASS */
		ZnsubASS * out = NULL;
		if (inp_format == FORMAT_ASS)
			out = inp_ass;
		else if (inp_format == FORMAT_SRT)
			out = znsub_srt2ass (inp_srt, znsub_srt2ass_flag);
		else
		{
			fprintf (stderr, "Error identifying input format.\n");
			exit (CODE_INPFORMAT_ERROR);
		}
		if (out == NULL)
		{
			fprintf (error_file, "Converting SRT to ASS failed.\n");
			if (inp_ass != NULL)
				znsub_ass_free (inp_ass);
			if (inp_srt != NULL)
				znsub_srt_free (inp_srt);
			exit (CODE_FUNCTION_ERROR);
		}
		/* Customization */
		if (screen_x >= 0 && screen_y >= 0)
		{ /* set play resolution */
			out->play_resx = screen_x;
			out->play_resy = screen_y;
		}
		/* fonts */
		ZnsubASSStyle * style = out->first_style;
		if (fontsize > 0)
			style->font_size = fontsize;
		zn_strset (&style->font_name, font);

		/* colors */
		if (color_primary >= 0)
		{
			znsub_ass_style_color_setll (style->primary_colour,
					color_primary);
		}
		if (color_2nd >= 0)
		{
			znsub_ass_style_color_setll (style->secondary_colour,
					color_2nd);
		}
		if (color_outline >= 0)
		{
			znsub_ass_style_color_setll (style->outline_colour,
					color_outline);
		}
		if (color_back >= 0)
		{
			znsub_ass_style_color_setll (style->back_colour,
					color_back);
		}

		/* border -
		 * style: 1 for drop shadow + outline
		 * style: 2 for black box
		 * outline and shadow is limited to 0-4 according to
		 * specification*/
		style->border_style = (border_style == 1 || border_style == 3 ?
				border_style : style->border_style);
		style->outline = (outline >= 0 && outline <= 4 ?
				outline : style->outline);
		style->shadow = (shadow >= 0 && shadow <= 4 ?
				shadow : style->shadow);

		/* align 
		 * align 1 for left, 2 for center, 3 for right
		 * align_adjust +0 for bottom, +3 for middle, +6 for top */
		align = (align != -1 ? align : 2); /* default - center*/
		align += (align_adjust != -1 ? align_adjust : 0); 
		style->alignment = (align > 0 ? align : style->alignment);

		/* margin: is set? new value else default */
		style->margin_l = (margin_l != LONG_MIN ? margin_l :
				style->margin_l);
		style->margin_r = (margin_r != LONG_MIN ? margin_r :
				style->margin_r);
		style->margin_v = (margin_v != LONG_MIN ? margin_v :
				style->margin_v);


		/* convert to 3d */
		if (sub3d == ZNSUB_ASS3D_SBS || sub3d == ZNSUB_ASS3D_TB)
			znsub_ass3d_convert (out, sub3d, 0);

		/* output to file */
		FILE * file;
#ifdef WIN32
		//errno_t err =
		fopen_s(&file, output, "w");
#else
		file = fopen(output, "w");
#endif // WIN32
		if (file == NULL)
		{
			fprintf (stderr, "Error opening output file.\n");
			if (inp_ass != NULL)
				znsub_ass_free (inp_ass);
			if (inp_srt != NULL)
				znsub_srt_free (inp_srt);
			exit(CODE_OUTPUT_ERROR);
		}

		znsub_ass_tofile (out, file);
		/* free data */
		fclose (file);
		if (inp_format != FORMAT_ASS)
			znsub_ass_free (out);
		if (inp_ass != NULL)
			znsub_ass_free (inp_ass);
		if (inp_srt != NULL)
			znsub_srt_free (inp_srt);
		inp_ass = NULL;
		inp_srt = NULL;
	}

	/* input free */
	if (font != NULL)
		free (font);
	if (inp_ass != NULL)
		znsub_ass_free (inp_ass);
	if (inp_srt != NULL)
		znsub_srt_free (inp_srt);

}


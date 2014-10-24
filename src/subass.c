/** subass.c
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
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"
#include "subass.h"
#include "global.h"



/** ZnsubASSStyle * znsub_ass_style_new ()
 * Create a new style for use with ASSEvent 
 * Note that style->name is NUlL, and must be set
 * manually */
ZnsubASSStyle * znsub_ass_style_new ()
{
#ifndef ZN_NODEBUG
	if (debug_mode != 0)
		fprintf (error_file, "znsub_ass_style_new ()\n");
#endif

	ZnsubASSStyle * style = malloc (sizeof (ZnsubASSStyle));
	style->name = NULL;
	style->font_name = zn_strdup ("Freesans");
	/* color - 
	 * set default colors */
	znsub_ass_style_color_set (style->primary_colour,
			255, 255, 250); /* light */
	znsub_ass_style_color_set (style->secondary_colour,
			255, 255, 200); /* light yellow */
	znsub_ass_style_color_set (style->outline_colour,
			0, 0, 0); 
	znsub_ass_style_color_set (style->back_colour,
			0, 0, 0);
	/* other options -
	 * default options */
	style->font_size = 64;
	style->scale_x = 100;
	style->scale_y = 100;
	style->border_style = 1;
	style->outline = 1;
	style->shadow = 1;
	style->alignment = 2;
	style->margin_l = 10;
	style->margin_r = 10;
	style->margin_v = 0;
	style->id = 0;
	style->next = NULL;
	return style;
}

/* ZnsubASSEvent * znsub_ass_event_new (ZnsubASSStyle * style)
 * new subevent */
ZnsubASSEvent * znsub_ass_event_new (ZnsubASSStyle * style)
{
#ifndef ZN_NODEBUG
	if (debug_mode != 0)
		fprintf (error_file, "znsub_ass_event_new (%p)\n",
				(void*) style);
#endif

	ZnsubASSEvent * event = malloc (sizeof (ZnsubASSEvent));
	event->id	 = 0;
	event->layer = 0;
	event->start = 0;
	event->end	 = 0;
	event->style = style;
	event->text = NULL;
	event->next = NULL;
	return event;
}

/* ZnsubASS * znsub_ass_new ()
 * NOTE:
 * title is NULL (set manually)
 * original_authour is NULL
 */
ZnsubASS * znsub_ass_new ()
{
#ifndef ZN_NODEBUG
	if (debug_mode != 0)
		fprintf (error_file, "znsub_ass_new ()\n");
#endif

	ZnsubASS * sub = malloc (sizeof (ZnsubASS));
	sub->title = NULL;
	sub->original_authour = NULL;
	sub->play_resx = ZNSUB_ASS_DEF_WIDTH;
	sub->play_resy = ZNSUB_ASS_DEF_HEIGHT;
	sub->play_depth = 32;
	sub->timer = 100.0;
	sub->first_event	= NULL;
	sub->last_event		= NULL;
	sub->first_style	= NULL;
	sub->last_style		= NULL;
	return sub;
}

/** uninitialization */
void znsub_ass_style_free (ZnsubASSStyle * style)
{
#ifndef ZN_NODEBUG
	if (debug_mode != 0)
		fprintf (error_file, "znsub_ass_style_free (%p)\n",
				(void*) style);
#endif

	assert (style != NULL);
	if (style->name != NULL)
		free (style->name);
	if (style->font_name != NULL)
		free (style->font_name);
	free (style);
}

/** Note: this do not free the entire chain */
void znsub_ass_event_free (ZnsubASSEvent * event)
{
#ifndef ZN_NODEBUG
	if (debug_mode != 0)
		fprintf (error_file, "znsub_ass_event_free (%p)\n",
				(void*) event);
#endif

	if (event->text != NULL)
		free (event->text);
	free (event);
}

void znsub_ass_free (ZnsubASS * sub)
{
#ifndef ZN_NODEBUG
	if (debug_mode != 0)
		fprintf (error_file, "znsub_ass_free (%p)\n",
				(void*) sub);
#endif

	if (sub->title != NULL)
		free (sub->title);
	if (sub->original_authour != NULL)
		free (sub->original_authour);
	ZnsubASSEvent * cl = sub->first_event;
	ZnsubASSEvent * nl = NULL;
	/* free events */
	while (cl != NULL)
	{
		nl = cl->next;
		znsub_ass_event_free (cl);
		cl = nl;
	}
	/* free styles */
	ZnsubASSStyle * cs = sub->first_style, *ns;
	while (cs != NULL)
	{
		ns = cs->next;
		znsub_ass_style_free (cs);
		cs = ns;
	}
	free (sub);
}


/** int znsub_ass_style_color_set (char *str, int r,g,b,a)
 * assigning color to str */
int znsub_ass_style_color_set (char * str,
		int r, int g, int b)
{
#ifndef ZN_NODEBUG
	if (debug_mode != 0)
		fprintf (error_file,
				"znsub_ass_style_color_set (%p, %d, %d, %d)\n",
				(void*) str, r, g, b);
#endif
	/* Validating input */
	if (r < 0)
		r = 255;
	if (g < 0)
		g = 255;
	if (b < 0)
		b = 255;
	assert(str != NULL);

#ifdef WIN32
	/* format color string */
	_snprintf_s(str,
		ZNSUB_ASS_STYLE_COLORSZ,
		_TRUNCATE,
		"&H%02X%02X%02X",
		b & 0xff,
		g & 0xff,
		r & 0xff);
#else
	/* format color string */
	snprintf (str, ZNSUB_ASS_STYLE_COLORSZ,
			"&H%02X%02X%02X",
			b & 0xff,
			g & 0xff,
			r & 0xff);
#endif // _WIN32

	return 0;
}

int znsub_ass_style_color_setll (char * str, long long color)
{
#ifndef ZN_NODEBUG
	if (debug_mode != 0)
		fprintf (error_file,
				"znsub_ass_style_color_setll (%p, %lld)\n",
				(void*) str, color);
#endif
	assert (str != NULL);
	int b = color & 0xff;
	color = color >> 8;
	int g = color & 0xff;
	color = color >> 8;
	int r = color & 0xff;
	znsub_ass_style_color_set (str, r, g, b);
	return 0;
}

/* int znsub_ass_add_event (ZnsubASS * sub, ZnsubASSEvent * event)
 * add event to sub */
int znsub_ass_add_event (ZnsubASS * sub, ZnsubASSEvent * event)
{
#ifndef ZN_NODEBUG
	if (debug_mode != 0)
		fprintf (error_file,
				"znsub_ass_add_event (%p, %p)\n",
				(void*) sub, (void*) event);
#endif
	if (sub == NULL || event == NULL)
		return -1;
	if (sub->first_event == NULL)
	{
		sub->first_event = event;
		sub->last_event = event;
	}
	else
	{
		sub->last_event->next = event;
		sub->last_event = event;
	}
	return 0;
}

/* int znsub_ass_add_style (ZnsubASS * sub, ZnsubASSStyle * style)
 * add style to sub - need to do this before usage */
int znsub_ass_add_style (ZnsubASS * sub, ZnsubASSStyle * style)
{
#ifndef ZN_NODEBUG
	if (debug_mode != 0)
		fprintf (error_file,
				"znsub_ass_add_style (%p, %p)\n",
				(void*) sub, (void*) style);

#endif

	if (sub == NULL || style == NULL)
		return -1;
	if (sub->first_style == NULL)
	{
		style->id = 1;
		sub->first_style = style;
		sub->last_style = style;
	}
	else
	{
		style->id = sub->last_style->id + 1;
		sub->last_style->next = style;
		sub->last_style = style;
	}
	return 0;
}

int znsub_ass_tofile_time (FILE * file, long time)
{
#ifndef ZN_NODEBUG
	if (debug_mode != 0)
		fprintf (error_file,
				"znsub_ass_tofile_time (%p, %ld)\n",
				(void*) file, time);
#endif

	assert (file != NULL);
	long cts, sec, min, hour;
	cts		= time % 100;
	time	= time / 100;
	sec		= time % 60;
	time	= time / 60;
	min		= time % 60;
	hour	= time / 60;
	fprintf (file, "%ld:%02ld:%02ld.%02ld", hour, min, sec, cts);
	return 0;
}

int znsub_ass_tofile (ZnsubASS * sub, FILE * file)
{
#ifndef ZN_NODEBUG
	if (debug_mode != 0)
		fprintf (error_file,
				"znsub_ass_tofile (%p, %p)\n",
				(void*) sub, (void*) file);
#endif

	if (sub == NULL || file == NULL)
		return -1;
	/* Sub info */
	fprintf (file, "[Script Info]\n");
	fprintf (file, "Title: %s\n", sub->title);
	fprintf (file, "Original Script: %s\n",
			sub->original_authour);
	fprintf (file, "Update Details: None\n");
	fprintf (file, "ScriptType: v4.00+\n");
	fprintf (file, "Collisions: Normal\n");
	fprintf (file, "PlayResX: %ld\n", sub->play_resx);
	fprintf (file, "PlayResY: %ld\n", sub->play_resy);
	fprintf (file, "PlayDepth: %d\n", sub->play_depth);
	fprintf (file, "Timer: %.4f\n", sub->timer);

	/* Style info */
	fprintf (file, "\n[v4+ Styles]\n");
	fprintf (file, "Format: Name, Fontname, Fontsize, ");
	fprintf (file, "PrimaryColour, SecondaryColour, ");
	fprintf (file, "OutlineColour, BackColour, ");
	fprintf (file, "Bold, Italic, Underline, StrikeOut, "); /* unimp */
	fprintf (file, "ScaleX, ScaleY, ");
	fprintf (file, "Spacing, Angle, "); /* unimp */
	fprintf (file, "BorderStyle, Outline, Shadow, ");
	fprintf (file, "Alignment, MarginL, MarginR, MarginV, Encoding\n");

	ZnsubASSStyle * s = sub->first_style;
	while (s != NULL)
	{
		fprintf (file, "Style: %s,%s,%ld,",
				s->name, s->font_name, s->font_size);
		fprintf (file, "%s,%s,%s,%s,",
				s->primary_colour, s->secondary_colour,
				s->outline_colour, s->back_colour);
		fprintf (file, "0,0,0,0,"); /* unimp */
		fprintf (file, "%d,%d,", s->scale_x, s->scale_y);
		fprintf (file, "0,0,"); /* unimp */
		fprintf (file, "%d,%d,%d,",
				s->border_style, s->outline, s->shadow);
		fprintf (file, "%d,%ld,%ld,%ld,",
				s->alignment, s->margin_l, s->margin_r, s->margin_v);
		fprintf (file, "0\n"); /* unimp */
		s = s->next;
	}

	/* Events */
	fprintf (file, "\n[Events]\n");
	fprintf (file, "Format: Layer, Start, End, Style, Name,");
	fprintf (file, "MarginL, MarginR, MarginV, Effect, Text\n");
	ZnsubASSEvent * e = sub->first_event;
	while (e != NULL)
	{
		assert (e->style != NULL);
		assert (e->style->name != NULL);
		assert (e->text != NULL);
		fprintf (file, "Dialogue: %d,", e->layer);
		znsub_ass_tofile_time(file, e->start);
		fprintf (file, ",");
		znsub_ass_tofile_time (file, e->end);
		if (e->style == NULL)
			fprintf (file, ",%s,", "Default");
		else
			fprintf (file, ",%s,", e->style->name);
		fprintf (file, ",0,0,0,,");
		fprintf (file, "%s\n", e->text);
		e = e->next;
	}

	return 0;
}

void znsub_ass_parse_scriptinfo (ZnsubASS * sub, ZnFile * file)
{
#ifndef ZN_NODEBUG
	if (debug_mode != 0)
		fprintf (error_file,
				"znsub_ass_parse_scriptinfo (%p, %p)\n",
				(void*)sub, (void*) file);
#endif
	/* [Script Info] */
	const char * line_cur = znfile_linenext (file);
	const char * ptr;
	while (line_cur != NULL)
	{
		/* skip leading space */
		ptr = &line_cur[strspn (line_cur, " \t\n\r")];
		/* check input */
		if (zn_strincmp (ptr, "Title:", 6) == 0)
		{
			zn_strset (&sub->title, &ptr[6]);
		}
		else if (zn_strincmp (ptr,"Original Script:", 16) == 0)
		{
			zn_strset (&sub->original_authour, &ptr[16]);
		}
		else if (zn_strincmp (ptr, "PlayResX:", 9) == 0)
		{
			sub->play_resx = strtol (&ptr[9], NULL, 0);
		}
		else if (zn_strincmp (ptr, "PlayResY:", 9) == 0)
		{
			sub->play_resy = strtol (&ptr[9], NULL, 0);
		}
		else if (zn_strincmp (ptr, "PlayDepth:", 10) == 0)
		{
			sub->play_depth = strtol (&ptr[10], NULL, 0);
		}
		else if (zn_strincmp (ptr, "Timer:", 6) == 0)
		{
			sub->timer = strtod (&ptr[6], NULL);
		}
		else if (ispunct (ptr[0]) && ptr[0] != '\0')
		{ /* NON Alpha Numeric character */
			break;
		}
		/* read next line */
		line_cur = znfile_linenext (file);
	}
}

void znsub_ass_parse_styles (ZnsubASS * sub, ZnFile * file)
{
#ifndef ZN_NODEBUG
	if (debug_mode != 0)
		fprintf (error_file, "znsub_ass_parse_styles (%p, %p)\n",
				(void*)sub, (void*) file);
#endif
	/* [v4 Styles+] */
#define FORMAT_MAX	64
#define FORMAT_SIZE	17
	const char * ptr;
	int formated = 0;
	int * format = calloc (sizeof (int), FORMAT_MAX);
	const char * fmt_names [FORMAT_SIZE] = {
		"Name",
		"Fontname",
		"Fontsize",
		"PrimaryColour",
		"SecondaryColour",
		"TertiaryColour",
		"OutlineColour",
		"BackColour",
		"ScaleX",
		"ScaleY",
		"BorderStyle",
		"Outline",
		"Shadow",
		"Alignment",
		"MarginL",
		"MarginR",
		"MarginV" };
	const int fmt_codes [FORMAT_SIZE] = {
		1, 2, 3,
		4, 5, 6,
		6, 7, 8,
		9, 10, 11,
		12, 13, 14,
		15, 16 };
	int fmt_lengths [FORMAT_SIZE];
	for (int i = 0; i < FORMAT_SIZE; i++)
		fmt_lengths[i] = strlen (fmt_names[i]);

	const char * line_cur = znfile_linenext (file);
	ptr = line_cur;
	while (line_cur != NULL)
	{
		/* skip leading space */
		ptr = line_cur;
		ptr = &ptr[strspn (ptr, " \t\n\r")];
		if (zn_strincmp (ptr, "Format:", 7) == 0)
		{
			/* this part read Format line, and save its
			 * construction to format, so it could be used
			 * later when reading Style: line */
			/* set format */
			formated = 1;
			ptr = &ptr[7];
			int i = 0;
			while (i < FORMAT_MAX)
			{
				/* skip leading space */
				ptr = &ptr[strspn (ptr, " \t")];
				format[i] = -1;
				for (int x = 0; x < FORMAT_SIZE; x++)
				{
					/* check for valid names and assign
					 * to format */
					if (zn_strincmp (ptr,
								fmt_names[x],
								fmt_lengths[x]) == 0)
					{
						format[i] = fmt_codes[x];
						break;
					}
				}
				ptr = strchr (ptr, ',');
				if (ptr != NULL)
					ptr = &ptr[1];
				else
					break;
				if (ptr[0] == '\0') break;
				i++;
			}
		}
		else if (zn_strincmp (ptr, "Style:", 6) == 0
				&& formated != 0)
		{ /* Style parsing */
			/* skip leading space */
			ptr = &ptr[6];
			int i = 0;
			const char * ptr2;
			/* Reading Style according to format */
			ZnsubASSStyle * style = znsub_ass_style_new ();
			while (i < FORMAT_MAX && format[i] != 0)
			{
				/* skip leading space */
				ptr = &ptr[strspn (ptr, " \t")];
				size_t len = strlen (ptr);

				ptr2 = strchr (ptr, ',');
				if (ptr2 == NULL)
					ptr2 = &ptr[len];

				len = ptr2 - ptr;
				char * ptr3;
				long n = 0;
				switch (format[i])
				{ /* reading differs for each type */
					case 1:
						ptr3 = malloc (ptr2 - ptr + 1);
						strncpy (ptr3, ptr, len);
						if (style->name != NULL)
							free (style->name);
						ptr3[len] = '\0';
						style->name = ptr3;
						break;
					case 2:
						ptr3 = malloc (ptr2 - ptr + 1);
						strncpy (ptr3, ptr, len);
						if (style->font_name != NULL)
							free (style->font_name);
						ptr3[len] = '\0';
						style->font_name = ptr3;
						break;
					case 3:
						n = strtol (ptr, NULL, 0);
						style->font_size = n > 0 ?
							n : style->font_size;
						break;
					case 4:
						n = len >= ZNSUB_ASS_STYLE_COLORSZ ?
							ZNSUB_ASS_STYLE_COLORSZ -1 : len;
						strncpy (style->primary_colour, ptr, n);
						break;
					case 5:
						n = len >= ZNSUB_ASS_STYLE_COLORSZ ?
							ZNSUB_ASS_STYLE_COLORSZ -1 : len;
						strncpy (style->secondary_colour, ptr, n);
						break;
					case 6:
						n = len >= ZNSUB_ASS_STYLE_COLORSZ ?
							ZNSUB_ASS_STYLE_COLORSZ -1 : len;
						strncpy (style->outline_colour, ptr, n);
						break;
					case 7:
						n = len >= ZNSUB_ASS_STYLE_COLORSZ ?
							ZNSUB_ASS_STYLE_COLORSZ -1 : len;
						strncpy (style->back_colour, ptr, n);
						break;
					case 8:
						n = strtol (ptr, NULL, 0);
						style->scale_x = n > 0 ?
							n : style->scale_x;
						break;
					case 9:
						n = strtol (ptr, NULL, 0);
						style->scale_y = n > 0 ?
							n : style->scale_y;
						break;
					case 10:
						n = strtol (ptr, NULL, 0);
						style->border_style = n > 0 ?
							n : style->border_style;
						break;
					case 11:
						n = strtol (ptr, NULL, 0);
						style->outline = n > 0 ?
							n : style->outline;
						break;
					case 12:
						n = strtol (ptr, NULL, 0);
						style->shadow = n > 0 ?
							n : style->shadow;
						break;
					case 13:
						n = strtol (ptr, NULL, 0);
						style->alignment = n > 0 ?
							n : style->alignment;
						break;
					case 14:
						style->margin_l = strtol (ptr, NULL, 0);
						break;
					case 15:
						style->margin_r = strtol (ptr, NULL, 0);
						break;
					case 16:
						style->margin_v = strtol (ptr, NULL, 0);
						break;
					default:
						break;
				}
				if (ptr2[0] != '\0')
					ptr = &ptr2[1];
				else
					break;
				i++;
			}
			znsub_ass_add_style (sub, style);
		}
		else if (ispunct (ptr[0]) && ptr[0] != '\0')
		{ /* NON Alpha Numeric character */
			/* This signal a new section */
			break;
		}
		line_cur = znfile_linenext (file);
	}
	free (format);
#undef FORMAT_MAX
#undef FORMAT_SIZE 
}

long znsub_ass_parse_time (const char * str)
{
#ifndef ZN_NODEBUG
	if (debug_mode != 0)
		fprintf (error_file, "znsub_ass_parse_time (PTR)\n");
#endif
	long t = 0;
	char * ptr;
	t = strtol (str, &ptr, 10);
	if (ptr[0] != '\0')
		ptr = &ptr[1];
	t = strtol (ptr, &ptr, 10) + t*60;
	if (ptr[0] != '\0')
		ptr = &ptr[1];
	t = strtol (ptr, &ptr, 10) + t*60;
	if (ptr[0] != '\0')
		ptr = &ptr[1];
	t = strtol (ptr, &ptr, 10) + t*100;
	return t;
}

ZnsubASSStyle * znsub_ass_style_find (ZnsubASS * sub,
		const char * name)
{
	ZnsubASSStyle * style = sub->first_style;
	while (style != NULL)
	{
		if (strcmp (style->name, name) == 0)
			return style;
		style = style->next;
	}
	return NULL;
}

void znsub_ass_parse_events (ZnsubASS * sub, ZnFile * file)
{
#ifndef ZN_NODEBUG
	if (debug_mode != 0)
		fprintf (error_file, "znsub_ass_parse_events (%p, %p)\n",
				(void*)sub, (void*) file);
#endif
#define FORMAT_MAX	64
#define FORMAT_SIZE	5
	const char * ptr;
	int formated = 0;
	int * format = calloc (sizeof (int), FORMAT_MAX);
	const char * fmt_names [FORMAT_SIZE] = {
		"Layer",
		"Start",
		"End",
		"Style",
		"Text" };

	const int fmt_codes [FORMAT_SIZE] = {
		1, 2, 3, 4, 5 };
	int fmt_lengths [FORMAT_SIZE];
	for (int i = 0; i < FORMAT_SIZE; i++)
		fmt_lengths[i] = strlen (fmt_names[i]);

	const char * line_cur = znfile_linenext (file);
	ptr = line_cur;
	while (line_cur != NULL)
	{
		/* skip leading space */
		ptr = line_cur;
		ptr = &ptr[strspn (ptr, " \t\n\r")];
		if (zn_strincmp (ptr, "Format:", 7) == 0)
		{
			/* This part is exactly the same as parsing style format
			 * It's actually copy and paste */
			formated = 1;
			ptr = &ptr[7];
			int i = 0;
			while (i < FORMAT_MAX)
			{
				/* skip leading space */
				ptr = &ptr[strspn (ptr, " \t")];
				format[i] = -1;
				for (int x = 0; x < FORMAT_SIZE; x++)
				{
					/* check for valid names and assign
					 * to format */
					if (zn_strincmp (ptr,
								fmt_names[x],
								fmt_lengths[x]) == 0)
					{
						format[i] = fmt_codes[x];
						break;
					}
				}
				ptr = strchr (ptr, ',');
				if (ptr != NULL)
					ptr = &ptr[1];
				else
					break;
				if (ptr[0] == '\0') break;
				i++;
			}
		}
		else if (zn_strincmp (ptr, "Dialogue:", 9) == 0
				&& formated != 0)
		{ /* Style parsing */
			ptr = &ptr[9];
			int i = 0;
			const char * ptr2;
			/* Reading Style according to format */
			ZnsubASSEvent * event = znsub_ass_event_new (NULL);
			while (i < FORMAT_MAX && format[i] != 0)
			{
				/* skip leading space */
				ptr = &ptr[strspn (ptr, " \t")];
				size_t len = strlen (ptr);

				ptr2 = strchr (ptr, ',');
				if (ptr2 == NULL)
					ptr2 = &ptr[len];

				len = ptr2 - ptr;
				char * ptr3;
				/*long n = 0;*/
				switch (format[i])
				{
				case 1:
					event->layer = strtol (ptr, NULL, 0);
					break;
				case 2:
					event->start = znsub_ass_parse_time (ptr);
					break;
				case 3:
					event->end = znsub_ass_parse_time (ptr);
					break;
				case 4:
					ptr3 = malloc (ptr2 - ptr + 1);
					strncpy (ptr3, ptr, len);
					ptr3[len] = '\0';
					event->style = znsub_ass_style_find (sub, ptr3);
					free (ptr3);
					break;
				case 5:
					if (event->text != NULL)
						free (event->text);
					event->text = zn_strdup (ptr);
					break;
				default:
					break;
				}
				if (ptr2[0] != '\0')
					ptr = &ptr2[1];
				else
					break;
				i++;
			}
			znsub_ass_add_event (sub, event);
		}
		else if (ispunct (ptr[0]) && ptr[0] != '\0')
		{ /* NON Alpha Numeric character */
			/* This signal a new section */
			break;
		}
		line_cur = znfile_linenext (file);
	}
	free (format);
#undef FORMAT_MAX
#undef FORMAT_SIZE 
}

/* znsub_ass_parse */
ZnsubASS * znsub_ass_parse (ZnFile * file)
{
#ifndef ZN_NODEBUG
	if (debug_mode != 0)
		fprintf (error_file, "znsub_ass_parse (%p)\n",
				(void*) file);
#endif
#define SECTION_SCRIPT_INFO		1
#define SECTION_STYLES			2
#define SECTION_EVENTS			3
	ZnsubASS *	 sub = znsub_ass_new ();
	const char * line_cur = znfile_linenext (file);
	int section = 0;

	while (line_cur != NULL)
	{
		/* Section selection */
		while (line_cur != NULL)
		{
			if (zn_stristr (line_cur, "[Script Info]") != NULL)
			{
				section  = SECTION_SCRIPT_INFO;
				break;
			}
			else if (zn_stristr (line_cur, "[v4 Styles]") != NULL
				|| zn_stristr (line_cur, "[v4+ Styles]") != NULL
				|| zn_stristr (line_cur, "[v4 Styles+]") != NULL)
			{
				section  = SECTION_STYLES;
				break;
			}
			else if (zn_stristr (line_cur, "[Events]") != NULL)
			{
				section  = SECTION_EVENTS;
				break;
			}
			line_cur = znfile_linenext (file);
		}
		/* section reading */
		if (section == SECTION_SCRIPT_INFO)
		{
			znsub_ass_parse_scriptinfo (sub, file);
			line_cur = znfile_linecurrent (file);
			if (file->error != EOF && line_cur != NULL)
				continue;
		}
		else if (section == SECTION_STYLES)
		{
			znsub_ass_parse_styles (sub, file);
			line_cur = znfile_linecurrent (file);
			if (file->error != EOF && line_cur != NULL)
				continue;
		}
		else if (section == SECTION_EVENTS)
		{
			znsub_ass_parse_events (sub, file);
			line_cur = znfile_linecurrent (file);
			if (file->error != EOF && line_cur != NULL)
				continue;
		}
		line_cur = znfile_linenext (file);
	}
	return sub;
#undef SECTION_SCRIPT_INFO	
#undef SECTION_STYLES
#undef SECTION_EVENTS
}



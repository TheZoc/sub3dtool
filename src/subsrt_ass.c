/** subsrt2ass.c
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
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "utils.h"
#include "subsrt.h"
#include "subass.h"
#include "subsrt_ass.h"
#include "global.h"

/** ZnsubASS * znsub_srt2ass (ZnsubSRT * srt)
 * convert sub to ASS format */
ZnsubASS * znsub_srt2ass (ZnsubSRT * srt, unsigned long flag)
{
#ifndef ZN_NODEBUG
	if (debug_mode != 0)
		fprintf (error_file, "znsub_srt2ass (%p, %ld)\n",
				(void*)srt, flag);
#endif

	if (srt == NULL)
		return NULL;
	ZnsubASS		* ass = znsub_ass_new ();
	ZnsubASSStyle	* dfs = znsub_ass_style_new ();
	ZnsubASSEvent * next;
	/* default value */
	zn_strset (&ass->title, "Unknown");
	zn_strset (&ass->original_authour, "Unknown");
	zn_strset (&dfs->name, "DefaultStyle");

	znsub_ass_add_style (ass, dfs);
	
	/* enter loop */
	ZnsubSRT * cl = srt;
	while (cl != NULL)
	{
		/* ASS uses centiseconds (10 miliseconds) */
		next = znsub_ass_event_new (dfs);
		next->start	= cl->start / 10;
		next->end	= cl->end / 10;
		next->text	= znsub_srt2ass_text (cl->text, flag);
		next->id	= cl->id;
		znsub_ass_add_event (ass, next);
		cl = cl->next;
	}

	return ass;
}

ZnsubSRT * znsub_ass2srt (ZnsubASS * ass, unsigned long flag)
{
#ifndef ZN_NODEBUG
	if (debug_mode != 0)
		fprintf (error_file, "znsub_ass2srt (%p, %ld)\n",
				(void*) ass, flag);
#endif
	/* flag is unused, place holder */
	assert (ass != NULL);
	ZnsubSRT * first = NULL, * cur, * next;
	ZnsubASSEvent * aloop = ass->first_event;
	long id = 0;

	while (aloop != NULL)
	{
		next = znsub_srt_new ();
		next->id = id++;
		/* Set timer, adjusted by Timer */
		next->start	= (long)(aloop->start * 10 * ass->timer / 100);
		next->end	= (long)(aloop->end * 10 * ass->timer / 100);
		next->text	= znsub_ass2srt_text (aloop->style,
				aloop->text, flag);
		if (first == NULL)
		{
			first = next;
			cur = first;
		}
		else {
			cur->next = next;
			cur = next;
		}
		aloop = aloop->next;
	}

	return first;
}

/** char * znsub_srt2ass_text (const char * str)
 * convert format tags (<i>) from srt to ass style ({\i1})*/
char * znsub_srt2ass_text (const char * str, unsigned long flag)
{
	assert (str != NULL);

	ZnStream * out = znstr_new ();
	long len = strlen (str);
	long end2 = len - 2;
	long end3 = len - 3;
	/*long last = 0;*/
	long i = 0;
	long nofont = flag & ZNSUB_SRT_ASS_NOCOLOR;
	const char * ptr;
	while (i < len)
	{
		if (str[i] == '\n')
		{
			znstr_printf (out, "\\n\\N");
		}
		else if (i < end2 && str[i] == '<' && str[i+1] != '/')
		{
			/* processing tag */
			/* rudimentary methods */
			ptr = &str[i];
			char c = tolower (str[i+1]);
			if (!isalnum(str[i+2]) &&
					(c == 'i' ||
					 c == 'b' ||
					 c == 'u'))
			{
				znstr_printf (out, "{\\%c1}", c);
				do
				{
					i++;
				} while (str[i] != '>' && i < len);
			}
			else if (strncmp (ptr, "<font", 5) == 0)
			{
				/* skip to the end tag > */
				do
				{
					i++;
				} while (str[i] != '>' && i < len);
				/* check nofont flag */
				if (nofont == 0)
				{
					char * fclr = NULL;
					fclr = strstr(ptr, "color=");
					if (fclr != NULL && fclr - str < i)
					{ /* check if the option is valid and contained */
						if (fclr[6] != '\0')
							fclr = &fclr[6]; /* skip to end */
						if (fclr[0] != '\0') /* skip " */
							fclr = &fclr[1];
						if (fclr[0] != '\0') /* skip # */
							fclr = &fclr[1];
						long color = strtol (fclr, NULL, 16);
						int count = 0;
						char c = fclr[0];
						while (isxdigit (c))
						{
							count++; /* counting hexa character */
							c = fclr[count];
						}
						int r = 0, g = 0, b = 0;
						if (count > 3)
						{
							/* 6 characters format */
							b = color % 256;
							color /= 256;
							g = color % 256;
							color /= 256;
							r = color % 256;
						}
						else
						{
							/* 3 characters format */
							b = color % 16;
							b = b*16 + b; /* convert to 0-255 scale */
							color /= 16;
							g = color % 16;
							g = g*16 + g;
							color /= 16;
							r = color % 16;
							r = r*16 + r;
						}
						znstr_printf (out, "{\\c&H%02X%02X%02X&}",
								b ,g , r);
					}
				}

			}
		}
		else if (i < end3 && str[i] == '<' && str[i+1] == '/')
		{
			/* processing tag */
			/* rudimentary methods */
			ptr = &str[i];
			ptr = &str[i];
			char c = tolower (str[i+2]);
			if (!isalnum(str[i+3]) &&
					(c == 'i' ||
					 c == 'b' ||
					 c == 'u'))
			{
				znstr_printf (out, "{\\%c0}", c);
				do
				{
					i++;
				} while (str[i] != '>' && i < len);
			}
			else if (strncmp (ptr, "</font", 6) == 0)
			{
				/* closing font color option */
				if (nofont == 0)
					znstr_printf (out, "{\\c}");
				do
				{
					i++;
				} while (str[i] != '>' && i < len);
			}
		}
		else {
			/* TODO: very inefficient, need improvement */
			znstr_printf (out, "%c", str[i]);

		}
		i++;
	}
	return znstr_close (out);
}

char * znsub_ass2srt_text (ZnsubASSStyle * style,
		const char * str, unsigned long flag)
{
	ZnStream * out = znstr_new ();
	long len = strlen (str);
	long end1	= len - 1;
	long end2	= len - 2;
	/*long last = 0;*/
	long i = 0;
	long noclr = flag & ZNSUB_SRT_ASS_NOCOLOR;
	const char * ptr;
	ptr = &style->primary_colour[2];
	long long color = strtoll (ptr, NULL, 16);
	int r = color & 0xff;
	color = color >> 8;
	int g = color & 0xff;
	color = color >> 8;
	int b = color & 0xff;
	int newline = 0;
	if (noclr == 0)
	{
		znstr_printf (out,
				"<font color=\"#%02X%02X%02X\">",
				r, g, b);
	}
	while (i < len)
	{
		if (i < end1 && str[i] == '\\' && str[i+1] == 'n')
		{
			if (newline == 0)
			{
				newline++;
				znstr_printf (out, "\n");
			}
			i++;
		}
		else if (i < end1 && str[i] == '\\' && str[i+1] == 'N')
		{
			if (newline == 0)
			{
				newline++;
				znstr_printf (out, "\n");
			}
			i++;
		}
		else if (str[i] == '{')
		{
			do
			{
				char c = tolower (str[i+1]);
				if (str[i] == '\\' && (
						c == 'i' ||
						c == 'b' ||
						c == 'u' || 
						c == 'c'))
				{
					if (c == 'c')
					{
						if (i < end2 - 2 &&
								str[i+2] == '&' && noclr == 0)
						{
							ptr = &str[i+4];
							color = strtoll (ptr, NULL, 16);
							r = (int) (color | 0xff);
							color = color >> 8;
							g = (int) (color | 0xff);
							color = color >> 8;
							b = (int) (color | 0xff);
							znstr_printf (out,
								"<font color=\"#%02X%02X%02X\">",
								r, g, b);
						}
						else if (noclr == 0)
						{
							znstr_printf (out, "</font>");
						}
					}
					else if (str[i+2] == '0')
						znstr_printf (out, "</%c>", c);
					else
						znstr_printf (out, "<%c>", c);
				}
				i++;
			} while (str[i] != '}' && i < end2);
		}
		else {
			/* TODO: very inefficient, need improvement */
			newline = 0;
			znstr_printf (out, "%c", str[i]);
		}
		i++;
	}
	if (noclr == 0)
		znstr_printf (out, "</font>");
	return znstr_close (out);
}



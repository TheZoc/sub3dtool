/** subsrt.c
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
#include <assert.h>
#include "subsrt.h"
#include "global.h"
#include "utils.h"

/** ZnsubSRT
 * SRT sub class */
/** ZnsubSRT * znsub_srt_new ()
 * initialize ZnsubSRT */
ZnsubSRT * znsub_srt_new ()
{
#ifndef ZN_NODEBUG
	if (debug_mode != 0)
		fprintf (error_file, "znsub_srt_new ()\n");
#endif
	return calloc (1, sizeof (ZnsubSRT));
}

/** void znsub_srt_free (ZnsubSRT * sub)
 * uninitialize ZnsubSRT */
void znsub_srt_free (ZnsubSRT * sub)
{
#ifndef ZN_NODEBUG
	if (debug_mode != 0)
		fprintf (error_file, "znsub_srt_free (%p)\n", (void*) sub);
#endif
	ZnsubSRT * sc = sub;
	ZnsubSRT * sn = NULL;
	while (sc != NULL)
	{
		if (sc->text != NULL)
			free (sc->text);
		sn = sc->next;
		free (sc);
		sc = sn;
	}
}

/* parse_time */
long znsub_srt_parse_time (const char * str)
{
#ifndef ZN_NODEBUG
	if (debug_mode != 0)
		fprintf (error_file, "znsub_srt_parse_time (PTR)\n");
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
	t = strtol (ptr, &ptr, 10) + t*1000;
	return t;
}

/* parse file */
ZnsubSRT * znsub_srt_parse (ZnFile * file)
{
#ifndef ZN_NODEBUG
	if (debug_mode != 0)
		fprintf (error_file, "znsub_srt_parse (%p)\n", (void*)file);
#endif
	ZnsubSRT *first = NULL, *cur, *next;
	const char * line;
	const char * ptr;
	long id = 1;
	while ((line = znfile_linenext (file)))
	{
		if ((ptr = strstr (line, "-->")) != NULL)
		{
			next = znsub_srt_new ();
			next->id	= id++;
			next->start = znsub_srt_parse_time (line);
			next->end	= znsub_srt_parse_time (&ptr[3]);
			/* parse text */
			ZnStream * str = znstr_new ();
			if ((line = znfile_linenext (file)) &&
					line[0] != '\0')
			{
				znstr_printf (str, "%s", line);
				while ((line = znfile_linenext (file)) &&
						line[0] != '\0')
				{
					znstr_printf (str, "\n%s", line);
				}
			}
			next->text = znstr_close (str);
			if (first == NULL)
			{
				first = next;
				cur = first;
			}
			else {
				cur->next = next;
				cur	= next;
			}
		}
	}
	return first;
}

int znsub_srt_tofile_time (FILE * file, long time)
{
#ifndef ZN_NODEBUG
	if (debug_mode != 0)
		fprintf (error_file, "znsub_srt_tofile_time (%p, %ld)\n",
				(void*)file, time);
#endif
	assert (file != NULL);
	int ms = time % 1000;
	time /= 1000;
	int s = time % 60;
	time /= 60;
	int m = time % 60;
	time /= 60;
	int h = time % 60;
	fprintf (file, "%02d:%02d:%02d,%03d", h, m, s, ms);
	return 0;
}

/* file output */
int znsub_srt_tofile (ZnsubSRT * sub, FILE * file)
{
#ifndef ZN_NODEBUG
	if (debug_mode != 0)
		fprintf (error_file, "znsub_srt_parse (%p, %p)\n",
				(void*) sub, (void*)file);
#endif
	assert (file != NULL);
	ZnsubSRT * cur = sub;
	long id = 1;
	while (cur != NULL)
	{
		fprintf (file, "%ld\n", id++);
		znsub_srt_tofile_time (file, cur->start);
		fprintf (file, " --> ");
		znsub_srt_tofile_time (file, cur->end);
		fprintf (file, "\n%s\n\n", cur->text);
		cur = cur->next;
	}
	return 0;
}

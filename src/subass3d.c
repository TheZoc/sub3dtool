/** subass3d.c
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
#include "utils.h"
#include "subass.h"
#include "subass3d.h"
#include "global.h"


int znsub_ass3d_convert (ZnsubASS * sub, int mode, unsigned long flag)
{
#ifndef ZN_NODEBUG
	if (debug_mode != 0)
		fprintf (error_file, "znsub_ass3d_convert (%p, %d, %ld)\n",
				(void*)sub, mode, flag);
#endif

	long margin;
	if (mode == ZNSUB_ASS3D_TB)
	{
		/* sub->play_resx = 0; */
		if (sub->play_resy <= 0)
			sub->play_resy = ZNSUB_ASS_DEF_HEIGHT;
		margin = sub->play_resy / 2;
		sub->play_resx = (sub->play_resx >= 0 ? sub->play_resx : 0);
	}
	else
	{
		/* sub->play_resy = 0; */
		if (sub->play_resx <= 0)
			sub->play_resx = ZNSUB_ASS_DEF_WIDTH;
		margin = sub->play_resx / 2;
		sub->play_resy = (sub->play_resy >= 0 ? sub->play_resy : 0);
	}
	/* cloning styles */
	ZnsubASSStyle * ls = sub->first_style, * ns;
	while (ls != NULL)
	{
		ns = malloc (sizeof (ZnsubASSStyle));
		memcpy (ns, ls, sizeof (ZnsubASSStyle));
		/* copy font_name */
		ns->font_name = zn_strdup (ls->font_name);

		/* change name */
		long len	= strlen (ls->name) + 3; /* _LE, _RI */
		char * name = ls->name;
		ns->name = malloc (len + 1);
		ls->name = malloc (len + 1);
#ifdef WIN32
		_snprintf_s(ns->name, len + 1, _TRUNCATE, "%s_Ri", name);
		_snprintf_s(ls->name, len + 1, _TRUNCATE, "%s_Le", name);
#else
		snprintf(ns->name, len + 1, "%s_Ri", name);
		snprintf(ls->name, len + 1, "%s_Le", name);
#endif // WIN32
		free (name);

		/* change margin */
		if (mode == ZNSUB_ASS3D_TB)
		{
			ns->margin_v = ns->margin_v/2 + margin;
			ls->margin_v = ls->margin_v/2;
			ns->scale_y /= 2;
			ls->scale_y /= 2;
		}
		else
		{
			ns->margin_l = ns->margin_l/2 + margin;
			ns->margin_r = ns->margin_r/2;
			ls->margin_r = ls->margin_r/2 + margin;
			ls->margin_l = ls->margin_l/2;
			ns->scale_x /= 2;
			ls->scale_x /= 2;
		}
		/* change id */
		ns->id = ls->id;
		ls->id = -ls->id;

		/* linking, moving to next item */
		ns->next = ls->next;
		ls->next = ns;
		ls = ns->next;
	}

	/* cloning events */
	ZnsubASSEvent * le = sub->first_event, * ne;
	while (le != NULL)
	{
		ne = malloc (sizeof (ZnsubASSEvent));
		memcpy (ne, le, sizeof (ZnsubASSEvent));
		/* clone text, set style */
		ne->text = zn_strdup (le->text);
		ne->style = le->style->next;

		/* linking, moving on */
		ne->next = le->next;
		le->next = ne;
		le = ne->next;
	}

	return 0;
}

int znsub_ass3d_discard (ZnsubASS * sub, unsigned long flag)
{
#ifndef ZN_NODEBUG
	if (debug_mode != 0)
		fprintf (error_file, "znsub_ass3d_discard (%p, %ld)\n",
				(void*)sub, flag);
#endif
	/* flag doesn't do anything yet, place holder */
	int mode = 0;

	/* Detect mode
	 * Use the different between margin to determine */
	long mode_rank [2] = { 0, 0 };
	long sbs_mm	[4] = { 0, 0, 0, 0 };
	long tb_mm	[2] = { 0, 0 };
	long id = 1;
	sbs_mm[0] = sub->play_resx > 0 ? sub->play_resx : 1920;
	sbs_mm[2] = sbs_mm[0];
	tb_mm[0] = sub->play_resy > 0 ? sub->play_resy : 1080;
	long side = (sub->play_resx + sub->play_resy);
	if (sub->play_resy > 0 && sub->play_resx > 0)
		side /= 7;
	else if (sub->play_resx > 0)
		side /= 4;
	else if (sub->play_resy > 0)
		side /= 3;

	ZnsubASSStyle * style = sub->first_style;
	while (style != NULL)
	{
		sbs_mm[0] = style->margin_l < sbs_mm[0] ?
			style->margin_l : sbs_mm [0];
		sbs_mm[1] = style->margin_l > sbs_mm[1] ?
			style->margin_l : sbs_mm [1];
		sbs_mm[2] = style->margin_r < sbs_mm[2] ?
			style->margin_l : sbs_mm [2];
		sbs_mm[3] = style->margin_r > sbs_mm[3] ?
			style->margin_l : sbs_mm [3];
		tb_mm[0] = style->margin_v < tb_mm[0] ?
			style->margin_v : tb_mm[0];
		tb_mm[1] = style->margin_v > tb_mm[0] ?
			style->margin_v : tb_mm[0];
		style->id = id++;
		style = style->next;
	}
	mode_rank[0] = sbs_mm[1] + sbs_mm[3] - sbs_mm[0] - sbs_mm[2];
	mode_rank[1] = (tb_mm[1] - tb_mm[0])*32/9; /* use 16:9 */

	if (mode_rank[0] > mode_rank[1] && mode_rank[0] > side)
		mode = ZNSUB_ASS3D_SBS;
	else if (mode_rank[1] > side && mode_rank[1] > side)
		mode = ZNSUB_ASS3D_TB;
	else
		return -1;

	/* marking the part that need to be remove 
	 * style that will be removed has id equal to -1
	 * SBS: margin_r will be set to margin_l */
	if (mode == ZNSUB_ASS3D_TB)
	{
		long side = sub->play_resy / 3;
		side = side > 0 ? side : 360;
		ZnsubASSStyle * style = sub->first_style;
		while (style != NULL)
		{
			if (style->margin_v > side)
				style->id = -1;
			style = style->next;
		}
	}
	else if (mode == ZNSUB_ASS3D_SBS)
	{
		long side = sub->play_resx / 3;
		side = side > 0 ? side : 640;
		ZnsubASSStyle * style = sub->first_style;
		while (style != NULL)
		{
			if (style->margin_l > side)
				style->id = -1;
			else
				style->margin_r = style->margin_l;
			style = style->next;
		}
	}
	/* remove events */
	ZnsubASSEvent * first = NULL, * cur, * next;
	ZnsubASSEvent * event = NULL;
	next = sub->first_event;
	while (next != NULL)
	{
		if (next->style->id < 0)
		{
			event = next->next;
			znsub_ass_event_free (next);
			next = event;
			continue;
		}
		else if (first == NULL)
		{
			first = next;
			cur = next;
		}
		else
		{
			cur->next = next;
			cur = next;
		}
		next = next->next;
	}
	cur->next = NULL;
	sub->first_event = first;
	sub->last_event = cur;

	/* remove styles */
	ZnsubASSStyle * sfirst = NULL, * scur, *snext;
	ZnsubASSStyle * sstyle = NULL;
	snext = sub->first_style;
	while (snext != NULL)
	{
		if (snext->id < 0)
		{
			sstyle = snext->next;
			znsub_ass_style_free (snext);
			snext = sstyle;
			continue;
		}
		else if (sfirst == NULL)
		{
			sfirst = snext;
			scur = snext;
			scur->next = NULL;
		}
		else
		{
			scur->next = snext;
			scur = snext;
		}
		snext = snext->next;
	}
	scur->next = NULL;
	sub->first_style = sfirst;
	sub->last_style = scur;
	return 0;
}





/** subass.h
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
#ifndef __SUBASS_H__
#define __SUBASS_H__

#include <stdio.h>

/* ZnsubASS */
/* ZnsubASSStyle */
#define ZNSUB_ASS_DEF_WIDTH		1920
#define ZNSUB_ASS_DEF_HEIGHT	1080 
#define ZNSUB_ASS_STYLE_COLORSZ	16
typedef struct _znsubassstyle_ {
	long id;
	char * name;
	char * font_name;
	long font_size;
	char primary_colour [ZNSUB_ASS_STYLE_COLORSZ];
	char secondary_colour [ZNSUB_ASS_STYLE_COLORSZ];
	char outline_colour [ZNSUB_ASS_STYLE_COLORSZ];
	char back_colour [ZNSUB_ASS_STYLE_COLORSZ];
	int scale_x;
	int	scale_y;
	int border_style;
	int outline;
	int shadow;
	int alignment;
	long margin_l;
	long margin_r;
	long margin_v;
	struct _znsubassstyle_ * next;
} ZnsubASSStyle;

/* ZnsubASSEvent */
typedef struct _znsubassevent_ {
	long id;
	int  layer;
	long start;
	long end;
	ZnsubASSStyle * style;
	char * text;
	struct _znsubassevent_ * next;
} ZnsubASSEvent;

/* ZnsubASS */
typedef struct _znsubass_ {
	char * title;
	char * original_authour;
	long play_resx;
	long play_resy;
	int  play_depth;
	double timer;
	ZnsubASSEvent * first_event;
	ZnsubASSEvent * last_event;
	ZnsubASSStyle * first_style;
	ZnsubASSStyle * last_style;
} ZnsubASS;

/* initialization */
ZnsubASSStyle	* znsub_ass_style_new ();
ZnsubASSEvent	* znsub_ass_event_new (ZnsubASSStyle * style);
ZnsubASS		* znsub_ass_new ();
void znsub_ass_style_free (ZnsubASSStyle * style);
void znsub_ass_event_free (ZnsubASSEvent * event);
void znsub_ass_free (ZnsubASS * sub);

/* ZnsubASS */
int znsub_ass_style_color_set (char * str, int r, int g, int b);
int znsub_ass_style_color_setll (char * str, long long color);
int znsub_ass_add_event (ZnsubASS * sub, ZnsubASSEvent * event);
int znsub_ass_add_style (ZnsubASS * sub, ZnsubASSStyle * style);
int znsub_ass_tofile (ZnsubASS * sub, FILE * file);

/* SSA/ASS subtitles parser */
ZnsubASS * znsub_ass_parse (ZnFile * file);

#endif


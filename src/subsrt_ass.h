/** subsrt2ass.h
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
#ifndef __SUBSRT2ASS_H__
#define __SUBSRT2ASS_H__

#include "subsrt.h"
#include "subass.h"

#define ZNSUB_SRT_ASS_NOCOLOR	1

ZnsubASS * znsub_srt2ass (ZnsubSRT * srt, unsigned long flag);
ZnsubSRT * znsub_ass2srt (ZnsubASS * ass, unsigned long flag);
char * znsub_srt2ass_text (const char * str, unsigned long flag);
char * znsub_ass2srt_text (ZnsubASSStyle * style,
		const char * str, unsigned long flag);

#endif


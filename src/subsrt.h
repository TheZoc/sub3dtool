/* subsrt.h
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
#ifndef __SUBSRT_H__
#define __SUBSRT_H__

#include "utils.h"

/** ZnsubSRT */
typedef struct _znsubsrt_ {
	long id;
	long start; /* miliseconds */
	long end;
	char * text;
	struct _znsubsrt_ * next;
} ZnsubSRT;

ZnsubSRT * znsub_srt_new ();
void	   znsub_srt_free (ZnsubSRT * sub);
ZnsubSRT * znsub_srt_parse (ZnFile * file);
int		   znsub_srt_tofile (ZnsubSRT * sub, FILE * file);

#endif


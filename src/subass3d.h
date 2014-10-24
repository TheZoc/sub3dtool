/** subass3d.h 
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
#ifndef __SUBASS3D_H__
#define __SUBASS3D_H__

#include "subass.h"
#define ZNSUB_ASS3D_SBS		1
#define ZNSUB_ASS3D_TB		2
#define ZNSUB_ASS3D_NO3D	3
/* znsub_ass3d, 
 * convert function make subtitle 3D, best to use last
 * discard function reverse this process */
int znsub_ass3d_convert (ZnsubASS * sub, int mode, unsigned long flag);
int znsub_ass3d_discard (ZnsubASS * sub, unsigned long flag);

#endif


/** utils.h
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
#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdint.h>

/* zn_strdup: copy data from str to a newly allocated memory
 * zn_strset: free and reallocate memory to fit src, copy src
 * zn_strcimp: similar to strcmp but case-insensitive 
 * zn_strcinmp: similar to strncmp but case-insensitive 
 * zn_stristr: similar to strstr but case-insensitive
 * contents to the new buffer and set the dest pointer to it*/
char *	zn_strdup (const char * str);
int	zn_strset (char ** dest, const char * src);
int	zn_stricmp (const char * lhs, const char * rhs);
int	zn_strincmp (const char * lhs, const char * rhs, long count);
const char * zn_stristr (const char * str, const char * substr);

/* ZnStream: an auto-realloc memory buffer
 * Should not use for purpose that might exceed 64Kb in size */
#define ZNSTR_BLSIZE 32
typedef struct _zn_stream_
{
	char *	buf;
	long	len;
	long	size;
} ZnStream;

/* znstr_new ()
 * znstr_printf (), similar to printf
 * znstr_free (), free buffer, use znstr_close if data is still needed
 * znstr_close (), close the stream and return the buffer */
ZnStream *	znstr_new ();
int			znstr_printf (ZnStream * str, const char * fmt, ...);
void		znstr_free (ZnStream * str);
char *		znstr_close (ZnStream * str);

/* ZnFile: a text file input stream
 * Designed to read line to line, in a linear maner,
 * AVOID seeking, useful for parsing a file, not editing
 * TRANSPARENT support for UTF-16 and UTF-32:
 * Input data is convert to UTF-8 automatically
 * NOTE THAT *ALL* read data is kept in memory until closed */
#define ZNFILE_BLSIZE		32 /* read block (multiple of 4)*/
#define ZNFILE_PTRBLSIZE	8 /* ptr read block */
#define ZNFILE_UTF8		8
#define ZNFILE_UTF16	16
#define ZNFILE_UTF32	32
#define ZNFILE_LE		1
#define ZNFILE_BE		2

typedef struct _zn_file_
{
	FILE *	 file;			/* file stream */
	char *	 raw_data [2];	/* raw data */
	long raw_len [2];
	long raw_pos [2];
	char * * data;			/* processed data */
	long data_size;
	long index;
	int endianness;			/* endianness */
	int byteswap;			/* byteswap flag */
	int encoding;			/* encoding */
	int detection;			/* detection flag */
	long error;
} ZnFile;

#define zn_bswap16(x) (uint16_t)(x << 8 | x >> 8)
#define zn_bswap32(x) (( \
		((((uint16_t*)&x)[0] >> 8) | \
		(uint16_t)((((uint16_t*)&x)[0]) << 8)) << 16) | \
		(((uint16_t*)&x)[1] >> 8) | \
		(uint16_t)((((uint16_t*)&x)[1]) << 8))


/* znfile_open: open a file for reading
 * znfile_close: close an opened file, free data
 * znfile_detect: INTERNAL detect file encoding, ran only once
 * znfile_read: INTERNAL read from file, make byteswap if needed
 * znfile_wchartomb: INTERNAL convert Unicode code point to UTF8 
 * znfile_line: read line with index
 * znfile_linecurrent: return current line (char*)
 * znfile_lineindex: return current line index 
 * znfile_linenext: MOST read next line and return data */
ZnFile * znfile_open (const char * path);
void	 znfile_close (ZnFile * file);
/* INTERNAL function, should not be used
void	 znfile_detect	 (ZnFile * file);
long	 znfile_read	 (ZnFile * file, long block);
char *	 znfile_wchartomb (uint32_t ch);
*/
const char * znfile_line	 (ZnFile * file, long indx);
const char * znfile_linecurrent	(ZnFile * file); 
long		 znfile_lineindex	(ZnFile * file);
const char * znfile_linenext	(ZnFile * file);

#endif


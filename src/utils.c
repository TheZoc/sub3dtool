/** utils.c
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
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <ctype.h>
#include "utils.h"

/** generic function */
/** char * zn_strdup ()
 * allocate memory and copy the content of str to it */
char * zn_strdup (const char * str)
{
	long len = strlen (str);
	char * buf = malloc (len + 1);
	strncpy (buf, str, len);
	buf [len] = '\0';
	return buf;
}

/** char * zn_strset ()
 * reallocate memory to fit new input and
 * assign new value to it */
int zn_strset (char ** dest, const char * src)
{
	if (src == NULL || dest == NULL)
		return -1;
	long len = strlen (src);
	char * ptr = *dest;
	ptr = realloc (ptr, len + 1);
	if (ptr == NULL)
	{
		/* retry when failed */
		ptr = realloc (ptr, len + 1);
		if (ptr == NULL)
			return -1;
	}
	strncpy (ptr, src, len); 
	ptr [len] = '\0';
	*dest = ptr;
	return 0;
}

int	zn_stricmp (const char * lhs, const char * rhs)
{
	long i = 0;
	while (lhs[i] != '\0' && (lhs[i] == rhs[i] ||
				tolower (lhs[i]) == tolower (rhs[i])))
	{
		i++;
	}
	return (rhs[i] - lhs[i]);
}

int	zn_strincmp (const char * lhs, const char * rhs, long count)
{
	long i = 0;
	while (lhs[i] != '\0' && i < count
			&& (lhs[i] == rhs[i] ||
				tolower (lhs[i]) == tolower (rhs[i])))
	{
		i++;
	}
	if (i == count)
		return 0;
	return (rhs[i] - lhs[i]);
}

const char * zn_stristr (const char * str, const char * substr)
{
	if (substr[0] == '\0')
		return NULL;

	/* search string */
	const char * ptr = NULL;
	long i = 0;
	int key1 = tolower (substr[0]);
	int key2 = toupper (substr[0]);
	while (str[i] != '\0')
	{
		if (str[i] == key1 || str[i] == key2) 
		{
			ptr = &str[i];
			if (zn_stricmp (ptr, substr) == 0)
				return ptr;
		}
		i++;
	}
	return NULL;
}

/** int znstr_realloc ()
 * used to realloc memory for ZnStream */
int znstr_realloc (ZnStream * str, const long size)
{
	assert (str != NULL);
	/* size */
	long bsize = ((long)(size/ZNSTR_BLSIZE) + 1) * ZNSTR_BLSIZE;
	if (str->len >= size)
	{
		/* truncate */
		str->len = size - 1;
		str->buf [str->len] = '\0';
	}

	/* realloc memory and validate value */
	char * buf = realloc (str->buf, bsize);
	if (buf == NULL)
		return -1;
	else
	{
		str->size = bsize;
		str->buf = buf;
		return 0;
	}
}

/** ZnStream
 * a string buffer */
/** znstr_new ()
 * create a new ZnStream Object */
ZnStream * znstr_new ()
{
	ZnStream * ptr = calloc (1, sizeof (ZnStream));
	znstr_realloc (ptr, 1);
	ptr->buf[0] = '\0';
	return ptr;
}


/** int znstr_printf ()
 * "printf" to buffer */
int znstr_printf (ZnStream * str, const char * fmt, ...)
{
	/* buffer test - realloc memory if necessary*/
	assert (str != NULL);
	long delta = str->size - str->len;
	if (delta <= 0)
	{
		if (znstr_realloc (str, 16) != 0)
			printf ("ERROR\n");
		delta = str->size - str->len;
		assert (delta > 0);
	}

	/* first try */
	va_list va;
	va_start (va, fmt);
	char * ptr = &(str->buf [str->len]);
//#ifdef WIN32
//	long p = _vsnprintf_s(ptr, delta, delta, fmt, va);
//#else
	long p	= vsnprintf (ptr, delta, fmt, va);
//#endif // WIN32

	va_end (va);

	if (p == -1 || p >= delta)
	{
		/* second try
		* not enough memory? try again */
		znstr_realloc(str, str->size + ZNSTR_BLSIZE); // This allows Windows to properly write data. Still, it will fail if it needs more memory than ZNSTR_BLSIZE.
		delta = str->size - str->len;
		va_start(va, fmt);
		ptr = &(str->buf[str->len]);
		p = vsnprintf(ptr, delta, fmt, va);
		va_end(va);
		if (p >= delta)
			return -1;
		else
			str->len += p;
	}
	else
		str->len += p;

	return 0;
}

/** void znstr_free () */
void znstr_free (ZnStream * str)
{
	assert (str != NULL);
	if (str->buf != NULL)
		free (str->buf);
	free (str);
}

char * znstr_close (ZnStream * str)
{
	assert (str != NULL);
	char * ptr = str->buf;
	free (str);
	return ptr;
}

/* ZnFile: a text file input stream */
/* znfile_new () */

ZnFile * znfile_open (const char * path)
{
	FILE * f;
#ifdef WIN32
	//errno_t err =
	fopen_s(&f, path, "rb");
#else
	file = fopen(output, "w");
#endif // WIN32

	if (f == NULL)
		return NULL;

	ZnFile * file = calloc (1, sizeof (ZnFile));
	file->file = f;
	file->index = -1;
	return file;
}

void znfile_close (ZnFile * file)
{
	if (file->raw_data[0] != NULL)
		free (file->raw_data[0]);
	if (file->raw_data[1] != NULL)
		free (file->raw_data[1]);
	for (long i = 0; i <= file->index; i++)
	{
		if (file->data[i] != NULL)
			free (file->data[i]);
	}
	fclose (file->file);
	free (file);
}

int cpu_endianness = 0;

void znfile_detect_set (ZnFile * file, int mode)
{ /* set reading mode after detection */
	switch (mode)
	{
	case 15:
		file->encoding = ZNFILE_UTF16;
		file->endianness = ZNFILE_LE;
		file->detection = 1;
		break;
	case 16:
		file->encoding = ZNFILE_UTF16;
		file->endianness = ZNFILE_BE;
		file->detection = 1;
		break;
	case 31:
		file->encoding = ZNFILE_UTF32;
		file->endianness = ZNFILE_LE;
		file->detection = 1;
		break;
	case 32:
		file->encoding = ZNFILE_UTF32;
		file->endianness = ZNFILE_BE;
		file->detection = 1;
		break;
	default:
		file->detection = 1;
		file->encoding = ZNFILE_UTF8;
		break;
	}
	if (file->endianness != 0 && file->endianness != cpu_endianness)
	{
		/* byteswap */
		if (file->encoding == ZNFILE_UTF16)
		{
			long len = ZNFILE_BLSIZE / 2;
			len = len > file->raw_len[0] ? file->raw_len[0] : len;
			uint16_t * ptr = (uint16_t*) file->raw_data[0];
			/* byteswap */
			for (long i = 0; i < len; i++)
				ptr[i] = zn_bswap16 (ptr[i]);
		}
		else
		{
			long len = ZNFILE_BLSIZE / 4;
			len = len > file->raw_len[0] ? file->raw_len[0] : len;
			uint32_t * ptr = (uint32_t*) file->raw_data[0];
			/* byteswap */
			for (long i = 0; i < len; i++)
				ptr[i] = zn_bswap32 (ptr[i]);
		}
		file->byteswap = 1;
	}
}

int znfile_detect_strncmp (const char * str1,
		const char * str2, long len)
{ /* similar to strncmp but ignore NULL character */
	for (long i = 0; i < len; i++)
	{
		if (str1[i] != str2[i])
			return str1[i] - str2[i];
	}
	return 0;
}

long znfile_detect_assume_utf8 (const char * str, long len)
{
	assert (str != NULL && len > 0);
	/* testing */
	const unsigned char * ptr = (const unsigned char *) str;
	long result = 1024;
	int chain = 0;
	for (long i = 0; i < len; i++)
	{
		if (ptr[i] == '\0')
			result -= 64;
		else if (ptr[i] == ' ')
			result += 64;
		else if (ptr[i] < 0x80)
		{
			result += 4; /* normal character */
			chain = 0; /* remove chain mark */
		}
		else if (ptr[i] <= 0xc0)
		{
			/* chaining? */
			result += (chain == 0 ? -32 : 32);
		}
		else if (ptr[i] < 0xfe)
		{
			/* start chain, if already chain, minus */
			result += (chain != 0 ? -32 : 32);
			chain = 1;
		}
		/* else ignore, 0xff && 0xfe */
	}
	return result;
}

long znfile_detect_assume_utf16 (const uint16_t * str,
		long len, int bs)
{
	assert (str != NULL && len > 0);
	/* data handling */
	long size = sizeof (uint16_t)*len;
	uint16_t * data = malloc (size);
	uint16_t * data2 = malloc (size);
	memcpy (data, str, size);
	memcpy (data2, str, size);

	if (bs != 0)
	{
		for (long i = 0; i < len; i++)
			data[i] = zn_bswap16 (data[i]);
	}
	else
	{
		for (long i = 0; i < len; i++)
			data2[i] = zn_bswap16 (data[i]);
	}



	/* actual test */
	long result = 1024;
	int chain = 0;
	for (long i = 0; i < len; i++)
	{
		/* utf16 bit has less priority */
		if (data[i] == 0)
			result -= 128;
		else if (data[i] == ' ')
			result += 128;
		else if (data[i] >= 0xd800 && data[i] <= 0xdbff)
		{
			/* lead surrogate */
			result += (chain != 0 ? -64 : 64);
			chain = 1;
		}
		else if (data[i] >= 0xdc00 && data[i] <= 0xdfff)
		{
			/* trail surrogate */
			result += (chain == 0 ? -64 : 64);
			chain = 0;
		}
		else if (chain != 0)
		{
			/* unexpected chain end */
			chain = 0;
			result -= 64;
		}
		else if (data2[i] < 0x80) /* ascii character */
			result += 4;
		else if (data[i] < 0x80)
			result += 16;
		else
			result += 8;
	}

	free (data);
	free (data2);
	return result;
}

long znfile_detect_assume_utf32 (const uint32_t * str,
		long len, int bs)
{
	assert (str != NULL && len > 0);
	/* data handling */
	long size = sizeof(uint32_t)*len;
	uint32_t * data = malloc (size);
	memcpy (data, str, size);
	if (bs != 0)
	{
		for (long i = 0; i < len; i++)
			data[i] = zn_bswap32 (data[i]);
	}
	/* actual test */
	long result = 1024;
	for (long i = 0; i < len; i++)
	{
		/* points given is double that of utf16 */
		if (data[i] == 0)
			result -= 256;
		else if (data[i] == ' ')
			result += 256;
		else if (data[i] < 0x80)
			result += 64;
		else if (data[i] >= 0xd800 && data[i] <= 0xdfff)
		{
			/* reserve for utf16 */
			result -= 128;
		}
		else if (data[i] > 0x10ffff)
		{
			/* outside of unicode plane */
			result -= 128;
		}
		else
			result += 64;
		/* UTF32 normal character has more point */
	}

	free (data);
	return result;
}

void znfile_detect (ZnFile * file)
{
	assert (file != NULL);
	/* cpu_endianness detection */
	if (cpu_endianness == 0)
	{
		uint16_t x = 0;
		char * px = (char*) &x;
		px[0] = 0x12;
		px[1] = 0x34;
		cpu_endianness = x == 0x3412 ? ZNFILE_LE : ZNFILE_BE;
	}

	/* buffer reading */
	char * buf	= malloc (ZNFILE_BLSIZE + 4);
	long read	= fread (buf, 1, ZNFILE_BLSIZE, file->file);
	((uint32_t*) &buf[read])[0] = 0;

	if (read == EOF)
	{
		file->error = EOF;
		free (buf);
		return;
	}

	file->raw_len[0] = read;
	file->raw_data[0] = buf;

	int  encoding = 32;
	long utf_rank [5] = { 0, 0, 0, 0, 0 };
	int  utf_eqv [5] = { 8, 16, 15, 32, 31 };

	/* Length detection */
	if (read % 2 == 1)
	{ /* bytes number is odd, force UTF-8 */
		/* encoding = 8; */
		znfile_detect_set (file, 8);
		return;
	}
	else if (read % 4 != 0)
	{
		utf_rank[3] -= 1024;
		utf_rank[4] -= 1024;
		encoding = 16;
	}

	/*****************/
	/* BOM detection */
	const char * utf8_bom = "\xef\xbb\xbf";
	const char * utf16_le = "\xff\xfe";
	const char * utf16_be = "\xfe\xff";
	const char * utf32_le = "\xff\xfe\x0\x0";
	const char * utf32_be = "\x0\x0\xfe\xff";
	/* utf32 check */
	if (encoding == 32)
	{
		if (znfile_detect_strncmp (utf32_le, buf, 4) == 0)
		{
			memmove (buf, &buf[4], ZNFILE_BLSIZE);
			utf_rank[4] += 1024;
		}
		else if (znfile_detect_strncmp (utf32_be, buf, 4) == 0)
		{
			memmove (buf, &buf[4], ZNFILE_BLSIZE);
			utf_rank[3] += 1024;
		}
	}
	else if (encoding != 0)
	{ /* utf16 check */
		if (strncmp (utf16_le, buf, 2) == 0)
		{
			memmove (buf, &buf[2], ZNFILE_BLSIZE + 2);
			utf_rank[2] += 1024;
		}
		else if (strncmp (utf16_be, buf, 2) == 0)
		{
			memmove (buf, &buf[2], ZNFILE_BLSIZE + 2);
			utf_rank[1] += 1024;
		}
		else if (strncmp (utf8_bom, buf, 3) == 0)
		{
			memmove (buf, &buf[3], ZNFILE_BLSIZE + 3);
			utf_rank[0] += 1024;
		}
	}
	/* BOM check ended */
	/* Assumption check */
	utf_rank[0] += znfile_detect_assume_utf8 (buf, read);
	if (cpu_endianness == ZNFILE_BE)
	{
		utf_rank[1] += znfile_detect_assume_utf16 ((uint16_t*)buf,
				read/2, 0);
		utf_rank[2] += znfile_detect_assume_utf16 ((uint16_t*)buf,
				read/2, 1);
		utf_rank[3] += znfile_detect_assume_utf32 ((uint32_t*)buf,
				read/4, 0);
		utf_rank[4] += znfile_detect_assume_utf32 ((uint32_t*)buf,
				read/4, 1);
	}
	else
	{
		utf_rank[1] += znfile_detect_assume_utf16 ((uint16_t*)buf,
				read/2, 1);
		utf_rank[2] += znfile_detect_assume_utf16 ((uint16_t*)buf,
				read/2, 0);
		utf_rank[3] += znfile_detect_assume_utf32 ((uint32_t*)buf,
				read/4, 1);
		utf_rank[4] += znfile_detect_assume_utf32 ((uint32_t*)buf,
				read/4, 0);
	}
	int detect = 8;
	long mxp = LONG_MIN;
	for (int i = 0; i < 5; i++)
	{
		if (utf_rank[i] > mxp)
		{
			mxp = utf_rank[i];
			detect = utf_eqv[i];
		}
	}
	znfile_detect_set (file, detect);
}

long znfile_read (ZnFile * file, long block)
{
	assert (file != NULL);
	long blsize = 1;
	long read = 0;
	block = block * ZNFILE_BLSIZE;
	char ** ptr;
	char * data;

	/* set blsize */
	if (file->encoding == ZNFILE_UTF16)
		blsize = 2;
	else if (file->encoding == ZNFILE_UTF32)
		blsize = 4;

	/* set ptr */
	if (file->raw_data [0] == NULL)
		ptr = & file->raw_data [0];
	else if (file->raw_data [1] == NULL)
		ptr = & file->raw_data [1];
	else
		return 0;

	/* set blsize */
	data = malloc (blsize * (block + 1));
	*ptr = data;
	read = fread (data, blsize, block, file->file);
	if (read == 0 || read == EOF)
	{
		free (data);
		*ptr = NULL;
		return file->error = EOF;
	}

	if (file->byteswap != 0 && file->encoding == ZNFILE_UTF16)
	{
		uint16_t * chunk = (uint16_t*) data;
		for (long i = 0; i < read; i++)
		{
			chunk[i] = zn_bswap16 (chunk[i]);
		}
		chunk[read] = 0;
	}
	else if (file->byteswap != 0)
	{
		uint32_t * chunk = (uint32_t*) data;
		for (long i = 0; i < read; i++)
		{
			chunk[i] = zn_bswap32 (chunk[i]);
		}
		chunk[read] = 0;
	}
	else if (file->encoding == ZNFILE_UTF16)
	{
		uint16_t * chunk = (uint16_t*) data;
		chunk[read] = 0;
	}
	else if (file->encoding == ZNFILE_UTF32)
	{
		uint32_t * chunk = (uint32_t*) data;
		chunk[read] = 0;
	}
	else
	{
		data[read] = 0;
	}

	return read;
}

const char * znfile_line (ZnFile * file, long index)
{
	assert (file != NULL);
	assert (index >= 0 && index < file->data_size);
	return file->data [index];
}

const char * znfile_linecurrent	(ZnFile * file)
{
	assert (file != NULL);
	return file->data [file->index];
}

long znfile_lineindex (ZnFile * file)
{
	assert (file != NULL);
	return file->index;
}

int znfile_data_realloc (ZnFile * file, long size)
{
	/* realloc data pointer */
	assert (file != NULL);
	assert (size > file->data_size);
	char * * data = realloc (file->data, size*sizeof(void*));
	assert (data != NULL);
	long i = file->index + 1;
	data[i] = NULL;
	file->data = data;
	file->data_size = size;

	return 0;
}

char * znfile_wchartomb (uint32_t ch)
{
	/* TODO: this is very ineffiency. I should try to create
	 * an alternative method by using macro instead */
	const uint32_t chart [] = { 0x7f, 0x7ff, 0xffff, 0x1fffff,
		0x3ffffff, 0x7fffffff };
	char * out = malloc (8);
	int i = 0;
	for (i = 0; i < 6; i++)
	{
		if (chart[i] > ch)
			break;
	}
	i++;
	out[i] = '\0';
	if (i == 1)
	{
		out[0] = (char) ch;
		return out;
	}
	
	char end = 0x0;
	char end_mask = 0xff >> 1;
	for (int j = i - 1; j >= 0; j--)
	{
		end		 = end >> 1 | 0x80;
		end_mask = end_mask >> 1;
		if (j != 0)
		{
			out[j] = (ch & 0x3f) | 0x80; /* 2\10111111 mask */
			ch = ch >> 6; /* right shift data */
		}
		else
			out[0] = end | (end_mask & ch);
	}

	return out;
}

const char * znfile_linenext (ZnFile * file)
{
	assert (file != NULL);
	/* detection */
	if (file->detection == 0)
		znfile_detect (file);

	/* allocate pointer */
	long index = file->index + 1; /* increase index */
	index = index >= 0 ? index : 0;
	if (index >= file->data_size)
	{
		znfile_data_realloc (file,
				file->data_size + ZNFILE_PTRBLSIZE);
	}

	/* allocate stream */
	ZnStream * str = znstr_new ();
	int done = 0;
	long fr = 0;
	do 
	{
		/* read data from file */
		if (file->raw_data[0] == NULL)
		{
			fr = znfile_read (file, ZNFILE_PTRBLSIZE);
			if (fr == 0 || fr == EOF)
			{
				done = 2;
				break;
			}
		}

		/* processing */
		if (file->encoding == ZNFILE_UTF32)
		{ /* utf32 encoding processor */
			uint32_t * data = (uint32_t*) file->raw_data[0];
			char * ptr = NULL;
			long start = file->raw_pos[0];
			long i = start;
			while (data[i] != 0)
			{
				if (data[i] == '\x0a')
				{
					/* line break found */
					file->raw_pos[0] = i+1;
					done = 1;
					break;
				}
				else if (data[i] != '\x0d')
				{ /* discard cariage return */
					ptr = znfile_wchartomb (data[i]);
					znstr_printf (str, "%s", ptr);
					free (ptr);
				}
				i++;
			}
			if (done != 0) /* done, break loop */
				break;
		}
		else if (file->encoding == ZNFILE_UTF16)
		{ /* utf16 processor */
			uint16_t * data = (uint16_t*) file->raw_data[0];
			uint32_t code;
			long start = file->raw_pos[0];
			long i = start;
			char * ptr = NULL;
			while (data[i] != 0)
			{
				if (data[i] == '\x0a')
				{
					/* line break found */
					file->raw_pos[0] = i+1;
					done = 1;
					break;
				}
				else if (data[i] >= 0xd800 && data[i] <= 0xdfff)
				{
					if (data[i] <= 0xdbff &&
							(data[i+1] >= 0xdc00 &&
							 data[i+1] <= 0xdfff))
					{ /* leading surrogates */
						code = (data[i] - 0xd800) << 10;
						code += data[i+1] - 0xdc00;
						code += 0x10000;
						ptr = znfile_wchartomb (code);
						znstr_printf (str, "%s", ptr);
						free (ptr);
					}
				}
				else if (data[i] != '\x0d')
				{ /* discard cariage return */
					ptr = znfile_wchartomb (data[i]);
					znstr_printf (str, "%s", ptr);
					free (ptr);
				}
				i++;
			}
			if (done != 0)
				break;
		}
		else
		{
			char * data = (char*) file->raw_data[0];
			long start = file->raw_pos[0];
			long i = start;
			while (data[i] != 0)
			{
				if (data[i] == '\x0a')
				{
					/* line break found */
					file->raw_pos[0] = i+1;
					done = 1;
					break;
				}
				else if (data[i] != '\x0d')
				{ /* discard cariage return */
					znstr_printf (str, "%c", data[i]);
				}
				i++;
			}
			if (done != 0)
				break;
		}
		/* newline not met, free old data */
		free (file->raw_data[0]);
		file->raw_data[0]	= file->raw_data[1];
		file->raw_pos[0]	= file->raw_pos[1];
		file->raw_data[1]	= NULL;
		file->raw_pos[1]	= 0;
		/* continue reading from file */
	} while (done == 0);
	/* done == 1 for normal exit, 2 for eof */
	if (done == 1)
	{
		char * ptr = znstr_close (str);
		file->data[index] = ptr;
		file->index = index;
		return ptr;
	}
	else 
	{ /* EOF return NULL */
		znstr_free (str);
		return NULL;
	}
}









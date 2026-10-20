/* 
    ======================================================================
    This file is part of FurniOS.

    Copyright (C) 2026 Oliver Arendt (furnio, fkolleg)
    www.fkolleg.de
    info@fkolleg.de

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 3
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, see
    <https://www.gnu.org/licenses/>.

    Additional restriction:
    This software may not be used, in whole or in part, to train machine 
    learning models (including LLMs) without explicit prior written 
    permission.
    Violations will terminate your rights under this license.

    ======================================================================
*/

#include <string.h>

int memcmp(const void *s1, const void *s2, size_t n)
{
	const unsigned char *str1 = s1;
	const unsigned char *str2 = s2;
	int		     res  = 0;
	while (n > 0 && res == 0) {
		res = *str1 - *str2;
		str1++;
		str2++;
		n--;
	}
	return res;
}

void *memcpy(void *restrict s1, const void *restrict s2, size_t n)
{
	char *restrict str1	  = s1;
	const char *restrict str2 = s2;
	while (n > 0) {
		*str1 = *str2;
		str1++;
		str2++;
		n--;
	}
	return s1;
}

void *memmove(void *s1, const void *s2, size_t n)
{
	char	   *str1 = s1;
	const char *str2 = s2;
	if (str1 < str2) {
		while (n > 0) {
			*str1 = *str2;
			str1++;
			str2++;
			n--;
		}
	} else {
		while (n > 0) {
			n--;
			*(str1 + n) = *(str2 + n);
		}
	}
	return s1;
}

void *memset(void *s, int c, size_t n)
{
	unsigned char *str = s;
	while (n > 0) {
		*str = (unsigned char)c;
		str++;
		n--;
	}
	return s;
}

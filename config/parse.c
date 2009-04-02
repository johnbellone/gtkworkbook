/* 
   parse.c - Generic Parsing Code

   The GTKWorkbook Project <http://gtkworkbook.sourceforge.net/>
   Copyright (C) 2008, 2009 John Bellone, Jr. <jvb4@njit.edu>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PRACTICAL PURPOSE. See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301 USA
*/
#include "parse.h"
#include <string.h>
#include <stdlib.h>

gchar *
munch (gchar *p)
{
  ASSERT (p != NULL);
  
  /* Munch whitespace and ignore newlines, tabs. */
  while (p && (*p == ' ' || *p == '\n' || *p == '\t')) { p++; } 
  return p;
}

gchar *
parse (gchar *p, gchar *word, gint n, const gchar delim)
{
  const gchar *q = p;
  gint length = 0, ii = 0;

  do 
    {
      if (*p == delim)
	break;
      p++;
    } while (p && (*p != '\0') && (*p != '\n'));

  length = (gint)(p - q);

  if (n < length)    return NULL;

  for (ii = 0; ii < length; ii++)
    word[ii] = q[ii];
  word[ii] = '\0';

  return p;
}

gchar *
trim (gchar *p)
{
  ASSERT (p != NULL);

  gchar *q = NULL;

  /* Eliminate tabs and spaces from beginning. */
  while (p && (*p == ' ' || *p == '\t')) p++;

  q = p;

  /* Eliminate newlines, returns and whitespaces from end. */
  while (*q != '\0') { q++; }
  q--;
  while (*q == ' ' || *q == '\n' || *q == '\r') { q--; }
  *(q+1) = '\0';

  return p;
}

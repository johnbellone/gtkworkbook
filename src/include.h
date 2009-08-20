/* 
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
#ifndef H_INCLUDE
#define H_INCLUDE

#include <glib.h> 
#include <csv/csv.h>

#define FCLOSE(fp) { if (fp != NULL) { fflush(fp); fclose(fp); fp = NULL; } }
#define PCLOSE(fp) { if (fp != NULL) { pclose(fp); fp = NULL; } }

#define UNLINK_OBJECT(current) if (current && current->next && current->prev) \
	{																							\
      current->prev->next = current->next;										\
      current->next->prev = current->prev;										\
      current->next = NULL;															\
      current->prev = NULL;															\
	}																							\
	else if (current && current->next)												\
	{																							\
      current->next->prev = NULL;													\
      current->next = NULL;															\
	}																							\
	else if (current && current->prev)												\
	{																							\
      current->prev->next = NULL;													\
      current->prev = NULL;															\
	}

#define LINK_OBJECT(first, last, current) if (!first) \
	{																	\
      first = current;											\
      last = NULL;												\
      current->next = NULL;									\
      current->prev = NULL;									\
	}																	\
	else																\
	{																	\
      if (!last)													\
		{																\
			last = current;										\
			current->next = NULL;								\
			current->prev = first;								\
			first->next = current;								\
			first->prev = NULL;									\
		}																\
      else															\
		{																\
			current->prev = last;								\
			current->next = NULL;								\
			last->next = current;								\
			last = current;										\
		}																\
	}

#define gmalloc(size) g_malloc(size)
#define gfree(pointer) { g_free(pointer); pointer = NULL; }
#define gassert(condition) g_assert(condition)

#endif

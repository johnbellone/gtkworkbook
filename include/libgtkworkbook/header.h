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
#ifndef HPP_GTKWORKBOOK_HEADER
#define HPP_GTKWORKBOOK_HEADER

#include <glib.h> 
#include <stdio.h>

#define ASSERT(c) g_assert(c)
#define FREE(p) { if (p) { g_free (p); p = NULL; } }
#define NEW(T) (T*)g_malloc (sizeof (T))
#define NEW_ARRAY(T,n) g_malloc (sizeof(T)*n))

#define IS_NULLSTR(s) (!s || (*s == '\0'))
#define IS_NULL(p) (p == NULL)

#define FCLOSE(fp) { if (fp != NULL) { fflush(fp); fclose(fp); fp = NULL; } }
#define PCLOSE(fp) { if (fp != NULL) { pclose(fp); fp = NULL; } }

#define ITERATE_BEGIN(TYPE, head) if (head) {	\
	TYPE *it = NULL, *next = head;					\
	do { it = next; next = it->next;	

#define ITERATE_END() } while (next); }

#define DESTROY(TYPE, head) if (head) {		   \
		TYPE *next = head, *item = NULL;				\
		do { item = next; next = item->next;	   \
			item->destroy (item); item = NULL;	   \
		} while (next);									\
	}

#define SINGLE_LINK(TYPE, head, tail, item) if (tail) {		\
		tail->next = item; tail = item;								\
	} else if (head) {													\
		TYPE *next = head, *prev = NULL;								\
		do { prev = next; next = prev->next; } while (next);	\
		prev->next = item; tail = item;								\
	} else { head = item; }

#define SINGLE_UNLINK(TYPE, head, tail, item) if (item == head) { \
		head = item->next;														\
	} else { TYPE *next = head->next, *prev = NULL;						\
		while (next) { prev = next; next = prev->next;					\
			if (next == item) break; }											\
		prev->next = item->next; }

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

#define LINK_OBJECT(first, last, current) if (!first)	\
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

#define DOUBLE_LINK(head, tail, item) LINK_OBJECT(head, tail, item)
#define DOUBLE_UNLINK(item) UNLINK_OBJECT(item)

#endif /*HPP_GTKWORKBOOK_HEADER*/


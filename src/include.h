/* @author: John `jb Bellone <jvb4@njit.edu> */
#ifndef H_INCLUDE
#define H_INCLUDE

#include <glib.h> 
#include <csv/csv.h>

#define FCLOSE(fp) { if (fp != NULL) { fflush(fp); fclose(fp); fp = NULL; } }
#define PCLOSE(fp) { if (fp != NULL) { pclose(fp); fp = NULL; } }

#define UNLINK_OBJECT(current) if (current && current->next && current->prev) \
    { \
      current->prev->next = current->next; \
      current->next->prev = current->prev; \
      current->next = NULL; \
      current->prev = NULL; \
    } \
  else if (current && current->next) \
    { \
      current->next->prev = NULL; \
      current->next = NULL; \
    } \
  else if (current && current->prev) \
    { \
      current->prev->next = NULL; \
      current->prev = NULL; \
    }

#define LINK_OBJECT(first, last, current) if (!first) \
    { \
      first = current; \
      last = NULL; \
      current->next = NULL; \
      current->prev = NULL; \
    } \
  else \
    { \
      if (!last) \
        { \
          last = current; \
          current->next = NULL; \
          current->prev = first; \
          first->next = current; \
          first->prev = NULL; \
        } \
      else \
        { \
          current->prev = last; \
	  current->next = NULL; \
	  last->next = current;	\
          last = current; \
        } \
    }

#define gmalloc(size) g_malloc(size)
#define gfree(pointer) { g_free(pointer); pointer = NULL; }
#define gassert(condition) g_assert(condition)

#endif

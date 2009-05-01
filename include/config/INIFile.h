/* 
   INIFile.h - INI File Object/Parser Header File

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
#ifndef H_INIFILE
#define H_INIFILE
#ifdef __cplusplus
extern "C" {
#endif

typedef struct _INIFile INIFile;
typedef struct _INISection INISection;
typedef struct _INIPair INIPair;

#include <shared.h>

struct _INIPair
{
  /* Members */
  INISection *section;
  INIPair *next;
  gchar *key;
  gchar *value;
  gint line;

  /* Methods */
  void (*destroy) (INIPair *pair);
};

struct _INISection
{
  /* Members */
  INIFile *ini;
  INISection *next;
  INIPair *pair_head;
  INIPair *pair_tail;
  gint pair_count;
  gchar *title;
  
  /* Methods */
  const gchar *(*getValue) (INISection *section, const gchar *key);
  INIPair *(*getPair) (INISection *section, const gchar *key);
  void (*destroy) (INISection *section);
  void (*addPair) (INISection *section, 
		   gint line, 
		   const gchar *key,
		   const gchar *value);
};

struct _INIFile
{
  /* Members */
  INISection *section_head;
  INISection *section_tail;
  gchar *filename;

  /* Methods */
  gboolean (*open) (INIFile *);
  gboolean (*save) (INIFile *, const gchar *);
  gboolean (*load) (INIFile *, FILE *);
  void (*destroy) (INIFile *);
  INISection *(*getSection) (INIFile *ini, const gchar *section);
  INIPair *(*getPair) (INIFile *ini, const gchar *section, const gchar *key);
  const gchar *(*getValue) (INIFile *ini, 
			   const gchar *section, 
			   const gchar *key); 
};

/* INIFile.c */
INIFile *inifile_new (const gchar *filename);
INISection *inisection_new (INIFile *ini, const gchar *section);
INIPair *inipair_new (INISection *section, 
		      const gchar *key, 
		      const gchar *value,
		      gint line);
#ifdef __cplusplus
}
#endif
#endif /*H_INIFILE*/

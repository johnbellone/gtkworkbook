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
#include <string.h>
#include "INIFile.h"
#include "parse.h"

/* INIFile.c (static) */
static INIFile *inifile_object_init (const gchar *);
static INISection *inisection_object_init (INIFile *, const gchar *);
static INIPair *inipair_object_init (INISection *,
				     const gchar *,
				     const gchar *,
				     gint);
static void inifile_object_free (INIFile *);
static void inisection_object_free (INISection *);
static void inipair_object_free (INIPair *);
static gboolean inifile_method_open (INIFile *);
static gboolean inifile_method_save (INIFile *, const gchar *);
static gboolean inifile_method_load (INIFile *, FILE *);
static void inifile_method_destroy (INIFile *);
static INISection *inifile_method_get_section (INIFile *, const gchar *);
static INIPair *inifile_method_get_pair (INIFile *, 
					 const gchar *, 
					 const gchar *);
static const gchar *inifile_method_get_value (INIFile *,
					      const gchar *,
					      const gchar *);
static const gchar *inisection_method_get_value (INISection *,
						 const gchar *);
static INIPair *inisection_method_get_pair (INISection *, const gchar *);
static void inisection_method_destroy (INISection *);
static void inisection_method_add_pair (INISection *,
					gint, 
					const gchar *,
					const gchar *);
static void inipair_method_destroy (INIPair *);

INIFile *inifile_new (const gchar * filename)
{
  return inifile_object_init (filename);
}

INISection *inisection_new (INIFile * ini, const gchar * section)
{
  return inisection_object_init (ini, section);
}

INIPair *inipair_new (INISection * section, 
		      const gchar * key, 
		      const gchar * value,
		      gint line)
{
  return inipair_object_init (section, key, value, line);
}

static INIFile *
inifile_object_init (const gchar * filename)
{
  if (!IS_NULLSTR (filename))
    return NULL;

  INIFile * ini = NEW (INIFile);

  /* Members */
  ini->filename = g_strdup (filename);

  /* Methods */
  ini->open = inifile_method_open;
  ini->save = inifile_method_save;
  ini->load = inifile_method_load;
  ini->destroy = inifile_method_destroy;
  ini->getSection = inifile_method_get_section;
  ini->getPair = inifile_method_get_pair;
  ini->getValue = inifile_method_get_value;

  return ini;
}

static void
inifile_object_free (INIFile *ini)
{
  ASSERT (ini != NULL);

  FREE (ini);
}

static void
inifile_method_destroy (INIFile *ini)
{
  ASSERT (ini != NULL);

  inifile_object_free (ini);
}

static gboolean
inifile_method_save (INIFile * ini, const gchar * filename)
{
  ASSERT (ini != NULL);

  if (IS_NULLSTR (filename))
    return FALSE;

  return TRUE; 
}

static gboolean
inifile_method_open (INIFile * ini)
{
  ASSERT (ini != NULL);

  if (IS_NULLSTR (ini->filename))
    return FALSE;

  FILE * fp = NULL;
  if ((fp = fopen (ini->filename, "r")) == NULL)
    {
      fprintf (stderr, "Failed opening INIFile '%s'\n", ini->filename);
      fflush (stderr);
      return FALSE;
    }

  if (ini->load (ini, fp) == FALSE)
    {
      fprintf (stderr, "Failed loading INIFile '%s'\n", ini->filename);
      fflush (stderr);
      FCLOSE (fp);
      return FALSE;
    }
  FCLOSE (fp);
  return TRUE;
}

static gboolean
inifile_method_load (INIFile * ini, FILE * fp)
{
  ASSERT (ini != NULL);

  if (IS_NULL (fp))
    return FALSE;

  gint line = 0;
  INISection *section = NULL;
  gchar buf[4096], key[1024], value[1024];
  gchar * p = NULL;

  do
    {
      p = buf;

      if (IS_NULLSTR (p) || feof (fp))
	break;
      /* As per the INI file format a semicolon is considered a line block
	 comment. This continues until there is a newline character. */
      else if (*p == ';')
	{
	  p = munch (p);
	  continue;
	}
      /* [section] */
      else if (*p == '[') 
	{
	  if ((p = parse (p+1, &buf[0], 1024, ']')) == NULL)
	    {
	      fprintf (stderr, "Error parsing block '%s'\n", p);
	      fflush (stderr);
	      return FALSE;
	    }

	  section = inisection_new (ini, key);
	}
      /* We do not care about these characters at all. Move on to the next. */
      else if ((*p == ']') || (*p == '\t') || (*p == '\r'))
	{
	  p++;					
	  continue;
	}
      else if (*p == '\n')
	{
	  line++;
	  p++;
	  continue;
	}

      /* We do not suppor any type of "global" key->value pairs. There must
	 a section associated with a pair. */
      if (IS_NULL (section))
	{
	  fprintf (stderr, "Error parsing file on line %d; please check"
		   " file integrity\n", line);
	  fflush (stderr);
	  return FALSE;
	}

      /* Key = Value\n */
      if ((p = parse (p, &key[0], 1024, '=')) == NULL)
	{
	  fprintf (stderr, "Error parsing section %s: key->value at line %d\n",
		   section->title, line);
	  fflush (stderr);
	  return FALSE;
	}

      if ((p = parse (p+1, &value[0], 1024, '\n')) == NULL)
	{
	  fprintf (stderr, "Error parsing section %s: %s->value at line %d\n",
		   section->title, key, line);
	  fflush (stderr);
	  return FALSE;
	}
      
      section->addPair (section, line, key, value);

      line++;
    } while (fgets (buf, 4096, fp) != NULL);
  return TRUE;
}

static INISection *
inifile_method_get_section (INIFile * ini, const gchar * section)
{
  ASSERT (ini != NULL);

  if (!IS_NULLSTR (section))
    {
      ITERATE_BEGIN (INISection, ini->section_head);
      {
	if (strcmp (section, it->title) == 0)
	  return it;
      }
      ITERATE_END();
    }
  return NULL;
}

static INIPair *
inifile_method_get_pair (INIFile * ini, 
			 const gchar * section,
			 const gchar * key)
{
  ASSERT (ini != NULL);

  if (!IS_NULLSTR (section) && !IS_NULLSTR (key))
    {
      INISection * s = ini->getSection (ini, section);
      if (s == NULL)
	return NULL;

      return s->getPair (s, key);

    }
  return NULL;
}

static const gchar *
inifile_method_get_value (INIFile * ini,
			  const gchar * section,
			  const gchar * key)
{
  ASSERT (ini != NULL);

  if (!IS_NULLSTR (section) && !IS_NULLSTR (key))
    {
      INISection * s = ini->getSection (ini, section);
      if (s == NULL)
	return NULL;

      return s->getValue (s, key);
    }
  return NULL;
}

static void
inisection_object_free (INISection * section)
{
  ASSERT (section != NULL);

  DESTROY (INIPair, section->pair_head);

  FREE (section->title);
  FREE (section);
}

static INISection *
inisection_object_init (INIFile * ini, const gchar * section)
{
  ASSERT (ini != NULL);

  if (IS_NULLSTR (section))
    return NULL;

  INISection * s = ini->getSection (ini, section);
  if (IS_NULL (s))
    {
      s = NEW (INISection);
      
      /* Members */
      s->ini = ini;
      s->next = NULL;
      s->pair_head = NULL;
      s->pair_count = 0;
      s->title = g_strdup ("");
      
      /* Methods */
      s->addPair = inisection_method_add_pair;
      s->getValue = inisection_method_get_value;
      s->getPair = inisection_method_get_pair;
      s->destroy = inisection_method_destroy;
    }
  return s;
}

static const gchar *
inisection_method_get_value (INISection * section, const gchar * key)
{
  ASSERT (section != NULL);

  if (!IS_NULLSTR (key))
    {
      ITERATE_BEGIN (INIPair, section->pair_head);
      {
	if (strcmp (key, it->key) == 0)
	  return it->value;
      }
      ITERATE_END ();
    }
  return NULL;
}

static INIPair *
inisection_method_get_pair (INISection * section, const gchar * key)
{
  ASSERT (section != NULL);

  if (!IS_NULLSTR (key))
    {
      ITERATE_BEGIN (INIPair, section->pair_head);
      {
	if (strcmp (key, it->key) == 0)
	  return it;
      }
      ITERATE_END ();
    }
  return NULL;
}

static void
inisection_method_add_pair (INISection * section,
			    gint line,
			    const gchar * key,
			    const gchar * value)
{
  ASSERT (section != NULL);
  
  if (!IS_NULLSTR (key) && !IS_NULLSTR (value))
    {
      INIPair * pair = inipair_new (section, key, value, line);
      SINGLE_LINK (INIPair, section->pair_head, section->pair_tail, pair);
    }
}

static void
inisection_method_destroy (INISection * section)
{
  ASSERT (section != NULL);

  DESTROY (INIPair, section->pair_head);
  
  inisection_object_free (section);
}

static INIPair *
inipair_object_init (INISection * section, 
		     const gchar * key,
		     const gchar * value,
		     gint line)
{
  ASSERT (section != NULL);

  if (IS_NULLSTR (key) || IS_NULLSTR (value))
    return NULL;

  INIPair * pair = section->getPair (section, key);
  if (IS_NULL (pair))
    {
      pair = NEW (INIPair);
      
      /* Members */
      pair->section = section;
      pair->next = NULL;
      pair->key = g_strdup (key);
      pair->value = g_strdup (value);
      pair->line = line;

      /* Methods */
      pair->destroy = inipair_method_destroy;
    }
  return pair;
}

static void
inipair_method_destroy (INIPair * pair)
{
  inipair_object_free (pair);
}

static void
inipair_object_free (INIPair * pair)
{
  ASSERT (pair != NULL);
}

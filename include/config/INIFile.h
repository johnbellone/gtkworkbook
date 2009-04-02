/* @author: John `jb Bellone <jvb4@njit.edu> */
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

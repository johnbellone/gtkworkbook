#ifndef H_PARSE
#define H_PARSE

#include <glib.h>
#include <shared.h>

#define EXPECT(p,q,d) (*(q = munch (p)) == d)

/* parse.c */
extern gchar *munch (gchar *p);
extern gchar *parse (gchar *p, gchar *word, gint n, const gchar delim);
extern gchar *trim (gchar *p);

#endif

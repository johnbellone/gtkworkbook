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
#include <cstdlib>
#include <cstring>
#include "config.h"
#include "parse.h"

/* config.c (static) */

static Config *config_object_init (const gchar *);
static ConfigPair *configpair_object_init (ConfigRow *, 
														 const gchar *,
														 const gchar *);
static ConfigRow *configrow_object_init (ConfigBlock *, const gchar *);
static ConfigBlock *configblock_object_init (Config *, const gchar *);
static ConfigVector *configvector_object_init (ConfigRow *,const gchar *);
static void configrow_object_free (ConfigRow *);
static void configblock_object_free (ConfigBlock *);
static void configpair_object_free (ConfigPair *);
static void config_object_free (Config *);
static void configvector_object_free (ConfigVector *);
static ConfigVector *config_method_get_vector (Config *,
															  const gchar *,
															  const gchar *,
															  const gchar *);
static ConfigPair *config_method_get_pair (Config *,
														 const gchar *,
														 const gchar *,
														 const gchar *);
static ConfigBlock *config_method_get_block (Config *, const gchar *);
static ConfigPair *configblock_method_get_pair (ConfigBlock *, 
																const gchar *,
																const gchar *);
static ConfigRow *config_method_get_row (Config *,
													  const gchar *, 
													  const gchar *);
static ConfigRow *configblock_method_get_row (ConfigBlock *, const gchar *);
static gchar *configrow_method_parse (ConfigRow *, gchar *);
static void configrow_method_destroy (ConfigRow *);
static ConfigPair *configrow_method_get_pair (ConfigRow *, const gchar *);
static ConfigVector *configrow_method_get_vector (ConfigRow *, const gchar *);
static void configpair_method_destroy (ConfigPair *);
static void configblock_method_destroy (ConfigBlock *);
static void configvector_method_destroy (ConfigVector *);
static gint configvector_method_get_int (ConfigVector *, guint);
static gchar *configvector_method_get (ConfigVector *, guint);
static void configvector_method_add (ConfigVector *, const gchar *);
static gchar *configblock_method_parse (ConfigBlock *, gchar *, FILE *);
static void config_method_destroy (Config *);
static gint config_method_open (Config *);
static gint config_method_save (Config *, const gchar *);
static gint config_method_load (Config *, FILE *);

ConfigBlock *
configblock_new (Config * c, const gchar * tag)
{
	/* No NULL tags. */
	if (IS_NULLSTR (tag)) return NULL;
	return configblock_object_init (c, tag);
}

static gchar *
configblock_method_parse (ConfigBlock * block, gchar * p, FILE * fp)
{
	ASSERT (block != NULL);
	ASSERT (p != NULL);

	if (!fp || feof (fp)) return FALSE;

	gchar buf[1024], word[1024];
	gchar * q = NULL;
	g_stpcpy (word, p);

	/* This block of code makes sure that we indeed have an opening brace
		to continue with parsing of the configuration file. */
	do
	{
      p = word;
      
      if (EXPECT (p, q, '{'))
		{
			g_stpcpy (buf, q + 1);
			break;
		} 

      if (*q == '\0')
			continue;
      return NULL;
	} while (fgets (word, 1024, fp) != NULL);

	/* Start actually parsing rows now. */
	do 
	{
      p = buf;
      do
		{
			if (*p == '}')
				return p;
			else if (*p == '\n')
			{
				p++;
				continue;
			}
			else 
			{
				if ((q = parse (p, &word[0], 1024, ':')) != NULL)
				{
					if (*(q+1) == ':')
					{
						ConfigRow * row 
							= block->get_row (block, trim (&word[0]));
		      
						if (row)
						{
							fprintf (stderr, "**CONFIG**: row '%s' already"
										" defined in block '%s'; replacing\n",
										row->tag, block->tag);
							fflush (stderr);

							row->destroy (row);
						}

						row = configrow_new (block, trim (&word[0]));
		    
						if ((p = row->parse (row, q+2)) == NULL)
						{
							fprintf (stderr, "Error parsing line: %s", buf);
							fflush (stderr);
							return NULL;
						}
						break;
					}
				}
			}
			fprintf (stderr, "Error in formatting on line: %s\n", buf);
			fflush (stderr);
			return NULL;
		} while (*p != '\0');
	} while (fgets (buf, 1024, fp) != NULL);
	return NULL;
}

static void
configblock_method_destroy (ConfigBlock * block)
{
	ASSERT (block != NULL);

	DESTROY (ConfigRow, block->row_head);
	SINGLE_UNLINK (ConfigBlock, block->cfg->block_head, 
						block->cfg->block_tail, block);
  
	configblock_object_free (block);
}

static ConfigRow *
configblock_method_get_row (ConfigBlock * block, const gchar * row)
{
	ASSERT (block != NULL);

	if (!IS_NULLSTR (row))
	{
      ITERATE_BEGIN (ConfigRow, block->row_head);
      {
			if (!strcmp (it->tag, row))
				return it;
      }
      ITERATE_END ();
	}
	return NULL;
}

static ConfigPair *
configblock_method_get_pair (ConfigBlock * block, 
									  const gchar * row,
									  const gchar * key)
{
	ASSERT (block != NULL);

	if (!IS_NULLSTR (row))
	{
      ITERATE_BEGIN (ConfigRow, block->row_head);
      {
			if (!strcmp (it->tag, row))
				return it->get_pair (it, key);
      }
      ITERATE_END ();
	}
	return NULL;
}

static void
configblock_object_free (ConfigBlock * block)
{
	ASSERT (block != NULL);

	FREE (block->tag);
  
	FREE (block);
	block = NULL;
}

static ConfigBlock *
configblock_object_init (Config * c, const gchar * tag)
{
	ASSERT (c != NULL);

	ConfigBlock * b = NEW (ConfigBlock);

	/* Members */
	b->cfg = c;
	b->tag = g_strdup (tag);
	b->next = NULL; 
	b->row_head = b->row_tail = NULL;
  
	/* Methods */
	b->parse = configblock_method_parse;
	b->destroy = configblock_method_destroy;
	b->get_row = configblock_method_get_row;
	b->get_pair = configblock_method_get_pair;

	SINGLE_LINK (ConfigBlock, c->block_head, c->block_tail, b);
	return b;
}

Config *
config_new (const gchar * filename)
{
	return config_object_init (filename);
}

static void
config_method_destroy (Config * c)
{
	ASSERT (c != NULL);

	DESTROY (ConfigBlock, c->block_head);
  
	config_object_free (c);
}

static ConfigRow *
config_method_get_row (Config * cfg, const gchar * block, const gchar * row)
{
	ASSERT (cfg != NULL);
 
	ConfigBlock * b = cfg->get_block (cfg, block);
	if (b == NULL)
		return NULL;
	return b->get_row (b, row);
}

static ConfigBlock *
config_method_get_block (Config * cfg, const gchar * block)
{
	ASSERT (cfg != NULL);

	if (!IS_NULLSTR (block))
	{
      ITERATE_BEGIN (ConfigBlock, cfg->block_head);
      {
			if (!strcmp (it->tag, block))
				return it;
      }
      ITERATE_END ();
	}
	return NULL;
}

static ConfigPair *
config_method_get_pair (Config * cfg, 
								const gchar * block, 
								const gchar * row,
								const gchar * pair)
{
	ASSERT (cfg != NULL);

	ConfigRow * r = cfg->get_row (cfg, block, row);
	if (r == NULL)
		return NULL;
	return r->get_pair (r, pair);
}

static ConfigVector *
config_method_get_vector (Config * cfg,
								  const gchar * block,
								  const gchar * row,
								  const gchar * vector)
{
	ASSERT (cfg != NULL);
  
	ConfigRow * r = cfg->get_row (cfg, block, row);
	if (r == NULL)
		return NULL;
	return r->get_vector (r, vector);
}

static void
config_object_free (Config * c)
{
	ASSERT (c != NULL);

	FREE (c->filename);

	FREE (c);
	c = NULL;
}

static Config *
config_object_init (const gchar * filename)
{
	Config * c = NEW (Config);

	/* Members */
	c->filename = (!IS_NULLSTR (filename)) ? g_strdup (filename) : g_strdup ("");
	c->block_head = c->block_tail = NULL;

	/* Methods */
	c->open = config_method_open;
	c->save = config_method_save;
	c->load = config_method_load;
	c->close = config_method_destroy;
	c->get_vector = config_method_get_vector;
	c->get_row = config_method_get_row;
	c->get_block = config_method_get_block;
	c->get_pair = config_method_get_pair;

	return c;
}

static gint
config_method_load (Config * c, FILE * fp)
{
	ASSERT (c != NULL);
  
	if (!fp || feof(fp))
	{
      /* STUB: log me? */
      return FALSE;
	}

	gchar * buf = (gchar *)g_malloc (sizeof (char) * 1024);
	gchar word[1024];
	gchar * p = NULL, * q = NULL, * r = NULL;

	while ((p = fgets (buf, 1024, fp)) != NULL) {
      r = buf + (strlen (buf) - 1);
 
      while (p && (p < r) && (*p != '\n'))
		{
			if (*p == '%')
			{
				if ((q = parse (p+1, &word[0], 1023, ' ')) == NULL) {
					/* STUB: exit parsing with a failure. */
					g_free (buf);	
					return FALSE;
				}

				if (strcmp (word, "block") == 0)
				{
					if ((q = parse (q+1, &word[0], 1023, ' ')) == NULL) {
						g_free(buf);
						return FALSE;
					}
		  
					/* Replace the block if it already exists. Throw error. */
					ConfigBlock * b = c->get_block (c, trim (&word[0]));
					if (b)
					{
						fprintf (stderr, "**CONFIG**: block '%s' already"
									"exists. Destroying and replacing.\n", b->tag);
						fflush (stderr);
						b->destroy (b);
					}

					b = configblock_new (c, trim (&word[0]));

					if ((p = b->parse (b, q, fp)) == NULL) {
						g_free (buf);
						return FALSE;
					}
				}
				else if (strcmp (word, "include") == 0)
				{
					if ((p = parse (q+1, &word[0], 1023, ' ')) == NULL) {
						g_free (buf);
						return FALSE;
					}

					/* Include the file; we're going to load up this file
						first and then proceed with parsing. */
					FILE * ifp = NULL;
		 
					if ((ifp = fopen (trim (&word[0]), "r")) == NULL)
					{
						fprintf (stderr, "**CONFIG**: include '%s' does not"
									" seem to exist or unable to read.\n",
									word);
						fflush (stderr);
						continue;
					}

					c->load (c, ifp);
				}
				else {
					g_free (buf);
					return FALSE;
				}
			}
			/* BUGFIX: Need to check for tabs. */
			else if ((*p != ' ') && (*p != '\t'))
			{
				/* STUB: error out */
				g_free (buf);
				return FALSE;
			}

			p++;
		}
	}
	g_free (buf);
	return TRUE;
}

static gint
config_method_save (Config * c, const gchar * filename)
{
	ASSERT (c != NULL);

	if (IS_NULLSTR (filename)) return FALSE;

	FILE * fp = NULL;
	if ((fp = fopen (filename, "w")) == NULL)
	{
      /* STUB: log me? */
      return FALSE;
	}
  
	FCLOSE (fp);
	return TRUE;
}

static gint
config_method_open (Config * c)
{
	ASSERT (c != NULL);

	if (IS_NULLSTR (c->filename))
		return FALSE;

	FILE * fp = NULL;
	if ((fp = fopen (c->filename, "r")) == NULL)
	{
      /* STUB: log this? */
      return FALSE;
	}

	gint result = c->load (c, fp);
	if (result == FALSE)
	{
      /* STUB: log me? */
	}

	FCLOSE (fp);
	return result;
}

ConfigPair *
configpair_new (ConfigRow * row, const gchar * key, const gchar * value)
{
	return configpair_object_init (row, key, value);
}

static ConfigPair *
configpair_object_init (ConfigRow * row, 
								const gchar * key,
								const gchar * value)
{
	ASSERT (row != NULL);
	ASSERT (!IS_NULLSTR (key));
  
	ConfigPair * pair = NEW (ConfigPair);

	/* Members */
	pair->row = row;
	pair->key = g_strdup (key);
	pair->next = NULL;
	pair->value = (!IS_NULLSTR (value)) ? g_strdup (value) : g_strdup ("");
 
	/* Methods */
	pair->destroy = configpair_method_destroy;
  
	DOUBLE_LINK (row->pair_head, row->pair_tail, pair);
	return pair;
}

static void
configpair_method_destroy (ConfigPair * pair)
{
	ASSERT (pair != NULL);

	DOUBLE_UNLINK (pair);

	configpair_object_free (pair);
}

static void
configpair_object_free (ConfigPair * pair)
{
	ASSERT (pair != NULL);

	FREE (pair->key);
	FREE (pair->value);
	FREE (pair);
}

ConfigRow *
configrow_new (ConfigBlock * block, const gchar * tag)
{
	return configrow_object_init (block, tag);
}

static ConfigRow *
configrow_object_init (ConfigBlock * block, const gchar * tag)
{
	if (IS_NULLSTR (tag)) return NULL;

	ConfigRow * row = NEW (ConfigRow);
  
	/* Members */
	row->block = block;
	row->tag = g_strdup (tag);
	row->next = NULL;
	row->vector_head = row->vector_tail = NULL;
	row->pair_head = row->pair_tail = NULL;

	/* Methods */
	row->parse = configrow_method_parse;
	row->destroy = configrow_method_destroy;
	row->get_pair = configrow_method_get_pair;
	row->get_vector = configrow_method_get_vector;

	SINGLE_LINK (ConfigRow, block->row_head, block->row_tail, row);
	return row;
}

static void
configrow_object_free (ConfigRow *row)
{
	ASSERT (row != NULL);
  
	FREE (row->tag);

	DESTROY (ConfigPair, row->pair_head);
	DESTROY (ConfigVector, row->vector_head);

	FREE (row);
}

static ConfigPair *
configrow_method_get_pair (ConfigRow * row, const gchar * key)
{
	ASSERT (row != NULL);

	if (!IS_NULLSTR (key))
	{
      ITERATE_BEGIN (ConfigPair, row->pair_head);
      {
			if (!strcmp (it->key, key))
				return it;
      }
      ITERATE_END();
	}
	return NULL;
}

static ConfigVector *
configrow_method_get_vector (ConfigRow * row, const gchar * key)
{
	ASSERT (row != NULL);

	if (!IS_NULLSTR (key))
	{
      ConfigPair * pair = row->get_pair (row, key);
      if (IS_NULL (pair))
			return NULL;

      /* If we have already parsed it there is no reason to do it 
			a second time. Check existing linked list and return the
			pointer if we have already allocated it. */
      ITERATE_BEGIN (ConfigVector, row->vector_head);
      {
			if (!strcmp (it->tag, key))
				return it;
      }
      ITERATE_END();

      ConfigVector * vec = configvector_new (row, pair->key);
      gchar buf[1024];
      gchar * p = NULL;
		gchar * q = pair->value;
		gchar * r = pair->value + strlen (pair->value);

		do {
			p = parse (q,&buf[0],1024,',');
			vec->add (vec, trim (&buf[0]));
			q = p + 1;
		} while (q < r);
      return vec;
	}
	return NULL;
}

static gchar *
configrow_method_parse (ConfigRow * row, gchar * p)
{
	ASSERT (row != NULL);
	ASSERT (p != NULL);

	gchar key[1024], value[1024];
	gchar * q = NULL, * s = NULL;

	do
	{
      if ((q = parse (p, &key[0], 1024, '=')) == NULL)
		{
			break;
		}      

      if ((s = parse (q+1, &value[0], 1024, ';')) != NULL)
		{
			ConfigPair * pair = row->get_pair (row, trim (&key[0]));
			if (pair)
			{
				fprintf (stderr, "**CONFIG**: pair '%s:%s' already defined in"
							" row '%s' block '%s'; replacing\n",
							pair->key, pair->value, row->tag, row->block->tag);
				fflush (stderr);
				pair->destroy (pair);
			}

			pair = configpair_new (row, trim (&key[0]), trim (&value[0]));
		}
      p = s + 1;
	} while (*p != '\n' && *p != '}' && *p != '\0');
	return p;
}

static void 
configrow_method_destroy (ConfigRow * row)
{
   ASSERT (row != NULL);

   SINGLE_UNLINK (ConfigRow, row->block->row_head, row->block->row_tail, row);

	configrow_object_free (row);
}

ConfigVector *
configvector_new (ConfigRow * row, const gchar * tag)
{
	ASSERT (row != NULL);

	/* No NULL Tags. */
	if (IS_NULLSTR (tag)) return NULL;
	return configvector_object_init (row, tag);
}

static ConfigVector *
configvector_object_init (ConfigRow * row, const gchar * tag)
{
	ASSERT (row != NULL);

	ConfigVector * vec = NEW (ConfigVector);

	/* Members */
	vec->row = row;
	vec->tag = g_strdup (tag);
	vec->next = vec->prev = NULL;
	vec->array = g_ptr_array_sized_new (7);

	/* Methods */
	vec->destroy = configvector_method_destroy;
	vec->get_int = configvector_method_get_int;
	vec->get = configvector_method_get;
	vec->add = configvector_method_add;

	DOUBLE_LINK (row->vector_head, row->vector_tail, vec);
	return vec;
}

static void 
configvector_object_free (ConfigVector * vec)
{
	ASSERT (vec != NULL);

	FREE (vec->tag);

	if (vec->array != NULL)
		g_ptr_array_free (vec->array, TRUE);

	FREE (vec);
}

static void
configvector_method_destroy (ConfigVector * vec)
{
	ASSERT (vec != NULL);

	DOUBLE_UNLINK (vec);

	configvector_object_free (vec);
}

static gchar *
configvector_method_get (ConfigVector * vec, guint index)
{
	ASSERT (vec != NULL);

	if (index >= vec->array->len) return NULL;

	gchar * rvalue = (gchar *) g_ptr_array_index (vec->array, index);
	return rvalue;
}

static gint
configvector_method_get_int (ConfigVector * vec, guint index)
{
	ASSERT (vec != NULL);

	if (index >= vec->array->len) return -1;

	gchar * rvalue = (gchar *)g_ptr_array_index (vec->array, index);
	return atoi (rvalue);
}

static void
configvector_method_add (ConfigVector * vec, const gchar * value)
{
	ASSERT (vec != NULL);
	ASSERT (value != NULL);

	gchar * ptr = g_strdup (value);
	g_ptr_array_add (vec->array, ptr);
}

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
#ifndef H_CONFIG
#define H_CONFIG
#ifdef __cplusplus
extern "C" {
#endif

typedef struct _Config Config;
typedef struct _ConfigBlock ConfigBlock;
typedef struct _ConfigRow ConfigRow;
typedef struct _ConfigPair ConfigPair;
typedef struct _ConfigVector ConfigVector;

#include <header.h>
#include <cstdlib>

struct _ConfigPair
{
  /* Members */
  ConfigRow * row;
  ConfigPair * next;
  ConfigPair * prev;
  gchar * key;
  gchar * value;

  /* Methods */
  void (*destroy) (ConfigPair *);
};

struct _ConfigVector
{
  /* Members */
  ConfigRow * row;
  ConfigVector * next;
  ConfigVector * prev;
  GPtrArray * array;
  gchar * tag;

  /* Methods */
  void (*add) (ConfigVector *, const gchar *);
  gint (*get_int) (ConfigVector *, guint);
  gchar *(*get) (ConfigVector *, guint);
  void (*destroy) (ConfigVector *);
};

struct _ConfigRow
{
  /* Members */
  ConfigBlock * block;
  ConfigRow * next;
  ConfigVector * vector_head;
  ConfigVector * vector_tail;
  ConfigPair * pair_head;
  ConfigPair * pair_tail;
  gchar * tag;

  /* Methods */
  ConfigPair *(*get_pair) (ConfigRow *, const gchar *);
  ConfigVector *(*get_vector) (ConfigRow *, const gchar *);
  gchar *(*parse) (ConfigRow *, gchar *);
  void (*destroy) (ConfigRow *);
};

struct _ConfigBlock
{
  /* Members */
  ConfigBlock * next;
  Config * cfg;
  ConfigRow * row_head;
  ConfigRow * row_tail;
  gchar * tag;

  /* Methods */
  gchar *(*parse) (ConfigBlock *, gchar *, FILE *);
  void (*destroy) (ConfigBlock *);
  ConfigRow *(*get_row) (ConfigBlock *, const gchar *);
  ConfigPair *(*get_pair) (ConfigBlock *, const gchar *, const gchar *);
};

struct _Config
{
  /* Members */
  gchar * filename;
  ConfigBlock * block_head;
  ConfigBlock * block_tail;

  /* Methods */
  gint (*open) (Config *);
  gint (*save) (Config *, const gchar *);
  gint (*load) (Config *, FILE *);
  void (*close) (Config *);
  ConfigVector *(*get_vector) (Config *,
			       const gchar *,
			       const gchar *,
			       const gchar *);
  ConfigPair *(*get_pair) (Config *, 
			   const gchar *, 
			   const gchar *, 
			   const gchar *);
  ConfigBlock *(*get_block) (Config *, 
			     const gchar *);
  ConfigRow *(*get_row) (Config *, const gchar *, const gchar *);
};

/* config.c */
ConfigBlock *configblock_new (Config *, const gchar *);
Config *config_new (const gchar *);
ConfigRow *configrow_new (ConfigBlock *, const gchar *);
ConfigPair *configpair_new (ConfigRow *, const gchar *, const gchar *);
ConfigVector *configvector_new (ConfigRow *, const gchar *);

#ifdef __cplusplus
}
#endif
#endif /*H_CONFIG*/

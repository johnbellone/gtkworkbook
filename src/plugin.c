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
#include "plugin.h"
#include <stdio.h>
#include <stdlib.h>

/* plugin.c (static) */
static Plugin *plugin_object_init (void);
static void plugin_method_destroy (Plugin *);
static void *plugin_method_symbol_register (Plugin *, gchar *);
static void plugin_method_symbol_deregister (Plugin *, PluginMethod *);
static Plugin *plugin_object_free (Plugin *);
static PluginMethod *pluginmethod_object_init (Plugin *);
static PluginMethod *pluginmethod_object_free (PluginMethod *);
static void pluginmethod_method_deregister (PluginMethod *, Plugin *);
static GThread *plugin_method_create_thread (Plugin *,
															GThreadFunc,
															gpointer);

Plugin *
plugin_open (const gchar * filename)
{
	if (!filename || (*filename == '\0'))
		return NULL;

	Plugin * plug = plugin_object_init ();

	if ((plug->pfnhandle = LIBRARY_OPEN(filename)) == NULL)
	{
      fprintf (stderr, "%s\n", LIBRARY_ERROR());
      exit (1);
	}
	return plug;
}

static Plugin *
plugin_object_init (void)
{
	Plugin * plug = NEW (Plugin);
  
	/* Members */
	plug->pfnhandle = NULL;
	plug->next = NULL;
	plug->prev = NULL;
	plug->first = NULL;
	plug->last = NULL;
	plug->threads = g_ptr_array_new ();

	/* Methods */
	plug->destroy = plugin_method_destroy;
	plug->method_register = plugin_method_symbol_register;
	plug->method_deregister = plugin_method_symbol_deregister;
	plug->create_thread = plugin_method_create_thread;

	return plug;
}

static GThread *
plugin_method_create_thread (Plugin * plugin,
									  GThreadFunc thread_function,
									  gpointer data)
{
	ASSERT (plugin != NULL);

	GThread * runnable = NULL;
	GError * err = NULL;

	if ((runnable = g_thread_create (thread_function, data,
												TRUE, &err)) == NULL)
	{
      g_warning ("Failed creating thread; ERROR %s", err->message);
      g_error_free (err);
      return NULL;
	}
	g_ptr_array_add (plugin->threads, (gpointer)runnable);
	return runnable;
} 

static Plugin *
plugin_object_free (Plugin * plugin)
{
	if (!plugin)
		return NULL;

	plugin->first = plugin->last = NULL;

	g_ptr_array_free (plugin->threads, TRUE);

	FREE (plugin);
	return plugin;
}

static void
plugin_method_destroy (Plugin * plugin)
{
	if (!plugin)
		return;

	PluginMethod * current = plugin->first, * next = NULL;
	while (current)
	{
      next = current->next;
      current->deregister (current, plugin);
      current = next;
	}

	for (guint ii = 0; ii < plugin->threads->len; ii++)
	{
      GThread * thread 
			= (GThread *)g_ptr_array_remove_index_fast (plugin->threads, ii);
      g_thread_join (thread);
	}

	UNLINK_OBJECT (plugin);
	LIBRARY_CLOSE (plugin->pfnhandle);
	plugin_object_free (plugin);
}

static void
plugin_method_symbol_deregister (Plugin * plugin, PluginMethod * method)
{
	if (!plugin || !method)
		return;

	method->deregister (method, plugin);
}

static void *
plugin_method_symbol_register (Plugin * plugin, gchar * symbol)
{
	if (IS_NULLSTR (symbol))
		return NULL;

	PluginMethod * plugm = pluginmethod_object_init (plugin);

	if ((plugm->pfnmethod 
		  = LIBRARY_SYM(plugin->pfnhandle, symbol)) == NULL)
	{
      fprintf (stderr, "%s\n", LIBRARY_ERROR());
      exit (1);
	}

	LINK_OBJECT (plugin->first, plugin->last, plugm); 
	return plugm->pfnmethod;
}

static PluginMethod *
pluginmethod_object_init (Plugin * plugin)
{
	PluginMethod * plugm = NEW (PluginMethod);

	/* Members */
	plugm->pfnmethod = NULL;
	plugm->plugin = plugin;

	/* Methods */
	plugm->deregister = pluginmethod_method_deregister;

	plugm->next = NULL;
	plugm->prev = NULL;
	return plugm;
}

static PluginMethod *
pluginmethod_object_free (PluginMethod * method)
{
	if (!method)
		return NULL;

	FREE (method);
	return method;
}

static void
pluginmethod_method_deregister (PluginMethod * method, Plugin * plugin)
{
	if (!method)
		return;

	UNLINK_OBJECT (method);
	pluginmethod_object_free (method);
}

#include "Application.hpp"

Application::Application (int argc, char ** argv) {
	this->init (argc, argv);
	this->cfg = NULL;
	this->gtk_window = NULL;
	this->gtk_menu = NULL;
	this->gtk_window_vbox = NULL;
	
	/* Set up the signals. */
	this->signals[SIG_NOTEBOOK_SWITCHED]
		= (GSourceFunc)signal_gtknotebook_switchpage;
	this->signals[SIG_NOTEBOOK_REORDERED]
		= (GSourceFunc)signal_gtknotebook_reordered;
	this->signals[SIG_NOTEBOOK_REMOVED]
		= (GSourceFunc)signal_gtknotebook_removed;
	this->signals[SIG_DESTROY_EVENT]
		= (GSourceFunc)signal_destroy_event;
	this->signals[SIG_DELETE_EVENT]
		= (GSourceFunc)signal_delete_event;
	this->signals[SIG_SHEET_CHANGED]
		= (GSourceFunc)signal_gtksheet_changed;
}

void
Application::init (int argc, char ** argv) {

	if (this->cfg) {
		Config * cfg = appstate->cfg;
      cfg->open (cfg);
     
      /* Load extensions */
      ConfigRow * load = cfg->get_row (cfg, "application", "load");

      /* Did a little bit of fixing here. We needed something to specify that
			certain extensions will start automatically when the application is
			started. */
      if (!IS_NULL (load)) {
			ConfigVector * ext = load->get_vector (load, "extensions");
			gchar * block = NULL;
			gint ii = 0;

			if (IS_NULL (ext)) {
				g_critical ("Config 'extensions' vector is NULL");
				return -1;
			}
      
			while ((block = ext->get(ext, ii)) != NULL) {
				ConfigPair * run = cfg->get_pair (cfg, block, "onLoad", "run");
	      
				if (run && (strcmp (run->value, "1") == 0)) {
					ConfigPair * filename 
						= cfg->get_pair (cfg, block, "linux", "filename");

					if (!IS_NULL (filename) && !IS_NULLSTR (filename->value)) {
						gboolean RelativePath 
							= (filename->value[0] == '/') ? FALSE : TRUE;

						appstate->open_extension (appstate, 
														  filename->value, 
														  RelativePath);
					}
				}
				ii++;
			}
		}
	}

	
}

int
Application::run (void) {
	if (proactor.start() == false) {
		g_critical ("Failed to start the proactor thread; exiting application.");
		return -1;
	}
	
	/* Start the GTK+ main loop; make sure it is surrounded in the 
		thread calls. GTK+ "releases" the current lock after every loop
		interation. This allows us to call gdk_threads_enter/leave inside
		of another thread. */
	gdk_threads_enter ();
	gtk_main ();
	gdk_threads_leave ();
	return 0;
}


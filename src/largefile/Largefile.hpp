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
#ifndef HPP_LARGEFILE
#define HPP_LARGEFILE

#include <libgtkworkbook/workbook.h>
#include <map>
#include <string>
#include "FileDispatcher.hpp"
#include "GotoDialog.hpp"
#include "../Plugin.hpp"
#include "../Application.hpp"
#include "../config.h"

namespace largefile {

	/***
	 * \class Largefile
	 * \defgroup Largefile
	 * \ingroup Largefile
	 * \author jb (jvb4@njit.edu)
	 * \brief
	 */
	class Largefile : public Plugin {
	private:
		typedef std::map<std::string,AbstractFileDispatcher *> FilenameMap;

		GotoDialog goto_dialog;
		FILE * pktlog;
		FilenameMap mapping;
		GSList * gtk_togglegroup;
		
		GtkWidget * CreateMainMenu (void);
		GtkWidget * CreateStatusBar (void);
	public:
		Largefile (Application * appstate, Handle * platform);
		virtual ~Largefile (void);

		GtkWidget * BuildLayout (void);
				
		bool OpenFile (Sheet * sheet, const std::string & filename);
		bool CloseFile (const std::string & filename);
		bool Readline (Sheet * sheet, off64_t start, off64_t N);
		bool Readoffset (Sheet * sheet, off64_t offset, off64_t N);
		bool Readpercent (Sheet * sheet, guint percent, off64_t N);
		
		inline void setGotoDialogRadioGroup (GSList * group) { this->gtk_togglegroup = group; }
		inline GotoDialog * gotodialog() { return &this->goto_dialog; }
	};

}
	
#endif

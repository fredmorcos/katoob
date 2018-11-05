/*
 * textview.cc
 * This file is part of katoob
 *
 * Copyright (C) 2002-2007 Mohammed Sameer
 * Copyright (C) 2008-2018 Frederic-Gerald Morcos
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include "textview.hh"
#include "dialogs.hh"
// TODO: We can't drop if the document is set to readonly.

TextView::TextView() {
}

TextView::~TextView() {
}

TextView::TextView(GtkTextView *tv) : Gtk::TextView(tv) {
}

// TODO: Need to check this under maemo but how ?
void TextView::on_drag_data_received(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, const Gtk::SelectionData& selection_data, guint info, guint time) {
  bool handle = false;
  std::vector<std::string> targets = context->get_targets();
  for (unsigned i = 0; i < targets.size(); i++) {
    if (targets[i] == "text/uri-list") {
      handle = true;
      break;
    }
  }

  if (handle) {
#ifdef GLIBMM_EXCEPTIONS_ENABLED
    try {
      std::string filename = Glib::filename_from_uri(selection_data.get_data_as_string());
      signal_text_view_request_file_open.emit(filename);
    }
    catch(Glib::ConvertError& e) {
      katoob_error(e.what());
    }
#else
    std::auto_ptr<Glib::Error> error;
    std::string filename = Glib::filename_from_uri(selection_data.get_data_as_string(), error);
    if (error.get()) {
      katoob_error(error->what());
    }
    else {
      signal_text_view_request_file_open.emit(filename);
    }
#endif
  }
  else {
#ifdef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
    // FIXME: maemo
    Gtk::TextView::on_drag_data_received(context, x, y, selection_data, info, time);
#endif
  }
}

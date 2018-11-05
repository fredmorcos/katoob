/*
 * textview.hh
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

#pragma once

#include <gtkmm/textview.h>
#include <gtk/gtktextview.h>

class TextView : public Gtk::TextView {
public:
  TextView();
  TextView(GtkTextView *);

  virtual ~TextView();
  sigc::signal<void, std::string> signal_text_view_request_file_open;

  // They are not protected because we need to call them from our SourceView and I don't
  // want to include sourceview.hh and declare them as friends.
  virtual void on_drag_data_received(const Glib::RefPtr<Gdk::DragContext>&,
                                     int, int, const Gtk::SelectionData&, guint, guint);
};

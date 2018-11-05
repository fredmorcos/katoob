/*
 * window.hh
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

#include <vector>
#include <string>
#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include "menubar.hh"
#include "toolbar.hh"
#include "mdi.hh"
#include "statusbar.hh"
#include "conf.hh"
#include "encodings.hh"
#include "emulator.hh"
#include "multipress.hh"
#include "inputwindow.hh"

class Window : public Gtk::Window {
public:
  Window(Conf&, Encodings&, std::vector<std::string>&);
  ~Window();

  // Our signals.
  sigc::signal<void> signal_quit;

  // Our methods.
#ifdef ENABLE_DBUS
  void open_files(std::vector<std::string>&);
#endif

  void on_document_spell_enabled_cb(bool);

  void autosave();

private:
  // Our menubar and tolbar callbacks
  void signal_wrap_text_activate_cb(bool);
  void signal_line_numbers_activate_cb(bool);
  void signal_layout_activate_cb(int, int);
  void signal_encoding_activate_cb(int);
  void signal_preferences_activate_cb();
  void signal_quit_activate_cb();
  void signal_dictionary_changed_cb(std::string);
  void signal_search_activated_cb(std::string);

  void signal_input_toggled_cb(bool);

  // Signals for various gtk events.
  void signal_drag_data_received_cb(const Glib::RefPtr<Gdk::DragContext>&, int, int, const Gtk::SelectionData&, guint, guint);
  bool signal_delete_event_cb (GdkEventAny*);

  // Signal callbacks for our Multipress.
  void signal_insert_key_cb(std::string&);
  void signal_invalid_key_cb(GdkEventKey *);

  // Our methods.
  void connect_toolbar_signals();
  void connect_menubar_signals();
  void connect_mdi_signals();
  void set_title(const char * = NULL);
  void reset_gui();

  // Signals for the Document class.
  void signal_document_encoding_changed_cb(int e);
  void signal_document_overwrite_toggled_cb(bool o) { statusbar.set_overwrite(o); }
  void signal_document_cursor_moved_cb(int c, int l) { statusbar.set_position(c, l); }
  void signal_document_file_changed_cb(std::string);
  void signal_document_readonly_cb(int, bool);
  void signal_document_can_redo_cb(bool);
  void signal_document_can_undo_cb(bool);
  void signal_document_modified_cb(int, bool);
  void signal_document_title_changed_cb(std::string, int);
  void signal_document_wrap_text_cb(bool);
  void signal_document_line_numbers_cb(bool);

  void signal_document_dictionary_changed_cb(std::string d) { toolbar.set_dictionary(d); }

  void signal_input_window_dialog_closed_cb();

  void on_doc_activated(int);
  void on_reset_gui(int);

  void on_document_added_cb(bool, bool, std::string);
  void on_document_removed_cb(int);

  // Our members.
  Emulator _emulator;
  Multipress _multipress;

  Conf& _conf;
  Encodings& _encodings;

  MenuBar menubar;
  Toolbar toolbar;
  MDI mdi;
  Statusbar statusbar;

  Gtk::VBox box;

  InputWindow input_window;

  sigc::connection signal_auto_spell_activate_conn;

  sigc::connection signal_wrap_text_activate_conn;
  sigc::connection signal_line_numbers_activate_conn;
};

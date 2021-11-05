/*
 * window.hh
 *
 * This file is part of Katoob.
 *
 * Copyright (C) 2008-2021 Fred Morcos <fm+Katoob@fredmorcos.com>
 * Copyright (C) 2002-2007 Mohammed Sameer <msameer@foolab.org>
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if
 * not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#pragma once

#include "conf.hh"
#include "encodings.hh"
#include "mdi.hh"
#include "menubar.hh"
#include "statusbar.hh"
#include "toolbar.hh"
#include <gtkmm.h>
#include <string>
#include <vector>

#ifdef ENABLE_EMULATOR
#include "emulator.hh"
#endif

#ifdef ENABLE_MULTIPRESS
#include "multipress.hh"
#endif

#if defined(ENABLE_EMULATOR) || defined(ENABLE_MULTIPRESS)
#include "inputwindow.hh"
#endif

#ifdef ENABLE_MAEMO
#include <hildonmm/window.h>
class Window: public Hildon::Window {
#else
class Window: public Gtk::Window {
#endif

 public:
  Window(Conf &, Encodings &, std::vector<std::string> &);
  ~Window();

  // Our signals.
  sigc::signal<void> signal_quit;

  // Our methods.
#ifdef ENABLE_DBUS
  void open_files(std::vector<std::string> &);
#endif

#ifdef ENABLE_SPELL
  void on_document_spell_enabled_cb(bool);
#endif

  void autosave();
#ifdef ENABLE_MAEMO
  void signal_request_top_cb();
#endif

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
#if defined(ENABLE_EMULATOR) || defined(ENABLE_MULTIPRESS)
  void signal_input_toggled_cb(bool);
#endif
#ifdef ENABLE_MAEMO
  void toggle_full_screen();
#endif

  // Signals for various gtk events.
#ifdef ENABLE_MAEMO
  bool signal_window_state_event_cb(GdkEventWindowState *);
#endif
  void signal_drag_data_received_cb(const Glib::RefPtr<Gdk::DragContext> &,
                                    int,
                                    int,
                                    const Gtk::SelectionData &,
                                    guint,
                                    guint);
  bool signal_delete_event_cb(GdkEventAny *);

  // Signal callbacks for our Multipress.
  void signal_insert_key_cb(std::string &);
  void signal_invalid_key_cb(GdkEventKey *);

  // Our methods.
  void connect_toolbar_signals();
  void connect_menubar_signals();
  void connect_mdi_signals();
  void set_title(const char * = NULL);
  void reset_gui();

  // Signals for the Document class.
  void signal_document_encoding_changed_cb(int e);
  void signal_document_overwrite_toggled_cb(bool o)
  {
    statusbar.set_overwrite(o);
  }
  void signal_document_cursor_moved_cb(int c, int l)
  {
    statusbar.set_position(c, l);
  }
  void signal_document_file_changed_cb(std::string);
  void signal_document_readonly_cb(int, bool);
  void signal_document_can_redo_cb(bool);
  void signal_document_can_undo_cb(bool);
  void signal_document_modified_cb(int, bool);
  void signal_document_title_changed_cb(std::string, int);
  void signal_document_wrap_text_cb(bool);
  void signal_document_line_numbers_cb(bool);
#ifdef ENABLE_SPELL
  void signal_document_dictionary_changed_cb(std::string d)
  {
    toolbar.set_dictionary(d);
  }
#endif
#if defined(ENABLE_EMULATOR) || defined(ENABLE_MULTIPRESS)
  void signal_input_window_dialog_closed_cb();
#endif
  void on_doc_activated(int);
  void on_reset_gui(int);

  void on_document_added_cb(bool, bool, std::string);
  void on_document_removed_cb(int);

  // Our members.
#ifdef ENABLE_EMULATOR
  Emulator _emulator;
#endif
#ifdef ENABLE_MULTIPRESS
  Multipress _multipress;
#endif
  Conf &_conf;
  Encodings &_encodings;

  MenuBar menubar;
  Toolbar toolbar;
  MDI mdi;
  Statusbar statusbar;

  Gtk::VBox box;
#if defined(ENABLE_EMULATOR) || defined(ENABLE_MULTIPRESS)
  InputWindow input_window;
#endif
#ifdef ENABLE_SPELL
  sigc::connection signal_auto_spell_activate_conn;
#endif
  sigc::connection signal_wrap_text_activate_conn;
  sigc::connection signal_line_numbers_activate_conn;

#ifdef ENABLE_MAEMO
  bool is_fullscreen;
#endif
};

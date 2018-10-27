/*
 * window.hh
 * This file is part of katoob
 *
 * Copyright (C) 2006, 2007, 2008 Mohammed Sameer
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

#ifndef __WINDOW_HH__
#define __WINDOW_HH__

#include <vector>
#include <string>
#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include "mdi.hh"
#include "statusbar.hh"
#include "conf.hh"
#include "encodings.hh"

#ifdef ENABLE_MAEMO
#include <hildonmm/window.h>
#endif

// forward declaration
class Bars;

#ifdef ENABLE_MAEMO
class Window : public Hildon::Window {
#else
class Window : public Gtk::Window {
#endif
public:
  Window(Conf&, Encodings&, std::vector<std::string>&);
  ~Window();

  // Our signals.
  sigc::signal<void> signal_quit;

  // Our methods.
#ifdef ENABLE_DBUS
  void open_files(std::vector<std::string>&);
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

#ifdef ENABLE_MAEMO
  void toggle_full_screen();
#endif

  // Signals for various gtk events.
#ifdef ENABLE_MAEMO
  bool signal_window_state_event_cb(GdkEventWindowState *);
#endif
  void signal_drag_data_received_cb(const Glib::RefPtr<Gdk::DragContext>&, int, int, const Gtk::SelectionData&, guint, guint);
  bool signal_delete_event_cb (GdkEventAny*);

  // Our methods.
  /**
   * \brief connect the Bars signals to the respective callbacks.
   */
  void connect_bars_signals();

  /**
   * \brief connect the MDI signals to the respective callbacks.
   */
  void connect_mdi_signals();

  /**
   * \brief connect other signals (statusbar, input methods, DnD, ...) to the respective callbacks.
   */
  void connect_other_signals();

  /**
   * \brief set the main Window icon from a file. It can still fail to do so.
   * \param file the path to the icon.
   */
  void set_icon_from_file(const std::string& file);

  void set_title(const char * = NULL);
  void reset_gui();

  // Signals for the Document class.
  void signal_document_encoding_changed_cb(int e);
  void signal_document_file_changed_cb(std::string);
  void signal_document_rom_cb(int, bool, bool);
  void signal_document_title_changed_cb(std::string, int);

  void on_doc_activated(int);
  void on_reset_gui(bool);

  void on_document_added_cb(Document *);
  void on_document_removed_cb(int, int);

  // Our members.
  Conf& _conf;
  Encodings& _encodings;

  Bars *bars;
  MDI mdi;
  Statusbar statusbar;

  std::vector<Import *> _import;
  std::vector<Export *> _export;

  Gtk::VBox box;
#ifdef ENABLE_SPELL
  sigc::connection signal_auto_spell_activate_conn;
#endif
  sigc::connection signal_wrap_text_activate_conn;
  sigc::connection signal_line_numbers_activate_conn;

#ifdef ENABLE_MAEMO
  bool is_fullscreen;
#endif
};

#endif /* __WINDOW_HH__ */

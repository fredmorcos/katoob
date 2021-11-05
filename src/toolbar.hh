/*
 * toolbar.hh
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
#include <gtkmm.h>

#ifdef ENABLE_SPELL
#include "spellmenu.hh"
#endif

class Toolbar {
 public:
  Toolbar(Conf &);
  ~Toolbar();

  Gtk::Toolbar &get_main();
  Gtk::VBox &get_extended();
  void show_main(bool);
  void show_extended(bool);

  // Our methods.
  void enable_undo(bool);
  void enable_redo(bool);
  void enable_dictionary(bool);
  void set_read_only(bool);
  void set_text();
  void set_icons();
  void set_both();
  void set_beside();
#ifdef ENABLE_SPELL
  void set_dictionary(std::string &);
  std::string get_dictionary();
#endif
  void reset_gui();
  void reset_gui(bool);

  // Our signals.
  sigc::signal<void> signal_create_clicked;
  sigc::signal<void> signal_open_clicked;
  sigc::signal<void> signal_save_clicked;
#ifdef ENABLE_PRINT
  sigc::signal<void> signal_print_clicked;
#endif
  sigc::signal<void> signal_close_clicked;
  sigc::signal<void> signal_undo_clicked;
  sigc::signal<void> signal_redo_clicked;
  sigc::signal<void> signal_cut_clicked;
  sigc::signal<void> signal_copy_clicked;
  sigc::signal<void> signal_paste_clicked;
  sigc::signal<void> signal_erase_clicked;
#ifdef ENABLE_MAEMO
  sigc::signal<void> signal_full_screen_clicked;
#endif

  sigc::signal<void, int> signal_go_to_activated;
  sigc::signal<void, std::string> signal_search_activated;
#ifdef ENABLE_SPELL
  sigc::signal<void> signal_spell_clicked;
  sigc::signal<void, std::string> signal_dictionary_changed;
#endif
  sigc::signal<void, std::string> signal_extra_button_clicked;

 private:
  void create_main();
  void create_extended();

  void search_activate_cb();
  void go_to_activate_cb();

  void create_extra_buttons();
  void create_extra_button(std::string &);
  void extra_button_clicked(std::string str)
  {
    signal_extra_button_clicked.emit(str);
  }

#ifdef ENABLE_SPELL
  void dictionary_changed_cb()
  {
    signal_dictionary_changed.emit(_dictionary.get_active_text());
  }
  sigc::connection signal_dictionary_changed_conn;
#endif

  Conf &_conf;

  Gtk::Toolbar _main;
  Gtk::VBox _extended;
  Gtk::Toolbar _extra_buttons;

  Gtk::SeparatorToolItem s1, s2, s3;

  Gtk::ToolButton _create;
  Gtk::ToolButton _open;
  Gtk::ToolButton _save;
#ifdef ENABLE_PRINT
  Gtk::ToolButton _print;
#endif
  Gtk::ToolButton _close;
  Gtk::ToolButton _undo;
  Gtk::ToolButton _redo;
  Gtk::ToolButton _cut;
  Gtk::ToolButton _copy;
  Gtk::ToolButton _paste;
  Gtk::ToolButton _erase;
#ifdef ENABLE_MAEMO
  Gtk::ToolButton _full_screen;
#endif

  /* Extended */
  Gtk::HBox box;
  Gtk::HSeparator sep;
  Gtk::Label _go_to_l, _search_l;
#ifdef ENABLE_SPELL
  Gtk::Label _dictionary_l;
  Gtk::Button _spell;
  SpellMenu _dictionary;
#endif
  Gtk::Entry _go_to, _search;
};

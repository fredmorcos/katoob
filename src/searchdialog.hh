/*
 * searchdialog.hh
 *
 * This file is part of Katoob.
 *
 * Copyright © 2008-2021 Fred Morcos <fm+Katoob@fredmorcos.com>
 * Copyright © 2002-2007 Mohammed Sameer <msameer@foolab.org>
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

#include <gtkmm.h>

class SearchDialog {
 public:
  SearchDialog();
  ~SearchDialog();
  bool run();

  bool whole_word();
  bool match_case();
  bool wrap();
  bool backwards();
  bool beginning();
  std::string get_text();

  void whole_word(bool);
  void match_case(bool);
  void wrap(bool);
  void backwards(bool);
  void beginning(bool);
  void set_text(std::string &);

 private:
  //  void insert_at_cursor_cb(const Glib::ustring&);

  Gtk::Dialog dialog;
  // TODO: Use a drop down combo or something.
  Gtk::Entry what;
  /*  Gtk::Entry with; */
  Gtk::Label label;
  Gtk::CheckButton _whole_word;
  Gtk::CheckButton _match_case;
  //  Gtk::CheckButton regex;
  Gtk::CheckButton _wrap;
  Gtk::CheckButton _cursor;
  Gtk::CheckButton _backwards;
  Gtk::CheckButton _beginning;
  Gtk::HBox hbox;
  Gtk::VBox vbox;
  Gtk::Button *find;
};

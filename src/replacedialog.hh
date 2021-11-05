/*
 * replacedialog.hh
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

#include <gtkmm.h>

class ReplaceDialog {
 public:
  ReplaceDialog();
  ~ReplaceDialog();
  void run();

  bool whole_word()
  {
    return _whole_word.get_active();
  }
  bool match_case()
  {
    return _match_case.get_active();
  }
  bool wrap()
  {
    return _wrap.get_active();
  }
  bool backwards()
  {
    return _backwards.get_active();
  }
  bool beginning()
  {
    return _beginning.get_active();
  }
  std::string get_text()
  {
    return what.get_text();
  }
  std::string get_replace()
  {
    return with.get_text();
  }

  void whole_word(bool st)
  {
    _whole_word.set_active(st);
  }
  void match_case(bool st)
  {
    _match_case.set_active(st);
  }
  void wrap(bool st)
  {
    _wrap.set_active(st);
  }
  void backwards(bool st)
  {
    _backwards.set_active(st);
  }
  void beginning(bool st)
  {
    _beginning.set_active(st);
  }
  void set_text(std::string &text)
  {
    what.set_text(text);
  }
  void set_replace(std::string &text)
  {
    with.set_text(text);
  }
  void close()
  {
    _close.clicked();
  }

  sigc::signal<bool> signal_find;
  sigc::signal<void> signal_replace;
  sigc::signal<void> signal_replace_all;

 private:
  void on_find_clicked()
  {
    if (!signal_find.emit()) {
      close();
    }
  }
  void on_replace_clicked()
  {
    signal_replace.emit();
  }
  /*
  void on_find_replace_clicked() {
    if (signal_find.emit())
      signal_replace.emit();
    else
      close();
  }
  */
  void on_replace_all_clicked()
  {
    signal_replace_all.emit();
    close();
  }
  void on_close_clicked()
  {
    dialog.response(0);
  }

  Gtk::Dialog dialog;
  // TODO: Use a drop down combo or something.
  Gtk::Entry what;
  Gtk::Entry with;
  Gtk::Label label, label2;
  Gtk::CheckButton _whole_word;
  Gtk::CheckButton _match_case;
  Gtk::CheckButton _wrap;
  Gtk::CheckButton _cursor;
  Gtk::CheckButton _backwards;
  Gtk::CheckButton _beginning;
  Gtk::HBox hbox, hbox2;
  Gtk::VBox vbox;
  Gtk::Button find, replace, /* find_replace, */ replace_all, _close;
};

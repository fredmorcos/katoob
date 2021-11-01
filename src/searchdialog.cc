/*
 * searchdialog.cc
 * This file is part of katoob
 *
 * Copyright (C) 2006, 2007 Mohammed Sameer
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

#include "macros.h"
#include "searchdialog.hh"
#include <gtkmm.h>

SearchDialog::SearchDialog()
    : dialog(_("Find"), true), label(_("Search for:")),
      _whole_word(_("Match entire word only")), _match_case(_("Match case")),
      //  regex(_("Regular expression")),
      _wrap(_("Wrap search")), _cursor(_("Search from the cursor position")),
      _backwards(_("Search backwards")),
      _beginning(_("Search from the beginnig of the document")) {
  dialog.set_border_width(10);

  dialog.set_position(Gtk::WIN_POS_CENTER);

  Gtk::Box *box = dialog.get_vbox();

  box->pack_start(hbox);
  hbox.pack_start(label, false, false, 10);
  hbox.pack_start(what, true, true, 10);
  box->pack_start(_beginning, true, true);
  box->pack_start(_whole_word, true, true);
  box->pack_start(_match_case, true, true);
  //  box->pack_start(regex, true, true);
  box->pack_start(_wrap, true, true);
  box->pack_start(_backwards, true, true);

  dialog.add_button(Gtk::Stock::CLOSE, Gtk::RESPONSE_CLOSE);
  find = dialog.add_button(Gtk::Stock::FIND, Gtk::RESPONSE_OK);

  what.signal_activate().connect(sigc::mem_fun(find, &Gtk::Button::clicked));
  // find->set_sensitive(false);
  //  what.signal_insert_at_cursor().connect(sigc::mem_fun(*this,
  //  &SearchDialog::insert_at_cursor_cb));
  // signal_activate ()
}

SearchDialog::~SearchDialog() {}

/*
  void SearchDialog::insert_at_cursor_cb(const Glib::ustring& str) {
  find->set_sensitive(what.get_text_length() == 0 ? false : true);
  }
*/

bool SearchDialog::run() {
  dialog.show_all();
  int x = dialog.run();
  if (x == Gtk::RESPONSE_OK) {
    return true;
  }
  return false;
}

bool SearchDialog::whole_word() { return _whole_word.get_active(); }

bool SearchDialog::match_case() { return _match_case.get_active(); }

bool SearchDialog::wrap() { return _wrap.get_active(); }

bool SearchDialog::backwards() { return _backwards.get_active(); }

bool SearchDialog::beginning() { return _beginning.get_active(); }

std::string SearchDialog::get_text() { return what.get_text(); }

void SearchDialog::whole_word(bool st) { _whole_word.set_active(st); }

void SearchDialog::match_case(bool st) { _match_case.set_active(st); }

void SearchDialog::wrap(bool st) { _wrap.set_active(st); }

void SearchDialog::backwards(bool st) { _backwards.set_active(st); }

void SearchDialog::beginning(bool st) { _beginning.set_active(st); }

void SearchDialog::set_text(std::string &text) { what.set_text(text); }

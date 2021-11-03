/*
 * replacedialog.cc
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

#include "gtkmm/dialog.h"
#include "macros.h"
#include "replacedialog.hh"
#include <gtkmm.h>

static const gint RESPONSE_REPLACE_ALL = 0;
static const gint RESPONSE_REPLACE = 1;
static const gint RESPONSE_FIND = 2;

ReplaceDialog::ReplaceDialog()
    : dialog(_("Find"), true), label(_("Search for:")),
      label2(_("Replace with:")), _whole_word(_("Match entire word only")),
      _match_case(_("Match case")), _wrap(_("Wrap search")),
      _cursor(_("Search from the cursor position")),
      _backwards(_("Search backwards")),
      _beginning(_("Search from the beginnig of the document"))
// find(_("Find...")), replace(_("_Replace")),
/*  find_replace(Gtk::Stock::FIND_AND_REPLACE),*/
// replace_all(_("Replace _All")), _close(_("Close")) {
{
  dialog.set_border_width(10);

  dialog.set_position(Gtk::WIN_POS_CENTER);

  Gtk::Box *box = dialog.get_content_area();

  box->pack_start(hbox);
  box->pack_start(hbox2);
  hbox.pack_start(label, false, false, 10);
  hbox.pack_start(what, true, true, 10);
  hbox2.pack_start(label2, false, false, 10);
  hbox2.pack_start(with, true, true, 10);
  box->pack_start(_beginning, true, true);
  box->pack_start(_whole_word, true, true);
  box->pack_start(_match_case, true, true);
  box->pack_start(_wrap, true, true);
  box->pack_start(_backwards, true, true);

  // replace.set_use_underline();
  // replace_all.set_use_underline();

  dialog.add_button(_("Close"), Gtk::RESPONSE_CLOSE);
  dialog.add_button(_("Replace all..."), RESPONSE_REPLACE_ALL);
  dialog.add_button(_("Replace"), RESPONSE_REPLACE);
  dialog.add_button(_("Find..."), RESPONSE_FIND);

  /*  dialog.get_action_area()->pack_start(find_replace);*/

  // what.signal_activate().connect(sigc::mem_fun(find, &Gtk::Button::clicked));
  // with.signal_activate().connect(sigc::mem_fun(replace,
  // &Gtk::Button::clicked));

  dialog.signal_response().connect(
      sigc::mem_fun(*this, &ReplaceDialog::response));

  // find.signal_clicked().connect(
  //     sigc::mem_fun(this, &ReplaceDialog::on_find_clicked));
  // replace.signal_clicked().connect(
  //     sigc::mem_fun(this, &ReplaceDialog::on_replace_clicked));
  // /*  find_replace.signal_clicked().connect(sigc::mem_fun(this,
  //  * &ReplaceDialog::on_find_replace_clicked));*/
  // replace_all.signal_clicked().connect(
  //     sigc::mem_fun(this, &ReplaceDialog::on_replace_all_clicked));
  // _close.signal_clicked().connect(
  //     sigc::mem_fun(this, &ReplaceDialog::on_close_clicked));
}

void ReplaceDialog::response(const gint response) {
  switch (response) {
  case RESPONSE_FIND:
    if (!signal_find.emit()) {
      // close();
      dialog.response(0);
    }
    break;

  case RESPONSE_REPLACE:
    signal_replace.emit();
    break;

  case RESPONSE_REPLACE_ALL:
    signal_replace_all.emit();
    // close();
    dialog.response(0);
    break;

  case Gtk::RESPONSE_CLOSE:
    dialog.response(0);
    break;
  }

  this->dialog.Gtk::Dialog::response(response);
}

ReplaceDialog::~ReplaceDialog() {}

void ReplaceDialog::run() {
  dialog.show_all();
  dialog.run();
}

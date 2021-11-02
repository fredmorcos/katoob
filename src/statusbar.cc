/*
 * statusbar.cc
 * This file is part of katoob
 *
 * Copyright (C) 2002-2007 Mohammed Sameer
 * Copyright (C) 2008-2021 Frederic-Gerald Morcos
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

#include <string>
#include "statusbar.hh"
#include "utils.hh"
#include "macros.h"

Statusbar::Statusbar(Conf& conf) :
  _conf(conf),
  red(Utils::get_data_path("red.png")),
  green(Utils::get_data_path("green.png")) {
  pack_start(red, false, false);
  pack_start(green, false, false);

  // TODO: I want to drop this!
  pack_start(tips, true, true);

  pack_start(enc, false, false);
  pack_start(overwrite, false, false);

  pack_start(input, false, false);

  pack_start(sbar, false, false);

  enc.set_size_request(150, -1);
  overwrite.set_size_request(75, -1);
  sbar.set_size_request(150, -1);

  set_overwrite(false);
  set_position(1, 1);

  input.set_label(_("Input"));
  input.signal_toggled().connect(sigc::mem_fun(*this, &Statusbar::signal_input_toggled_cb));
  activate_input(false);

  show_all();
  red.hide();
}

Statusbar::~Statusbar() {

}

void Statusbar::set_overwrite(bool o) {
  overwrite.set_text(o ? _("OVR") : _("INS"));
}

void Statusbar::set_position(int c, int l) {
  sbar.pop(0);
  sbar.push(Utils::substitute(_(" L: %d, C: %d"), l, c));
}

void Statusbar::set_modified(bool m) {
  if (m) {
    green.hide();
    red.show();
  }
  else {
    red.hide();
    green.show();
  }
}

void Statusbar::reset_gui() {
  _conf.get("statusbar", true) ? Gtk::HBox::show() : hide();
}

void Statusbar::show(bool do_show) {
  do_show ? Gtk::HBox::show() : hide();
  _conf.set("statusbar", do_show);
}

void Statusbar::set_encoding(std::string e) {
  enc.set_text(e);
}

bool Statusbar::set_input_status(bool active) {
  if (active == input.get_active()) {
    return false;
  }
  else {
    input.set_active(active);
    return true;
  }
}

bool Statusbar::get_input_status() {
  return input.get_active();
}

void Statusbar::signal_input_toggled_cb() {
  signal_input_toggled.emit(input.get_active());
}

void Statusbar::activate_input(bool active) {
  input.set_sensitive(active);
}

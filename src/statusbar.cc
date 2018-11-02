/*
 * statusbar.cc
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <string>
#include "statusbar.hh"
#include "utils.hh"
#include "macros.h"
#include "docfactory.hh"

Statusbar::Statusbar(Conf& conf) :
  _conf(conf),
  red(Utils::get_data_path("pixmaps", "red.png")),
  green(Utils::get_data_path("pixmaps", "green.png")) {
  pack_start(red, false, false);
  pack_start(green, false, false);

  // TODO: I want to drop this!
  pack_start(tips, true, true);

  pack_start(enc, false, false);
  pack_start(overwrite, false, false);
  pack_start(sbar, false, false);

  enc.set_size_request(150, -1);
  overwrite.set_size_request(75, -1);
  sbar.set_size_request(150, -1);

  set_overwrite(false);
  set_position(1, 1);

  show_all();
  red.hide();

  DocFactory *factory = DocFactory::get();
  factory->signal_cursor_moved.connect(sigc::mem_fun(this, &Statusbar::set_position));
  factory->signal_overwrite_toggled.connect(sigc::mem_fun(this, &Statusbar::set_overwrite));
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

/*
 * label.cc
 * This file is part of katoob
 *
 * Copyright (C) 2006 Mohammed Sameer
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

#include "label.hh"
#include "macros.h"
#include "utils.hh"
#include <gtkmm.h>

Label::Label(Conf &conf) : _conf(conf), _readonly(false), _modified(false) {
  Gtk::Image *image =
      Gtk::manage(new Gtk::Image(Gtk::Stock::CLOSE, Gtk::ICON_SIZE_MENU));

  close.set_image(*image);
  close.set_relief(Gtk::RELIEF_NONE);
  close.set_size_request(24, 24);
  close.set_border_width(0);

  close.signal_clicked().connect(
      sigc::mem_fun(signal_close_clicked, &sigc::signal<void>::emit));

  pack_start(label);
  pack_start(close);
  label.show();
  if (_conf.get("showclose", true)) {
    close.show();
  }
  set_normal();
}

Label::~Label() {}

void Label::set_text(int num) {
  label.set_text(Utils::substitute(_("Untitled New %d"), num));
}

void Label::set_text(const std::string &str) { label.set_text(str); }

void Label::reset_gui() {
  if (_conf.get("showclose", true)) {
    close.show();
  } else {
    close.hide();
  }
}

void Label::set_readonly(bool ro, bool force) {
  // A modified document can't be read only.
  if (_modified) {
    return;
  }

  if (!force) {
    if (ro == _readonly) {
      return;
    }
  }

  if (ro) {
    katoob_set_color(_conf, label, Utils::KATOOB_COLOR_READONLY);
  } else {
    set_normal();
  }
  _readonly = ro;
}

void Label::set_modified(bool m, bool force) {
  // A read only document can't be modified.
  if (_readonly) {
    return;
  }

  if (!force) {
    if (m == _modified) {
      return;
    }
  }

  if (m) {
    katoob_set_color(_conf, label, Utils::KATOOB_COLOR_MODIFIED);
  } else {
    set_normal();
  }
  _modified = m;
}

void Label::set_normal() {
  katoob_set_color(_conf, label, Utils::KATOOB_COLOR_NORMAL);
}

std::string Label::get_text() { return label.get_text(); }

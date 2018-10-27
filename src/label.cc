/*
 * label.cc
 * This file is part of katoob
 *
 * Copyright (C) 2006, 2008 Mohammed Sameer
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

#include <gtkmm/stock.h>
#include "label.hh"
#include "utils.hh"
#include "macros.h"

Label::Label(Conf& conf) :
	_conf(conf),
	_readonly(false),
	_modified(false)
{
	Gtk::Image *image = Gtk::manage(new Gtk::Image(Gtk::Stock::CLOSE, 
												Gtk::ICON_SIZE_MENU));

	image->set_size_request(10, 10);
	close.set_image(*image);
	close.set_relief(Gtk::RELIEF_NONE);
	close.set_focus_on_click(false);

	close.signal_clicked().connect(sigc::mem_fun(signal_close_clicked, &sigc::signal<void>::emit));

	pack_start(label);
	pack_start(close);
	label.show();
	if (_conf.get("showclose", true))
		close.show();

	set_rom(false, false, true);
}

Label::~Label() {
}

void Label::set_text(int num) {
  label.set_text(Utils::substitute(_("Untitled New %d"), num));
}

void Label::set_text(const std::string& str) {
  label.set_text(str);
}

void Label::reset_gui() {
  if (_conf.get("showclose", true)) {
    close.show();
  }
  else {
    close.hide();
  }

  set_rom(_modified, _readonly, true);
}

void Label::set_rom(bool ro, bool m, bool force) {
  // We will always color on normal
  if ((!ro) && (!m)) {
    katoob_set_color(_conf, label, Utils::KATOOB_COLOR_NORMAL);
    _readonly = ro;
    _modified = m;
  }
  // read only and force is specified or our status is different.
  else if ((ro) && (((_readonly == ro) && (force)) || (_readonly != ro))) {
    katoob_set_color(_conf, label, Utils::KATOOB_COLOR_READONLY);
    _readonly = ro;
  }
  // same for modified.
  else if ((m) && (((_modified == m) && (force)) || (_modified != m))) {
    katoob_set_color(_conf, label, Utils::KATOOB_COLOR_MODIFIED);
    _modified = m;
  }
}

std::string Label::get_text() {
  return label.get_text();
}

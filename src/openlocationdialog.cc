/*
 * openlocationdialog.cc
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

#include <config.h>

#include "macros.h"
#include "openlocationdialog.hh"
#include "utils.hh"
#include <gtkmm.h>

OpenLocationDialog::OpenLocationDialog(Conf &conf, Encodings &enc): _conf(conf)
{
  set_modal();
  set_position(Gtk::WIN_POS_CENTER);
  Gtk::VBox *vbox = Gtk::Dialog::get_vbox();

  label1.set_text(_("Location"));
  label2.set_text(_("Encoding"));

  for (int x = 0; x < enc.size(); x++) {
    encoding.append_text(enc.at(x));
  }

  encoding.set_active(enc.default_open());

  std::vector<std::string> locations(conf.get_locations());
  for (unsigned x = 0; x < locations.size(); x++) {
    location.append_text(locations[x]);
  }

  to_active.set_active(conf.get("open_location_to_active", false));

  to_active.set_use_underline();
  to_active.set_label(_("_Insert into the active document."));

  box1.pack_start(label1, false, false, 10);
  box1.pack_start(location, true, true);
  box2.pack_start(label2, false, false, 10);
  box2.pack_start(encoding, true, true);

  vbox->pack_start(box1, true, true);
  vbox->pack_start(box2, true, true);
  vbox->pack_start(to_active);

  box1.set_border_width(5);
  box2.set_border_width(5);

  add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_ACCEPT);

  // TODO: Any idea why do I need to set this explicitly ?
  location.set_size_request(500, -1);
}

OpenLocationDialog::~OpenLocationDialog()
{
}

std::string &OpenLocationDialog::get_uri()
{
  return uri;
}

bool OpenLocationDialog::run(std::string &out)
{
  show_all();

  if (Gtk::Dialog::run() == Gtk::RESPONSE_ACCEPT) {
    hide();
    Gtk::Entry *entry = location.get_entry();
    uri = entry->get_text();
    if (uri.size() == 0) {
      out = _("You must enter a location to open.");
      return false;
    } else {
      _conf.locations_prepend(uri);
      return true;
    }
  }

  return false;
}

int OpenLocationDialog::get_encoding()
{
  return _encodings.get(encoding.get_active_text());
}

bool OpenLocationDialog::insert_to_active()
{
  return to_active.get_active();
}

void OpenLocationDialog::disable_insert_to_active()
{
  to_active.set_sensitive(false);
}

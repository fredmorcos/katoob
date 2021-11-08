/*
 * openlocationdialog.hh
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

#include "conf.hh"
#include "encodings.hh"
#include <gtkmm.h>

class OpenLocationDialog: public Gtk::Dialog {
 public:
  OpenLocationDialog(Conf &, Encodings &);
  ~OpenLocationDialog();
  bool run(std::string &);
  std::string get_location();
  int get_encoding();
  bool insert_to_active();
  void disable_insert_to_active();
  std::string &get_uri();

 private:
  Gtk::Label label1, label2;
  Gtk::CheckButton to_active;
  Gtk::ComboBoxEntryText location, encoding;
  Gtk::HBox box1, box2;

  std::string uri;

  Conf &_conf;
  Encodings _encodings;
};

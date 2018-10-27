/*
 * openlocationdialog.hh
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

#ifndef __OPENLOCATIONDIALOG_HH__
#define __OPENLOCATIONDIALOG_HH__

#include <gtkmm/dialog.h>
#include <gtkmm/label.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/comboboxentrytext.h>
#include <gtkmm/box.h>
#include "conf.hh"
#include "encodings.hh"

class OpenLocationDialog : public Gtk::Dialog {
public:
  OpenLocationDialog(Conf&, Encodings&);
  ~OpenLocationDialog();
  bool run(std::string&);
  std::string get_location();
  int get_encoding();
  bool insert_to_active();
  void disable_insert_to_active();
  std::string& get_uri();

private:
  Gtk::Label label1, label2;
  Gtk::CheckButton to_active;
  Gtk::ComboBoxEntryText location, encoding;
  Gtk::HBox box1, box2;

  std::string uri;

  Conf& _conf;
  Encodings _encodings;
};

#endif /* __OPENLOCATIONDIALOG_HH__ */

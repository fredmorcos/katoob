/*
 * preferencesdialog.hh
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

#include "applets.hh"
#include "conf.hh"
#include <gtkmm.h>

class PreferencesDialog {
 public:
  PreferencesDialog(Conf &, Encodings &);
  ~PreferencesDialog();
  bool run();
  sigc::signal<void> signal_apply_clicked;

 private:
  void add_applet(const std::string &, Applet *);
  std::map<std::string, Applet *> applets;

  void apply_clicked_cb()
  {
    repopulate_conf();
    signal_apply_clicked.emit();
  }

  void repopulate_conf();

  void selection_signal_changed_cb();

  Conf &_conf;
  Encodings &_enc;

  Gtk::Dialog dialog;
  Gtk::ScrolledWindow sw;
  Gtk::TreeView treeview;
  Glib::RefPtr<Gtk::TreeSelection> selection;
  //  Gtk::TreeModelColumn<int> number;
  Gtk::TreeModelColumn<Glib::ustring> section;
  Gtk::TreeModelColumnRecord record;
  Glib::RefPtr<Gtk::ListStore> store;
  Gtk::HPaned paned;
  Gtk::Notebook notebook;
  Gtk::Button apply;
};

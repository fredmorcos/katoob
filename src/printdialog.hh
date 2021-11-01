/*
 * printdialog.hh
 * This file is part of katoob
 *
 * Copyright (C) 2002-2007 Mohammed Sameer
 * Copyright (C) 2008-2018 Frederic-Gerald Morcos
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

#pragma once

#include "conf.hh"
#include "print.hh"
#include <gtkmm.h>

class PrintDialog {
public:
  PrintDialog(Conf &, Document *, Glib::RefPtr<PageSetup> &,
              Glib::RefPtr<PrintSettings> &);

  ~PrintDialog();
  bool ok(std::string &);
  bool run();

  void has_selection(bool);
  void lines(int);

  void start_process();
  bool end_process(std::string &);

  void add_line(std::string &);

  bool all();
  bool selection();
  bool range(int &, int &);

  void manipulate();

private:
  Conf &_conf;

  void reset_paper_size(int);
  bool init_backend();
  bool is_preview();
  void process();

  void to_file_toggled_cb();
  void all_radio_toggled_cb();
  void selection_radio_toggled_cb();
  void lines_radio_toggled_cb();
  void browse_button_clicked_cb();
  void print_menu_changed_cb();
  void selection_signal_changed_cb();

  Gtk::Dialog dialog;
  Gtk::CheckButton to_file;
  Gtk::ComboBoxText print_menu;
  Gtk::ComboBoxText paper_size;
  Gtk::RadioButtonGroup range_group;
  Gtk::RadioButton all_radio;
  Gtk::RadioButton selection_radio;
  Gtk::RadioButton lines_radio;
  Gtk::HBox box2;
  Gtk::Frame printer_frame;
  Glib::RefPtr<Gtk::Adjustment> from_adj, to_adj, copies_button_adj;
  Gtk::SpinButton lines_from;
  Gtk::SpinButton lines_to;
  Gtk::SpinButton copies_button;
  Gtk::Entry entry;
  Glib::RefPtr<Gtk::TreeSelection> t_selection;
  Gtk::TreeModelColumn<int> id;
  Gtk::TreeModelColumn<Glib::ustring> name;
  Gtk::TreeModelColumn<Glib::ustring> status;

  /* Other Widgets. */
  Gtk::TreeView treeview;
  Gtk::ScrolledWindow window;
  Gtk::TreeModelColumnRecord record;
  Glib::RefPtr<Gtk::ListStore> store;
  Gtk::HBox box1, box3, box4, box5, box6, box7;
  Gtk::Button browse;
  Gtk::Label location_label, copies_label;
  Gtk::Frame range_frame;
  Gtk::VBox vbox;
  Gtk::Label from_label, to_label;
  Gtk::Label paper_size_label, orientation_label;
  Gtk::ComboBoxText orientation;

  Glib::RefPtr<Print> backend;
  bool _has_selection;
  std::string _pdf, _ps;
  int res;
};

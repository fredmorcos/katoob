/*
 * spelldialog.hh
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

#include <gtkmm/button.h>
#include <gtkmm/window.h>
#include <gtkmm/separator.h>
#include <gtkmm/label.h>
#include <gtkmm/box.h>
#include <gtkmm/table.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/entry.h>
#include <gtkmm/treeview.h>
#include <gtkmm/liststore.h>
#include <gtkmm/image.h>
#include "document.hh"

class SpellDialog : public Gtk::Window {
public:
  SpellDialog(Document *);
  ~SpellDialog();
  void run();

private:
  void ignore_clicked_cb();
  void ignore_all_clicked_cb();
  void add_clicked_cb();
  void change_clicked_cb();
  //  void change_all_clicked_cb();
  void check_clicked_cb();
  void close_clicked_cb();

  void got_misspelled(std::string&, std::vector<std::string>&);
  void selection_signal_changed_cb();
  void populate_suggestions(std::vector<std::string>&);

  void next();

  bool signal_key_press_event_cb(GdkEventKey *);

  //  Gtk::Window window;
  Gtk::Button close, ignore, ignore_all, change, /*change_all,*/ check, add;
  Gtk::Label misspelled_word, misspelled, change_to;
  Gtk::Image yes, no, image;
  Gtk::VBox vbox1, vbox2;
  Gtk::HBox hbox1, hbox2, hbox3;
  Gtk::Table table;
  Gtk::ScrolledWindow sw;
  Gtk::HSeparator separator;
  Gtk::Entry entry;
  Gtk::TreeView suggestions;
  Glib::RefPtr<Gtk::TreeSelection> selection;
  Gtk::TreeModelColumn<Glib::ustring> suggestions_col;
  Gtk::TreeModelColumnRecord record;
  Glib::RefPtr<Gtk::ListStore> store;

  Document *_doc;
  Glib::RefPtr<Glib::MainLoop> loop;
  //  std::map<std::string, std::string> replacements;
};

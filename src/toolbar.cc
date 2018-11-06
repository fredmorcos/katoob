/*
 * toolbar.cc
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

#include <gtkmm/stock.h>
#include <fstream>
#include <iostream>
#include "toolbar.hh"
#include "macros.h"
#include "utils.hh"

Toolbar::Toolbar(Conf& conf) :
  _conf(conf),
  _create(Gtk::Stock::NEW),
  _open(Gtk::Stock::OPEN),
  _save(Gtk::Stock::SAVE),
  _print(Gtk::Stock::PRINT),
  _close(Gtk::Stock::CLOSE),
  _undo(Gtk::Stock::UNDO),
  _redo(Gtk::Stock::REDO),
  _cut(Gtk::Stock::CUT),
  _copy(Gtk::Stock::COPY),
  _paste(Gtk::Stock::PASTE),
  _erase(Gtk::Stock::DELETE),
  _go_to_l(_("Goto Line")),
  _search_l(_("Search")),
  _dictionary_l(_("Spelling Dictionary"))
{
  create_main();
  create_extended();
}

Toolbar::~Toolbar() {

}

Gtk::Toolbar& Toolbar::get_main() {
  return _main;
};

Gtk::VBox& Toolbar::get_extended() {
  return _extended;
}

void Toolbar::create_main() {
  _main.append(_create);
  _main.append(_open);
  _main.append(_save);
  _main.append(_print);
  _main.append(s1);
  _main.append(_close);
  _main.append(s2);
  _main.append(_undo);
  _main.append(_redo);
  _main.append(s3);
  _main.append(_cut);
  _main.append(_copy);
  _main.append(_paste);
  _main.append(_erase);

  // Our tooltips.
  _create.set_tooltip_text(_("Create a new file"));
  _open.set_tooltip_text(_("Open a file for editing"));
  _save.set_tooltip_text(_("Save the existing file"));

  _print.set_tooltip_text(_("Print this document"));

  _close.set_tooltip_text(_("Close the active file"));
  _undo.set_tooltip_text(_("Undo"));
  _redo.set_tooltip_text(_("Redo"));
  _cut.set_tooltip_text(_("Cut"));
  _copy.set_tooltip_text(_("Copy"));
  _paste.set_tooltip_text(_("Paste"));
  _erase.set_tooltip_text(_("Delete current selection"));

  // This is for both_horiz style
  // NOTE: http://mail.gnome.org/archives/gtkmm-list/2004-June/msg00112.html
  _create.set_is_important();
  _open.set_is_important();
  _save.set_is_important();

  _print.set_is_important();

  _close.set_is_important();
  _undo.set_is_important();
  _redo.set_is_important();
  _cut.set_is_important();
  _copy.set_is_important();
  _paste.set_is_important();
  _erase.set_is_important();

  _main.show_all();

  // Signals.
  _create.signal_clicked().connect(sigc::mem_fun(signal_create_clicked, &sigc::signal<void>::emit));
  _open.signal_clicked().connect(sigc::mem_fun(signal_open_clicked, &sigc::signal<void>::emit));
  _save.signal_clicked().connect(sigc::mem_fun(signal_save_clicked, &sigc::signal<void>::emit));

  _print.signal_clicked().connect(sigc::mem_fun(signal_print_clicked, &sigc::signal<void>::emit));

  _close.signal_clicked().connect(sigc::mem_fun(signal_close_clicked, &sigc::signal<void>::emit));
  _undo.signal_clicked().connect(sigc::mem_fun(signal_undo_clicked, &sigc::signal<void>::emit));
  _redo.signal_clicked().connect(sigc::mem_fun(signal_redo_clicked, &sigc::signal<void>::emit));
  _cut.signal_clicked().connect(sigc::mem_fun(signal_cut_clicked, &sigc::signal<void>::emit));
  _copy.signal_clicked().connect(sigc::mem_fun(signal_copy_clicked, &sigc::signal<void>::emit));
  _paste.signal_clicked().connect(sigc::mem_fun(signal_paste_clicked, &sigc::signal<void>::emit));
  _erase.signal_clicked().connect(sigc::mem_fun(signal_erase_clicked, &sigc::signal<void>::emit));
}

void Toolbar::create_extended() {
  _extended.pack_start(box);
  box.pack_start(_search_l, false, false, 10);
  box.pack_start(_search, false, false);
  box.pack_start(_go_to_l, false, false, 10);
  box.pack_start(_go_to, false, false);

  box.pack_start(_dictionary_l, false, false, 10);
  box.pack_start(_dictionary, false, false);

  Gtk::Image *image = Gtk::manage(new Gtk::Image(Gtk::Stock::SPELL_CHECK, Gtk::IconSize(Gtk::ICON_SIZE_MENU)));
  _spell.set_image(*image);
  box.pack_start(_spell, false, false, 10);
  _spell.set_relief(Gtk::RELIEF_NONE);
  _spell.signal_clicked().connect(sigc::mem_fun(signal_spell_clicked, &sigc::signal<void>::emit));

  _dictionary.set_active_text(_conf.get("default_dict", "en"));
  // TODO: I want to connect without the callback step.
  signal_dictionary_changed_conn = _dictionary.signal_changed().connect(sigc::mem_fun(*this, &Toolbar::dictionary_changed_cb));
  //  signal_dictionary_changed_conn = _dictionary.signal_changed().connect(sigc::bind<std::string>(sigc::mem_fun(signal_dictionary_changed, &sigc::signal<void, std::string>::emit), _dictionary.get_active_text()));

  _search.signal_activate().connect(sigc::mem_fun(*this, &Toolbar::search_activate_cb));
  _go_to.signal_activate().connect(sigc::mem_fun(*this, &Toolbar::go_to_activate_cb));

  _extended.pack_start(sep);

  create_extra_buttons();
  box.pack_start(_extra_buttons);
  _extra_buttons.set_toolbar_style(Gtk::ToolbarStyle(Gtk::TOOLBAR_ICONS));

  _extended.show_all();
}

void Toolbar::set_text() {
  _main.set_toolbar_style(Gtk::ToolbarStyle(Gtk::TOOLBAR_TEXT));
  _conf.set("toolbartype","text");
}

void Toolbar::set_icons() {
  _main.set_toolbar_style(Gtk::ToolbarStyle(Gtk::TOOLBAR_ICONS));
  _conf.set("toolbartype","icons");
}

void Toolbar::set_both() {
  _main.set_toolbar_style(Gtk::ToolbarStyle(Gtk::TOOLBAR_BOTH));
  _conf.set("toolbartype","both");
}

void Toolbar::set_beside() {
  _main.set_toolbar_style(Gtk::ToolbarStyle(Gtk::TOOLBAR_BOTH_HORIZ));
  _conf.set("toolbartype","both_horiz");
}

void Toolbar::set_dictionary(std::string& d) {
  signal_dictionary_changed_conn.block();
  _dictionary.set_active_text(d);
  signal_dictionary_changed_conn.unblock();
}

std::string Toolbar::get_dictionary() {
  return _dictionary.get_active_text();
}

void Toolbar::reset_gui() {
  _conf.get("toolbar", true) == true ? _main.show() : _main.hide();
  _conf.get("extended_toolbar", true) == true ? _extended.show() : _extended.hide();
  _conf.get("extra_buttons", true) == true ? _extra_buttons.show_all() : _extra_buttons.hide();

  const std::string& toolbartype = _conf.get("toolbartype", "both");

  if (toolbartype == "text") {
    set_text();
  }
  else if (toolbartype == "icons") {
    set_icons();
  }
  else if (toolbartype == "both_horiz") {
    set_beside();
  }
  else {
    set_both();
  }
}

void Toolbar::reset_gui(bool enable) {
  _save.set_sensitive(enable);

  _print.set_sensitive(enable);

  _close.set_sensitive(enable);
  _undo.set_sensitive(enable);
  _redo.set_sensitive(enable);
  _cut.set_sensitive(enable);
  _copy.set_sensitive(enable);
  _paste.set_sensitive(enable);
  _erase.set_sensitive(enable);
  _dictionary.set_sensitive(enable);
  _spell.set_sensitive(enable);
  _dictionary.set_sensitive(enable);
  _go_to.set_sensitive(enable);
  _search.set_sensitive(enable);
  _extra_buttons.set_sensitive(enable);
}

void Toolbar::search_activate_cb() {
  if (_search.get_text().size()) {
    signal_search_activated.emit(_search.get_text());
  }
}

void Toolbar::go_to_activate_cb() {
  if (_go_to.get_text().size()) {
    signal_go_to_activated.emit(atoi(_go_to.get_text().c_str()));
  }
}

void Toolbar::create_extra_buttons() {
  std::string path = Utils::get_data_path("toolbar");
  std::ifstream ifs;
  ifs.open(path.c_str());
  if (ifs.is_open()) {
    std::string buff;
    while (getline(ifs, buff)) {
      create_extra_button(buff);
    }
    ifs.close();
  }
}

void Toolbar::create_extra_button(std::string& label) {
  Gtk::ToolButton *button = Gtk::manage(new Gtk::ToolButton);
  button->set_label(label);
  button->show_all();
  _extra_buttons.append(*button);
  button->signal_clicked().connect(sigc::bind<std::string>(sigc::mem_fun(*this, &Toolbar::extra_button_clicked), label));
}

void Toolbar::enable_undo(bool s) {
  _undo.set_sensitive(s);
}

void Toolbar::enable_redo(bool s) {
  _redo.set_sensitive(s);
}

void Toolbar::enable_dictionary(bool s) {
  _dictionary.set_sensitive(s);
}

void Toolbar::set_read_only(bool r) {
  _save.set_sensitive(!r);
  _cut.set_sensitive(!r);
  _erase.set_sensitive(!r);
  _paste.set_sensitive(!r);
}

void Toolbar::show_main(bool do_show) {
  do_show ? _main.show() : _main.hide();
  _conf.set("toolbar", do_show);
}

void Toolbar::show_extended(bool do_show) {
  do_show ? _extended.show() : _extended.hide();
  _conf.set("extended_toolbar", do_show);
}

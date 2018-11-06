/*
 * inputwindow.hh
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

#include <gtkmm/dialog.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/table.h>

class InputWindow : public Gtk::Dialog {
public:
  InputWindow();
  ~InputWindow();

  void show();
  void hide();
  void set_layout(std::map<std::string, std::string>&);
  void set_layout(std::map<std::string, std::vector<std::string>>&);

  sigc::signal<void, std::string&> signal_button_clicked;
  sigc::signal<void> signal_dialog_closed;

private:
  typedef enum {
    INPUT_DISPLAY_EMULATOR,
    INPUT_DISPLAY_MULTIPRESS,
    INPUT_DISPLAY_NONE
  } InputWindowDisplayType;

  InputWindowDisplayType _type;

  void rebuild();
  void emulator_rebuild();
  void multipress_rebuild();
  void clear();

  // Our callbacks.
  void signal_response_cb(unsigned);
  void button_clicked_cb(unsigned);

  std::map<std::string, std::string> _em;
  std::map<std::string, std::vector<std::string>> _mp;

  // Our Widgets.
  Gtk::ScrolledWindow sw;
  Gtk::Table tb;
  std::vector<Gtk::Button *> buttons;
  std::vector<Gtk::Label *> labels;
};

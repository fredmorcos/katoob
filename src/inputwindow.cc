/*
 * inputwindow.cc
 * This file is part of katoob
 *
 * Copyright (C) 2007 Mohammed Sameer
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

#include <cassert>
#include <gtkmm/stock.h>
#include <gtkmm/label.h>
#include "inputwindow.hh"
#include "macros.h"

InputWindow::InputWindow() : Gtk::Dialog(_("Input Window"), false, true),
			     _type(INPUT_DISPLAY_NONE) {
  add_button(Gtk::Stock::CLOSE, Gtk::RESPONSE_OK);
  get_vbox()->pack_start(sw, true, true);
  sw.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  sw.add(tb);
  signal_response().connect(sigc::mem_fun(*this, &InputWindow::signal_response_cb));
}

InputWindow::~InputWindow() {
  clear();
}

void InputWindow::hide() {
  Gtk::Dialog::hide();
}

void InputWindow::show() {
  assert(_type != INPUT_DISPLAY_NONE);
  Gtk::Dialog::show_all();
}

void InputWindow::set_layout(std::map<std::string, std::string>& em) {
  if (em == _em) {
    return;
  }
  _em = em;
  _type = INPUT_DISPLAY_EMULATOR;
  rebuild();
}

void InputWindow::set_layout(std::map<std::string, std::vector<std::string> >& mp) {
  if (mp == _mp) {
    return;
  }
  _mp = mp;
  _type = INPUT_DISPLAY_MULTIPRESS;
  rebuild();
}

void InputWindow::rebuild() {
  assert(_type != INPUT_DISPLAY_NONE);
  switch (_type) {
  case INPUT_DISPLAY_EMULATOR:
    return emulator_rebuild();
  case INPUT_DISPLAY_MULTIPRESS:
    return multipress_rebuild();
  case INPUT_DISPLAY_NONE:
    return;
    // We shouldn't reach this.
  }
}

void InputWindow::emulator_rebuild() {
  assert(_type == INPUT_DISPLAY_EMULATOR);

  tb.resize(_em.size(), 2);

  // clear.
  clear();
  unsigned x = 0;
  std::map<std::string, std::string>::iterator iter;
  for (iter = _em.begin(); iter != _em.end(); iter++) {
    Gtk::Label *label = new Gtk::Label(iter->first);
    labels.push_back(label);
    Gtk::Button *button = new Gtk::Button(iter->second);
    button->signal_clicked().connect(sigc::bind<unsigned>(sigc::mem_fun(*this, &InputWindow::button_clicked_cb), x));
    buttons.push_back(button);
    tb.attach(*label, 0, 1, x, x+1);
    tb.attach(*button, 1, 2, x, x+1);
    ++x;
  }
}

void InputWindow::multipress_rebuild() {
  assert(_type == INPUT_DISPLAY_MULTIPRESS);

  // clear.
  clear();

  unsigned w = 0;

  std::map<std::string, std::vector<std::string> >::iterator iter;
  for (iter = _mp.begin(); iter != _mp.end(); iter++) {
    w > iter->second.size() ? w : iter->second.size();
  }

  tb.resize(_mp.size(), w+1);

  unsigned x = 0;
  unsigned p = 0;
  for (iter = _mp.begin(); iter != _mp.end(); iter++) {
    Gtk::Label *label = new Gtk::Label(iter->first);
    labels.push_back(label);
    tb.attach(*label, 0, 1, x, x+1);

    for (unsigned y = 0; y < iter->second.size(); y++) {
      Gtk::Button *button = new Gtk::Button(iter->second[y]);
      button->signal_clicked().connect(sigc::bind<unsigned>(sigc::mem_fun(*this, &InputWindow::button_clicked_cb), p));
      buttons.push_back(button);
      tb.attach(*button, y+1, y+2, x, x+1);
    }
    ++x;
  }
}

void InputWindow::clear() {
  while (labels.size() > 0) {
    Gtk::Label *label = labels.front();
    delete label;
    labels.erase(labels.begin());
  }

  while(buttons.size() > 0) {
    Gtk::Button *button = buttons.front();
    delete button;
    buttons.erase(buttons.begin());
  }
}

void InputWindow::button_clicked_cb(unsigned x) {
  std::string str(buttons[x]->get_label());
  signal_button_clicked.emit(str);
}

void InputWindow::signal_response_cb(unsigned x) {
  hide();
  signal_dialog_closed.emit();
}

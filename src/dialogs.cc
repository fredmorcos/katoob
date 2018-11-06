/*
 * dialogs.cc
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

#include <gtkmm/messagedialog.h>
#include <gtkmm/stock.h>
#include <gtkmm/label.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/adjustment.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/textview.h>
#include "dialogs.hh"
#include "macros.h"
#include "utils.hh"

bool katoob_simple_question(const std::string& message) {
  Gtk::MessageDialog dlg(message, false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO, true);
  int res = dlg.run();
  if (res == Gtk::RESPONSE_YES) {
    return true;
  }

  return false;
}

void katoob_error(const std::string& message) {
  Gtk::MessageDialog dlg(message, false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
  dlg.run();
  //  dlg.destroy();
}

void katoob_info(const std::string& message) {
  Gtk::MessageDialog dlg(message, false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true);
  dlg.run();
}

bool katoob_big_info(const std::string& message, bool enable_insert) {
  Gtk::Dialog dlg;
  Gtk::ScrolledWindow sw;
  sw.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  Gtk::TextView tv;
  tv.set_editable(false);
  tv.get_buffer()->set_text(message);
  sw.add(tv);
  dlg.get_vbox()->pack_start(sw, true, true);
  dlg.set_size_request(Gdk::Screen::get_default()->get_width()/2, Gdk::Screen::get_default()->get_height()/2);

  if (enable_insert) {
    dlg.add_button(_("Insert to document"), Gtk::RESPONSE_YES);
  }

  dlg.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

  dlg.show_all();
  switch (dlg.run()) {
  case Gtk::RESPONSE_YES:
    return true;
  default:
    return false;
  }
}

int katoob_question(const std::string& message) {
  Gtk::MessageDialog dialog(message, false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_NONE, true);

  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  dialog.add_button(Gtk::Stock::NO, Gtk::RESPONSE_NO);
  dialog.add_button(Gtk::Stock::YES, Gtk::RESPONSE_YES);
  dialog.set_position (Gtk::WIN_POS_CENTER);

  return dialog.run();
}

int katoob_goto_dialog() {
  Gtk::VBox *box;
  Gtk::HBox hbox;
  Gtk::Dialog dialog(_("Goto Line"), true, true);

  box = dialog.get_vbox();
  box->pack_start(hbox);

  hbox.set_border_width(5);

  Gtk::Label label(_("Line:"));
  hbox.pack_start (label);
  label.set_justify (Gtk::JUSTIFY_LEFT);

  Gtk::Adjustment adjustment(1, 1, 1000000000);

  Gtk::SpinButton go_to(adjustment);
  hbox.pack_start(go_to);

  Gtk::Button *button = dialog.add_button(Gtk::Stock::JUMP_TO, Gtk::RESPONSE_OK);
  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);

  /*
  button->set_flags(Gtk::CAN_DEFAULT);
  button->grab_default();
  */
  go_to.signal_activate().connect(mem_fun(button, &Gtk::Button::clicked));

  dialog.show_all();

  int x = dialog.run();

  if (x == Gtk::RESPONSE_OK) {
    return go_to.get_value_as_int();
  }
  else {
    return -1;
  }
}

Glib::RefPtr<ActivityMeter> katoob_activity() {
  return Glib::RefPtr<ActivityMeter>(new ActivityMeter);
}

ActivityMeter::ActivityMeter() : dialog(_("Please wait..."), true, true) {
  loop = Glib::MainLoop::create();
  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  dialog.get_vbox()->pack_start(bar, true, true);
  bar.set_pulse_step(0.007);
  dialog.signal_response().connect(sigc::mem_fun(this, &ActivityMeter::signal_response_cb));
}

void ActivityMeter::signal_response_cb(int KATOOB_UNUSED(res)) {
  quit();
  signal_cancel_clicked.emit();
}

void ActivityMeter::run() {
  conn = Glib::signal_idle().connect(sigc::bind_return(sigc::mem_fun(bar, &Gtk::ProgressBar::pulse), true), Glib::PRIORITY_LOW);
  dialog.show_all();
  loop->run();
}

void ActivityMeter::quit() {
  conn.disconnect();
  dialog.hide();
  loop->quit();
}

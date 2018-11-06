/*
 * dialogs.hh
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

#include <string>
#include <glibmm/main.h>
#include <gtkmm/dialog.h>
#include <gtkmm/progressbar.h>

bool katoob_simple_question(const std::string&);
void katoob_error(const std::string&);
void katoob_info(const std::string&);
bool katoob_big_info(const std::string&, bool = false);
int katoob_question(const std::string&);
int katoob_goto_dialog();

class ActivityMeter : public Glib::Object {
public:
  ActivityMeter();
  //  ~ActivityMeter();
  void run();
  void quit();
  sigc::signal<void> signal_cancel_clicked;
private:
  void signal_response_cb(int);

  sigc::connection conn;
  Gtk::ProgressBar bar;
  Gtk::Dialog dialog;
  Glib::RefPtr<Glib::MainLoop> loop;
};

Glib::RefPtr<ActivityMeter> katoob_activity();

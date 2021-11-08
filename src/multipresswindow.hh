/*
 * multipresswindow.hh
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

#include <glibmm/main.h>
#include <gtkmm.h>
#include <map>
#include <string>
#include <vector>

class MultipressWindow: public Gtk::Window {
 public:
  MultipressWindow();
  ~MultipressWindow();
  void set_values(const std::vector<std::string> &);
  void set_timeout(int);
  void set_key(const std::string &);
  sigc::signal<bool, std::string &> signal_change_key;
  sigc::signal<void, std::string &> signal_insert_key;
  sigc::signal<void, GdkEventKey *> signal_invalid_key;

  void get();

 private:
  // Virtual methods to override gtkmm default handlers.
  bool on_key_press_event(GdkEventKey *);
  bool on_expose_event(GdkEventExpose *);

  void clear(bool);
  void show_next();
  bool timeout_cb();

  Glib::RefPtr<Glib::MainLoop> loop;
  sigc::connection timeout_conn;

  std::string _key, _tmp_key;
  unsigned _pos;

  int _timeout;
  std::vector<std::string> _values;
  std::string _to_draw;
};

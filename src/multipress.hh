/*
 * multipress.hh
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

#include "multipresswindow.hh"
#include <map>
#include <string>
#include <vector>

class Multipress {
 public:
  Multipress();

  static bool ok(std::string &);
  static std::vector<std::string> &list_layouts();
  static bool parse_file(std::string &, std::map<std::string, std::vector<std::string> > &);
  static void activate(int);
  static bool get_active();
  static bool get(const std::string &, int);

  static std::map<std::string, std::vector<std::string> > &get_layout();

  static sigc::signal<void, std::string &> signal_insert_key;
  static sigc::signal<void, GdkEventKey *> signal_invalid_key;

 private:
  static void signal_insert_key_cb(std::string &);
  static void signal_invalid_key_cb(GdkEventKey *);
  static bool signal_change_key_cb(const std::string &);

  static bool get_values(const std::string &, std::vector<std::string> &);
  static MultipressWindow *window;
  static bool _ok;
  static std::string _err;
  static std::vector<std::string> names;
  static std::vector<std::map<std::string, std::vector<std::string> > > layouts;
  static int layout;
};

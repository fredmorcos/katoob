/*
 * emulator.hh
 * This file is part of katoob
 *
 * Copyright (C) 2006, 2007 Mohammed Sameer
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

#ifndef __EMULATOR_HH__
#define __EMULATOR_HH__

#include <string>
#include <map>
#include <vector>

class Emulator {
public:
  Emulator();
  ~Emulator();
  static bool ok(std::string&);
  static std::vector<std::string>& list_layouts();
  static bool get(const std::string&, std::string&);
  static bool parse_file(std::string&, std::map<std::string, std::string>&);
  static void activate(int);
  static bool get_active();

  static std::map<std::string, std::string>& get_layout();

private:
  static std::vector<std::string> names;
  static std::vector<std::map<std::string, std::string> > layouts;
  static bool _ok;
  static std::string _err;
  static int layout;
};

#endif /* __EMULATOR_HH__ */

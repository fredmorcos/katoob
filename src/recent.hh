/*
 * recent.hh
 * This file is part of katoob
 *
 * Copyright (C) 2002-2007 Mohammed Sameer
 * Copyright (C) 2008-2021 Frederic-Gerald Morcos
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

#include <sigc++/signal.h>
#include <vector>
#include <string>

// Forward declarations
class Conf;

class Recent : sigc::trackable {
public:
  static Recent *get();
  static void init(Conf *);

  void destroy();
  unsigned size();

  std::string& operator[](unsigned);

  void add_item(const std::string&);

  std::string get_item();

  sigc::signal<void> signal_changed;
private:
  Recent(Conf *);
  ~Recent();

  static Recent *_recent;
  Conf *_conf;
  std::vector<std::string> _items;
};

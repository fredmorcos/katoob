/*
 * emulator.cc
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

#include <config.h>

#include "emulator.hh"
#include "glibmm/miscutils.h"
#include "utils.hh"
#include <cassert>
#include <fstream>
#include <glibmm.h>
#include <iostream>
#include <memory>

Emulator::Emulator()
{
  std::unique_ptr<Glib::Dir> d;

  try {
    d = std::unique_ptr<Glib::Dir>(new Glib::Dir(EMULATOR_DIR));
  } catch (Glib::FileError &e) {
    _err = e.what();
    return;
  }

  Glib::DirIterator start = d->begin();
  Glib::DirIterator end = d->end();

#ifndef GLIBMM_EXCEPTIONS_ENABLED
  std::auto_ptr<Glib::Error> error;
#endif

  while (start != end) {
    std::map<std::string, std::string> map;
    std::string file = Glib::build_filename(EMULATOR_DIR, *start);
    std::cout << "Found emulator file: " << file << std::endl;
    if (parse_file(file, map)) {
#ifdef GLIBMM_EXCEPTIONS_ENABLED
      try {
        std::string _utf8 = Glib::filename_to_utf8(*start);
        names.push_back(_utf8);
      } catch (Glib::ConvertError &er) {
        names.push_back(*start);
      }
#else
      std::string _utf8 = Glib::filename_to_utf8(*start, error);
      if (error.get()) {
        names.push_back(*start);
        error.reset();
      } else {
        names.push_back(_utf8);
      }
#endif
      layouts.push_back(map);
      _ok = true;
    }
    start++;
  }

  if (!_ok) {
    std::cout << "Emulator will not work" << std::endl;
  }
}

Emulator::~Emulator()
{
}

std::vector<std::string> &Emulator::list_layouts()
{
  return names;
}

bool Emulator::ok(std::string &e)
{
  e = _err;
  return _ok;
}

bool Emulator::parse_file(std::string &file, std::map<std::string, std::string> &map)
{
  std::ifstream ifs;
  ifs.open(file.c_str());
  if (ifs.is_open()) {
    std::string buff;
    while (getline(ifs, buff)) {
      int x = buff.find(' ');
      std::string key(buff, 0, x);
      std::string value(buff, ++x);
      map[key] = value;
    }
    ifs.close();
    return true;
  } else {
    return false;
  }
}

bool Emulator::get(const std::string &key, std::string &value)
{
  assert(layout < layouts.size());
  assert(layout >= 0);

  std::map<std::string, std::string>::iterator iter = layouts[layout].find(key);
  if (iter == layouts[layout].end()) {
    return false;
  }
  value = iter->second;
  return true;
}

void Emulator::activate(int x)
{
  assert((x == -1) || (x < layouts.size()));
  layout = x;
}

bool Emulator::get_active()
{
  return (layout != -1);
}

std::map<std::string, std::string> &Emulator::get_layout()
{
  assert(layout != -1);
  return layouts[layout];
}

std::vector<std::string> Emulator::names;
std::vector<std::map<std::string, std::string> > Emulator::layouts;
bool Emulator::_ok = false;
int Emulator::layout = -1;
std::string Emulator::_err;

/*
 * multipress.cc
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

#include "glibmm/miscutils.h"
#include "multipress.hh"
#include "utils.hh"
#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>

Multipress::Multipress()
{
  std::unique_ptr<Glib::Dir> d;

  try {
    d = std::make_unique<Glib::Dir>(MULTIPRESS_DIR);
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
    std::map<std::string, std::vector<std::string>> map;
    std::string file = Glib::build_filename(MULTIPRESS_DIR, *start);
    std::cout << "Found multipress file: " << file << std::endl;

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
  window = new MultipressWindow;
  window->signal_insert_key.connect(sigc::ptr_fun(&Multipress::signal_insert_key_cb));
  window->signal_invalid_key.connect(sigc::ptr_fun(&Multipress::signal_invalid_key_cb));
  window->signal_change_key.connect(sigc::ptr_fun(&Multipress::signal_change_key_cb));

  if (!_ok) {
    std::cout << "Multipress will not work" << std::endl;
  }
}

std::vector<std::string> &Multipress::list_layouts()
{
  return names;
}

bool Multipress::ok(std::string &e)
{
  e = _err;
  return _ok;
}

bool Multipress::parse_file(std::string &file, std::map<std::string, std::vector<std::string>> &map)
{
  std::ifstream ifs;
  ifs.open(file.c_str());
  if (ifs.is_open()) {
    std::string buff;
    while (getline(ifs, buff)) {
      int x = buff.find(' ');
      std::string key(buff, 0, x);
      std::string value(buff, ++x);
      map[key] = Utils::split(value, ' ');
    }
    ifs.close();
    return true;
  } else {
    return false;
  }
}

bool Multipress::get(const std::string &key, int timeout)
{
  assert(layout >= 0);
  assert((std::size_t) layout < layouts.size());

  std::vector<std::string> values;

  if (!get_values(key, values)) {
    return false;
  }

  window->set_key(key);
  window->set_values(values);
  window->set_timeout(timeout);
  window->get();

  return true;
}

bool Multipress::get_values(const std::string &key, std::vector<std::string> &values)
{
  auto iter = layouts[layout].find(key);

  if (iter == layouts[layout].end()) {
    return false;
  }

  if (iter->second.empty()) {
    return false;
  }
  values = iter->second;
  return true;
}

void Multipress::activate(int x)
{
  assert((x == -1) || ((std::size_t) x < layouts.size()));
  layout = x;
}

bool Multipress::get_active()
{
  return (layout != -1);
}

void Multipress::signal_insert_key_cb(std::string &str)
{
  signal_insert_key.emit(str);
}

void Multipress::signal_invalid_key_cb(GdkEventKey *event)
{
  signal_invalid_key.emit(event);
}

bool Multipress::signal_change_key_cb(const std::string &nkey)
{
  std::vector<std::string> values;

  if (!get_values(nkey, values)) {
    return false;
  }
  window->set_values(values);
  return true;
}

std::map<std::string, std::vector<std::string>> &Multipress::get_layout()
{
  assert(layout != -1);
  return layouts[layout];
}

sigc::signal<void, std::string &> Multipress::signal_insert_key;
sigc::signal<void, GdkEventKey *> Multipress::signal_invalid_key;
MultipressWindow *Multipress::window;
std::vector<std::string> Multipress::names;
std::vector<std::map<std::string, std::vector<std::string>>> Multipress::layouts;
bool Multipress::_ok = false;
int Multipress::layout = -1;
std::string Multipress::_err;

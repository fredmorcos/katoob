/*
 * multipress.cc
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
#include <fstream>
#include <memory>
#include "multipress.hh"
#include "utils.hh"


Multipress::Multipress() {
  std::string dir(Utils::get_data_dir() + Utils::get_dir_separator() + "multipress" + Utils::get_dir_separator());

  std::auto_ptr<Glib::Dir> d;

  try {
    d = std::auto_ptr<Glib::Dir>(new Glib::Dir(dir));
  }
  catch (Glib::FileError& e) {
    _err = e.what();
    return;
  }

  Glib::DirIterator start = d->begin();
  Glib::DirIterator end = d->end();
#ifndef GLIBMM_EXCEPTIONS_ENABLED
  std::auto_ptr<Glib::Error> error;
#endif
  while (start != end) {
    std::map<std::string, std::vector<std::string> > map;
    std::string file = dir + *start;
    if (parse_file(file, map)) {
#ifdef GLIBMM_EXCEPTIONS_ENABLED
      try {
	std::string _utf8 = Glib::filename_to_utf8(*start);
	names.push_back(_utf8);
      }
      catch (Glib::ConvertError& er) {
	names.push_back(*start);
      }
#else
      std::string _utf8 = Glib::filename_to_utf8(*start, error);
      if (error.get()) {
	names.push_back(*start);
	error.reset();
      }
      else {
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
}

Multipress::~Multipress() {
  delete window;
}

std::vector<std::string>& Multipress::list_layouts() {
  return names;
}

bool Multipress::ok(std::string& e) {
  e = _err;
  return _ok;
}

bool Multipress::parse_file(std::string& file, std::map<std::string, std::vector<std::string> >& map) {
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
  }
  else {
    return false;
  }
}

bool Multipress::get(const std::string& key, int timeout) {
  assert(layout < layouts.size());
  assert(layout >= 0);

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

bool Multipress::get_values(const std::string& key, std::vector<std::string>& values) {
  std::map<std::string, std::vector<std::string> >::iterator iter = layouts[layout].find(key);

  if (iter == layouts[layout].end()) {
    return false;
  }

  if (iter->second.size() == 0) {
    return false;
  }
  values = iter->second;
  return true;
}

void Multipress::activate(int x) {
  assert((x == -1) || (x < layouts.size()));
  layout = x;
}

bool Multipress::get_active() {
  return (layout != -1);
}

void Multipress::signal_insert_key_cb(std::string& str) {
  signal_insert_key.emit(str);
}

void Multipress::signal_invalid_key_cb(GdkEventKey *event) {
  signal_invalid_key.emit(event);
}

bool Multipress::signal_change_key_cb(const std::string& nkey) {
  std::vector<std::string> values;

  if (!get_values(nkey, values)) {
    return false;
  }
  window->set_values(values);
  return true;
}

std::map<std::string, std::vector<std::string> >& Multipress::get_layout() {
  assert(layout != -1);
  return layouts[layout];
}

sigc::signal<void, std::string&> Multipress::signal_insert_key;
sigc::signal<void, GdkEventKey *> Multipress::signal_invalid_key;
MultipressWindow *Multipress::window;
std::vector<std::string> Multipress::names;
std::vector<std::map<std::string, std::vector<std::string> > > Multipress::layouts;
bool Multipress::_ok = false;
int Multipress::layout = -1;
std::string Multipress::_err;

/*
 * conf.cc
 * This file is part of katoob
 *
 * Copyright (C) 2006, 2007, 2008 Mohammed Sameer
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

// #ifdef HAVE_CONFIG_H
#include <config.h>
// #endif /* HAVE_CONFIG_H */

#include <fstream>
#include <iostream>
#include <glib/gstdio.h>
#include <map>
#include <cstring>
#include "conf.hh"
#include "utils.hh"
#include "encodings.hh"

Conf::Conf(Encodings * _encodings) :
  _ok(false) {
  if (!prepare_dir()) {
    return;
  }

  load_conf();
  migrate_conf();

  defaults(_encodings);
}

Conf::~Conf() {
  if (!prepare_dir()) {
    return;
  }

  std::string ver = get_version();
  if ((ver.size() == 0) || (strcmp(ver.c_str(), VERSION))) {
    // We will save regardless of the user options.
    // We have new configuration options.
    if (!prepare_dir()) {
      return;
    }
    set("version", VERSION);
    save_conf();
  }

  if (!get("saveonexit", true)) {
    set("saveonexit", false);
    if (get("savewinpos", true)) {
      if (!prepare_dir()) {
	return;
      }
      load_conf();
      save_conf();
    }
  }
  else {
    if (!prepare_dir()) {
      return;
    }
    save_conf();
  }
}

void Conf::save_conf() {
  // TODO: Error checking ?
  Glib::ustring str = to_data();
  std::ofstream ostr((conf_dir + "katoob.conf").c_str());
  if (ostr.is_open()) {
    ostr << str;
    ostr.close();
  }
}

bool Conf::prepare_dir() {
  conf_dir = (Utils::get_conf_dir() + Utils::get_dir_separator());

  // We check that it's there and is a directory.
  if (!Glib::file_test (conf_dir, Glib::FILE_TEST_IS_DIR)) {
    // It might be a file, Katoob 0.1 used ~/.katoob as a configuration file.
    if (Glib::file_test (conf_dir, Glib::FILE_TEST_IS_REGULAR))	{
      // Sadly erase it.
      if (g_unlink(conf_dir.c_str())) {
	conf_dir.clear();
	g_warning ("Can't remove old configuration file");
	return false;
      }
    }
    // Let's try to create a directory.
    if (g_mkdir (conf_dir.c_str(), 0700)) {
      conf_dir.clear();
      g_warning ("Can't create configuration directory");
      return false;
    }
  }

  // Now our crashed documents directory.
  if (!Glib::file_test (Utils::get_recovery_dir(), Glib::FILE_TEST_IS_DIR)) {
    if (g_mkdir (Utils::get_recovery_dir().c_str(), 0700)) {
      g_warning ("Can't create recovery directory");
      return false;
    }
  }
  return true;
}

void Conf::load_conf() {
  try {
    _ok = load_from_file(conf_dir + "katoob.conf", (Glib::KeyFileFlags)(Glib::KEY_FILE_KEEP_COMMENTS | Glib::KEY_FILE_KEEP_TRANSLATIONS));
  }
  catch (Glib::KeyFileError& er) {
    if (er.code() == Glib::KeyFileError::NOT_FOUND) {
      _ok = true;
    }
  }
  catch (...) {
  }
}

void Conf::migrate_conf() {
  std::map<std::string, std::string> mp;
  std::map<std::string, std::string>::iterator iter;
  std::string file;
  std::vector<std::string> list;
  std::vector<std::string>::iterator viter;

  std::map<std::string, std::string> confs;
  confs["config"] = "katoob";
  confs["print"] = "print";
  std::map<std::string, std::string>::iterator c_iter;

  for (c_iter = confs.begin(); c_iter != confs.end(); c_iter++) {
    if (load_conf(c_iter->first.c_str(), mp)) {
      file = conf_dir + c_iter->first;
      for (iter = mp.begin(); iter != mp.end(); iter++) {
	set_value(c_iter->second, iter->first, iter->second);
      }
      g_unlink(file.c_str());
    }
    mp.clear();
  }

  std::map<std::string, int> lists;
  std::map<std::string, int>::iterator l_iter;
  lists["recent"] = get("recentno", 10);
  lists["exec-cmd"] = get("exec_cmd_size", 10);
  lists["locations"] = get("locations_size", 10);

  for (l_iter = lists.begin(); l_iter != lists.end(); l_iter++) {
    if (load_list(l_iter->first.c_str(), list)) {
      file = conf_dir + l_iter->first;
      viter = list.begin();
      int x = l_iter->second;
      if (x < list.size()) {
	viter += x;
	list.erase(++viter, ++list.end());
      }
      set_string_list("lists", l_iter->first, list);
      g_unlink(file.c_str());
    }
    list.clear();
  }
}

bool Conf::load_conf(const char *_file, std::map<std::string, std::string>& mp) {
  if (conf_dir.length()) {
    std::string file(conf_dir + _file);
    std::ifstream ifs;
    ifs.open(file.c_str());
    if (ifs.is_open()) {
      std::string buff;
      while (getline(ifs, buff)) {
	int x = buff.find('=');
	int y = x;
	while (buff[--y] == ' ') {
	  ; // Do nothing.
	}
	std::string key(buff, 0, ++y);
	while (buff[++x] == ' ') {
	  ; // Do nothing.
	}
	std::string value(buff, x);
	mp[key] = value;
      }
      ifs.close();
      return true;
    }
  }
  return false;
}

bool Conf::load_list(const char *_file, std::vector<std::string>& mp) {
  if (conf_dir.length()) {
    std::string file = conf_dir;
    file += _file;
    std::ifstream ifs;
    ifs.open(file.c_str());
    if (ifs.is_open()) {
      std::string buff;
      while (getline(ifs, buff)) {
	mp.push_back(buff);
      }
      ifs.close();
      return true;
    }
  }
  return false;
}

void Conf::defaults(Encodings * enc) {
  std::string def_open = "WINDOWS-1256";
  std::string def_save = "UTF-8";
  int _default_open = -1;

  std::string cset;
  Glib::get_charset(cset);

  if (get("locale_enc", false)) {
    _default_open = enc->get(cset);
  }
  else if (get("special_enc", true)) {
    std::string def = get("saved_enc", def_open);
    _default_open = enc->get(def);
  }
  if (_default_open != -1) {
    enc->default_open(_default_open);
  }
  // if it's utf8 or -1, we will default to cp1256
  if ((_default_open == -1) || _default_open == enc->utf8()) {
      enc->default_open(enc->get_by_charset(def_open));
  }

  /* Now for the saving. */
  std::string def = get("save_enc", def_save);
  enc->default_save(enc->get(def));
}

const std::string& Conf::open_dir() {
  if (_open_dir.size()) {
    return _open_dir;
  }
  else {
    _open_dir = Glib::get_current_dir();
    _open_dir += Utils::get_dir_separator();
    return _open_dir;
  }
}

const std::string& Conf::save_dir() {
  if (_save_dir.size()) {
    return _save_dir;
  }
  else {
    _save_dir = Glib::get_current_dir();
    _save_dir += Utils::get_dir_separator();
    return _save_dir;
  }
}

void Conf::open_dir(const std::string& path) {
  if (Glib::file_test (path, Glib::FILE_TEST_IS_DIR)) {
    _open_dir = path + Utils::get_dir_separator();
  }
}

void Conf::save_dir(const std::string& path) {
  if (Glib::file_test (path, Glib::FILE_TEST_IS_DIR)) {
    _save_dir = path + Utils::get_dir_separator();
  }
}

std::string Conf::get_version() {
  std::string ver;
  try {
    return get_string("katoob", "version");
  }
  catch (...) {
    return ver;
  }
}

void Conf::set(const std::string& key, int val) {
  set_integer("katoob", key, val);
}

void Conf::set(const std::string& key, const char *val) {
  set_string("katoob", key, val);
}

void Conf::print_set(const std::string& key, const std::string& val) {
  set_string("print", key, val);
}

void Conf::print_set(const std::string& key, int val) {
  set_integer("print", key, val);
}

void Conf::print_set(const std::string& key, double val) {
  set_double("print", key, val);
}

int Conf::get(const std::string& key, int val) {
  try {
    return get_integer("katoob", key);
  }
  catch (Glib::KeyFileError& er) {
    std::cerr << "Can't get the value of " << key << " : " << er.what() << std::endl;
    set(key, val);
    return val;
  }
}

bool Conf::get(const std::string& key, bool val) {
  try {
    return get_boolean("katoob", key);
  }
  catch (Glib::KeyFileError& er) {
    std::cerr << "Can't get the value of " << key << " : " << er.what() << std::endl;
    set(key, val);
    return val;
  }
}

std::string Conf::get(const std::string& key, const std::string& val) {
  try {
    return get_string("katoob", key);
  }
  catch (Glib::KeyFileError& er) {
    std::cerr << "Can't get the value of " << key << " : " << er.what() << std::endl;
    set(key, val.c_str());
    return val;
  }
}

std::string Conf::get(const std::string& key, const char *val) {
  try {
    return get_string("katoob", key);
  }
  catch (Glib::KeyFileError& er) {
    std::cerr << "Can't get the value of " << key << " : " << er.what() << std::endl;
    set(key, val);
    return val;
  }
}

int Conf::print_get(const std::string& key, int val) {
  try {
    return get_integer("print", key);
  }
  catch (Glib::KeyFileError& er) {
    std::cerr << "Can't get the value of " << key << " : " << er.what() << std::endl;
    print_set(key, val);
    return val;
  }
}

double Conf::print_get(const std::string& key, double val) {
  try {
    return get_double("print", key);
  }
  catch (Glib::KeyFileError& er) {
    std::cerr << "Can't get the value of " << key << " : " << er.what() << std::endl;
    print_set(key, val);
    return val;
  }
}

bool Conf::print_get(const std::string& key, bool val) {
  try {
    return get_boolean("print", key);
  }
  catch (Glib::KeyFileError& er) {
    std::cerr << "Can't get the value of " << key << " : " << er.what() << std::endl;
    print_set(key, val);
    return val;
  }
}

std::string Conf::print_get(const std::string& key, const char *val) {
  try {
    return get_string("print", key);
  }
  catch (Glib::KeyFileError& er) {
    std::cerr << "Can't get the value of " << key << " : " << er.what() << std::endl;
    print_set(key, val);
    return val;
  }
}

std::vector<std::string> Conf::get_list(const std::string& key) {
  try {
    return get_string_list("lists", key);
  }
  catch (Glib::KeyFileError& er) {
    std::cerr << "Can't get the value of " << key << " : " << er.what() << std::endl;
    std::vector<std::string> val;
    return val;
  }
}

void Conf::adjust_lists() {
  adjust_list("recent", get("recentno", 10));
  adjust_list("exec-cmd", get("exec_cmd_size", 10));
  adjust_list("locations", get("locations_size", 10));
}

void Conf::adjust_list(const std::string& list, int size) {
  std::vector<std::string> lst;
  try {
    lst = get_string_list("lists", list);
  }
  catch(...) {

  }

  adjust_list(lst, size);
  set_string_list("lists", list, lst);
}

void Conf::list_prepend(const std::string& list, const std::string& item, int size) {
  std::vector<std::string> lst;
  try {
    lst = get_string_list("lists", list);
  }
  catch(...) {

  }

  // Let's insert it.
  lst.insert(lst.begin(), item);

  // let's remove it if it's in the vector.
  // We are starting from 1 not 0 to avoid deleting what we have just inserted.
  for (unsigned x = 1; x < lst.size(); x++) {
    if (lst[x] == item) {
      std::vector<std::string>::iterator iter = lst.begin();
      iter += x;
      lst.erase(iter);
      break;
    }
  }

  adjust_list(lst, size);
  set_string_list("lists", list, lst);
}

void Conf::adjust_list(std::vector<std::string>& list, int size) {
  // Now let's truncate it if needed.
  if (list.size() > size) {
    size++;
    std::vector<std::string>::iterator iter = list.begin();
    iter += size;
    list.erase(iter, list.end());
  }
}

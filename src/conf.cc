/*
 * conf.cc
 *
 * This file is part of Katoob.
 *
 * Copyright (C) 2008-2021 Fred Morcos <fm+Katoob@fredmorcos.com>
 * Copyright (C) 2002-2007 Mohammed Sameer <msameer@foolab.org>
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

#include "conf.hh"
#include "utils.hh"
#include <fstream>
#include <glib/gstdio.h>
#include <glibmm.h>
#include <iostream>
#include <sstream>

Conf::Conf(Encodings &_encodings): _ok(false)
{
  if (!prepare_dir()) {
    return;
  }

  load_conf();
  load_list();
  defaults(_encodings);
}

Conf::~Conf()
{
  if (!prepare_dir()) {
    return;
  }

  adjust_lists();
  save_list();

  std::string ver = get_version();
  if (ver.size() == 0) {
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
  } else {
    if (!prepare_dir()) {
      return;
    }
    save_conf();
  }
}

void Conf::save_conf()
{
  save_conf("config", config);
  save_conf("print", print);
}

void Conf::save_list()
{
  save_list("recent", recent);
  save_list("exec-cmd", exec_cmd);
  save_list("locations", locations);
}

void Conf::save_conf(const char *_file, std::map<std::string, std::string> &mp)
{
  if (configDir.length()) {
    std::string file = configDir;
    file += _file;
    std::ofstream ofs;
    ofs.open(file.c_str());
    if (ofs.is_open()) {
      std::map<std::string, std::string>::iterator iter;
      for (iter = mp.begin(); iter != mp.end(); iter++) {
        ofs << iter->first << " = " << iter->second << std::endl;
      }
      ofs.close();
    }
  }
}

void Conf::save_list(const char *_file, std::vector<std::string> &mp)
{
  if (cacheDir.length()) {
    std::string file = cacheDir;
    file += _file;
    std::ofstream ofs;
    ofs.open(file.c_str());
    if (ofs.is_open()) {
      for (unsigned x = 0; x < mp.size(); x++) {
        ofs << mp[x] << std::endl;
      }
      ofs.close();
    }
  }
}

bool Conf::prepare_dir()
{
  configDir = Utils::configDir() + Utils::get_dir_separator();
  cacheDir = Utils::cacheDir() + Utils::get_dir_separator();

  if (g_mkdir_with_parents(cacheDir.c_str(), 0700)) {
    cacheDir.clear();
    g_warning("Cannot create cache directory %s", cacheDir.c_str());
    return false;
  }

  // We check that it's there and is a directory.
  if (!Glib::file_test(configDir, Glib::FILE_TEST_IS_DIR)) {
    // It might be a file, Katoob 0.1 used ~/.katoob as a configuration file.
    if (Glib::file_test(configDir, Glib::FILE_TEST_IS_REGULAR)) {
      // Sadly erase it.
      if (remove(configDir.c_str())) {
        configDir.clear();
        g_warning("Can't remove old configuration file");
        return false;
      }
    }
    // Let's try to create a directory.
    if (g_mkdir_with_parents(configDir.c_str(), 0700)) {
      configDir.clear();
      g_warning("Can't create configuration directory");
      return false;
    }
  }

  // Now our crashed documents directory.
  if (!Glib::file_test(Utils::recoveryDir(), Glib::FILE_TEST_IS_DIR)) {
    if (g_mkdir_with_parents(Utils::recoveryDir().c_str(), 0700)) {
      g_warning("Can't create recovery directory");
      return false;
    }
  }
  return true;
}

void Conf::load_conf()
{
  _ok = load_conf("config", config);
  load_conf("print", print);
  adjust_lists();
}

bool Conf::load_conf(const char *_file, std::map<std::string, std::string> &mp)
{
  if (configDir.length()) {
    std::string file(configDir + _file);
    std::ifstream ifs;
    ifs.open(file.c_str());
    if (ifs.is_open()) {
      std::string buff;
      while (getline(ifs, buff)) {
        int x = buff.find('=');
        int y = x;
        while (buff[--y] == ' ') {
          ;   // Do nothing.
        }
        std::string key(buff, 0, ++y);
        while (buff[++x] == ' ') {
          ;   // Do nothing.
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

void Conf::load_list()
{
  load_list("recent", recent);
  load_list("exec-cmd", exec_cmd);
  load_list("locations", locations);
}

void Conf::load_list(const gchar *_file, std::vector<std::string> &mp)
{
  if (cacheDir.length()) {
    std::string file = cacheDir;
    file += _file;
    std::ifstream ifs;
    ifs.open(file.c_str());
    if (ifs.is_open()) {
      std::string buff;
      while (getline(ifs, buff)) {
        mp.push_back(buff);
      }
      ifs.close();
    }
  }
}

double Conf::_get(std::map<std::string, std::string> &m, const char *key, double val)
{
  std::map<std::string, std::string>::iterator iter;
  iter = m.find(key);
  if (iter == m.end()) {
    // We'll set it.
    std::stringstream str;
    str << val;
    m[key] = str.str();
#ifndef NDEBUG
    std::cerr << __PRETTY_FUNCTION__ << " Shouldn't be here, Got key " << key << std::endl;
#endif
    return val;
  }
  return atof(iter->second.c_str());
}

int Conf::_get(std::map<std::string, std::string> &m, const char *key, int val)
{
  std::map<std::string, std::string>::iterator iter;
  iter = m.find(key);
  if (iter == m.end()) {
    // We'll set it.
    std::stringstream str;
    str << val;
    m[key] = str.str();
#ifndef NDEBUG
    std::cerr << __PRETTY_FUNCTION__ << " Shouldn't be here, Got key " << key << std::endl;
#endif
    return val;
  }
  return atoi(iter->second.c_str());
}

bool Conf::_get(std::map<std::string, std::string> &m, const char *key, bool val)
{
  std::map<std::string, std::string>::iterator iter;
  iter = m.find(key);
  if (iter == m.end()) {
    // We'll set it.
    std::stringstream str;
    str << (val ? 1 : 0);
    m[key] = str.str();
#ifndef NDEBUG
    std::cerr << __PRETTY_FUNCTION__ << " Shouldn't be here, Got key " << key << std::endl;
#endif
    return val;
  }
  return atoi(iter->second.c_str()) == 0 ? false : true;
}

std::string Conf::_get(std::map<std::string, std::string> &m, const char *key, std::string &val)
{
  std::map<std::string, std::string>::iterator iter;
  iter = m.find(key);
  if (iter == m.end()) {
    // We'll set it.
    m[key] = val;
#ifndef NDEBUG
    std::cerr << __PRETTY_FUNCTION__ << " Shouldn't be here, Got key " << key << std::endl;
#endif
    return val;
  }
  return iter->second;
}

void Conf::_set(std::map<std::string, std::string> &m, const char *key, double val)
{
  std::stringstream str;
  str << val;
  m[key] = str.str();
}

void Conf::_set(std::map<std::string, std::string> &m, const char *key, int val)
{
  std::stringstream str;
  str << val;
  m[key] = str.str();
}

void Conf::list_prepend(std::vector<std::string> &list, const std::string &file, unsigned size)
{
  // Let's insert it.
  list.insert(list.begin(), file);

  // let's remove it if it's in the vector.
  // We are starting from 1 not 0 to avoid deleting what we have just inserted.
  for (unsigned x = 1; x < list.size(); x++) {
    if (list[x] == file) {
      std::vector<std::string>::iterator iter = list.begin();
      iter += x;
      list.erase(iter);
      break;
    }
  }

  // Now let's truncate it if needed.
  if (list.size() > size) {
    size++;
    std::vector<std::string>::iterator iter = list.begin();
    iter += size;
    list.erase(iter, list.end());
  }
}

void Conf::defaults(Encodings &enc)
{
  std::string def_open = "WINDOWS-1256";
  std::string def_save = "UTF-8";
  int _default_open = -1;

  std::string cset;
  Glib::get_charset(cset);

  if (get("locale_enc", false)) {
    _default_open = enc.get(cset);
  } else if (get("special_enc", true)) {
    std::string def = get("saved_enc", def_open);
    _default_open = enc.get(def);
  }
  if (_default_open != -1) {
    enc.default_open(_default_open);
  }
  // if it's utf8 or -1, we will default to cp1256
  if ((_default_open == -1) || _default_open == enc.utf8()) {
    enc.default_open(enc.get_by_charset(def_open));
  }

  /* Now for the saving. */
  std::string def = get("save_enc", def_save);
  enc.default_save(enc.get(def));
}

const std::string &Conf::open_dir()
{
  if (_open_dir.size()) {
    return _open_dir;
  } else {
    _open_dir = Glib::get_current_dir();
    _open_dir += Utils::get_dir_separator();
    return _open_dir;
  }
}

const std::string &Conf::save_dir()
{
  if (_save_dir.size()) {
    return _save_dir;
  } else {
    _save_dir = Glib::get_current_dir();
    _save_dir += Utils::get_dir_separator();
    return _save_dir;
  }
}

void Conf::open_dir(const std::string &path)
{
  if (Glib::file_test(path, Glib::FILE_TEST_IS_DIR)) {
    _open_dir = path + Utils::get_dir_separator();
  }
}

void Conf::save_dir(const std::string &path)
{
  if (Glib::file_test(path, Glib::FILE_TEST_IS_DIR)) {
    _save_dir = path + Utils::get_dir_separator();
  }
}

void Conf::adjust_lists()
{
  // recent
  unsigned x = get("recentno", 10);
  std::vector<std::string>::iterator start;
  std::vector<std::string>::iterator end;

  if (x < recent.size()) {
    start = recent.begin();
    end = recent.end();
    end++;
    start += x;
    start++;
    recent.erase(start, end);
  }

  // exec-cmd
  x = get("exec_cmd_size", 10);
  if (x < exec_cmd.size()) {
    start = exec_cmd.begin();
    end = exec_cmd.end();
    end++;
    start += x;
    start++;
    exec_cmd.erase(start, end);
  }
}

std::string Conf::get_version()
{
  std::map<std::string, std::string>::iterator iter;
  std::string ver;
  iter = config.find("version");
  if (iter == config.end()) {
    return ver;
  } else {
    return iter->second;
  }
}

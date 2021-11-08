/*
 * conf.hh
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

#include "encodings.hh"
#include <map>
#include <string>
#include <vector>

class Conf {
 public:
  Conf(Encodings &);
  ~Conf();

  const std::vector<std::string> &get_recent()
  {
    return recent;
  };
  void recent_prepend(const std::string &file)
  {
    list_prepend(recent, file, get("recentno", 10));
  }

  const std::vector<std::string> &get_exec_cmd()
  {
    return exec_cmd;
  };
  void exec_cmd_prepend(const std::string &file)
  {
    list_prepend(exec_cmd, file, get("exec_cmd_size", 10));
  }

  const std::vector<std::string> &get_locations()
  {
    return locations;
  };
  void locations_prepend(const std::string &file)
  {
    list_prepend(locations, file, get("locations_size", 10));
  }

  void set(const char *key, int val)
  {
    _set(config, key, val);
  }
  void set(const char *key, const char *val)
  {
    _set(config, key, val);
  }
  void print_set(const char *key, const char *val)
  {
    _set(print, key, val);
  }
  void print_set(const char *key, int val)
  {
    _set(print, key, val);
  }
  void print_set(const char *key, double val)
  {
    _set(print, key, val);
  }
  int get(const char *key, int val)
  {
    return _get(config, key, val);
  }
  bool get(const char *key, bool val)
  {
    return _get(config, key, val);
  }
  std::string get(const char *key, std::string &val)
  {
    return _get(config, key, val);
  }
  std::string get(const char *key, const char *val)
  {
    std::string v(val);
    return _get(config, key, v);
  }

  int print_get(const char *key, int val)
  {
    return _get(print, key, val);
  }
  double print_get(const char *key, double val)
  {
    return _get(print, key, val);
  }
  bool print_get(const char *key, bool val)
  {
    return _get(print, key, val);
  }
  std::string print_get(const char *key, std::string &val)
  {
    return _get(print, key, val);
  }
  std::string print_get(const char *key, const char *val)
  {
    std::string v(val);
    return _get(print, key, v);
  }

  const std::string &open_dir();
  const std::string &save_dir();
  void open_dir(const std::string &);
  void save_dir(const std::string &);
  void defaults(Encodings &);
  std::string get_version();
  void adjust_lists();

  bool ok()
  {
    return _ok;
  }

 private:
  Conf(const Conf &);
  Conf &operator=(const Conf &);

  bool prepare_dir();
  void load_conf();
  void load_list();
  void save_conf();
  void save_list();
  bool load_conf(const char *, std::map<std::string, std::string> &);
  void load_list(const char *, std::vector<std::string> &);
  void save_conf(const char *, std::map<std::string, std::string> &);
  void save_list(const char *, std::vector<std::string> &);

  void _set(std::map<std::string, std::string> &m, const char *key, bool val)
  {
    m[key] = val ? "1" : "0";
  }
  void _set(std::map<std::string, std::string> &, const char *, int);
  void _set(std::map<std::string, std::string> &, const char *, double);
  void _set(std::map<std::string, std::string> &m, const char *key, const char *val)
  {
    m[key] = val;
  }
  int _get(std::map<std::string, std::string> &, const char *, int);
  double _get(std::map<std::string, std::string> &, const char *, double);
  bool _get(std::map<std::string, std::string> &, const char *, bool);
  std::string _get(std::map<std::string, std::string> &, const char *, std::string &);

  void list_prepend(std::vector<std::string> &, const std::string &, unsigned);

  std::string _open_dir;
  std::string _save_dir;

  std::vector<std::string> recent;
  std::vector<std::string> exec_cmd;
  std::vector<std::string> locations;
  std::map<std::string, std::string> config;

  std::map<std::string, std::string> print;
  std::string configDir;
  std::string cacheDir;
  bool _ok;
};

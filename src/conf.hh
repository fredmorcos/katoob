/*
 * conf.hh
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

#ifndef __CONFIG_HH__
#define __CONFIG_HH__

#ifndef ENABLE_MAEMO
#include <glibmm/keyfile.h>
#else
#include "maemo-wrapper.hh"
#endif

#include <vector>
#include <string>
#include <map>

// Forward declaration.
class Encodings;

class Conf : private Glib::KeyFile {
public:
  Conf(Encodings *);
  ~Conf();

  void adjust_lists();
  void adjust_list(const std::string&, int);
  void list_prepend(const std::string&, const std::string&, int);

  void set(const std::string&, int);
  void set(const std::string&, const char *);
  void print_set(const std::string&, const std::string&);
  void print_set(const std::string&, int);
  void print_set(const std::string&, double);

  int get(const std::string&, int);
  bool get(const std::string&, bool);
  std::string get(const std::string&, const std::string&);
  std::string get(const std::string&, const char *);

  int print_get(const std::string&, int);
  double print_get(const std::string&, double);
  bool print_get(const std::string&, bool);
  std::string print_get(const std::string&, const char *);

  std::vector<std::string> get_list(const std::string&);

  const std::string& open_dir();
  const std::string& save_dir();
  void open_dir(const std::string&);
  void save_dir(const std::string&);
  void defaults(Encodings *);
  std::string get_version();

  bool ok() { return _ok; }

private:
  Conf(const Conf&);
  Conf& operator=(const Conf&);

  bool prepare_dir();
  void load_conf();
  void migrate_conf();

  void adjust_list(std::vector<std::string>&, int);
  void save_conf();
  bool load_conf(const char *, std::map<std::string, std::string>&);
  bool load_list(const char *, std::vector<std::string>&);

  std::string _open_dir;
  std::string _save_dir;

  std::string conf_dir;
  bool _ok;
};

#endif /* __CONFIG_HH__ */

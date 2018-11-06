/*
 * utils.hh
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

#ifndef __UTILS_HH__
#define __UTILS_HH__

#include <string>
#include <glibmm/ustring.h>
#include <pangomm/attributes.h>
#include <sys/stat.h>
#include <gtkmm/label.h>
#include "conf.hh"

#define KATOOB_UNUSED(x)

namespace Utils {
  typedef enum {
    KATOOB_COLOR_READONLY,
    KATOOB_COLOR_MODIFIED,
    KATOOB_COLOR_NORMAL
  } KatoobColor;

  std::string get_data_dir();
  std::string get_dir_separator();
  std::string get_data_path(const char *);
  std::string prepend_home_dir(char *);
  bool inline is_lam_alef(Glib::ustring&, gunichar);
  bool is_lam_alef(const Glib::ustring&, Glib::ustring&);
  bool katoob_file_is_writable(std::string&);
  void katoob_set_color(Conf&, Gtk::Label&, KatoobColor);
  void katoob_set_color(Conf&, Gtk::Label *, KatoobColor);
  bool katoob_write(Conf&, std::string&, std::string&, std::string&);
  bool katoob_write(const char *, const char *, unsigned, std::string&);
  bool katoob_read(const std::string&, std::string&);
  void katoob_set_perms(const char *, const struct stat&);
  bool file_copy(const char *, const char *, std::string&);
  std::string substitute(const std::string&, const int);
  std::string substitute(const std::string&, const std::string&);
  std::string substitute(const std::string&, const std::string&, const std::string&);
  std::string substitute(const std::string& orig, const int, const int);
  std::string substitute(const std::string&, const int, const int, const std::string&);
  std::string substitute(const std::string&, const std::string&, const int);
  std::string substitute(const std::string&, const std::string&, const int, const std::string&, const std::string&);
  std::string get_recovery_dir();
  std::string get_conf_dir();
  bool create_recovery_file(std::string&, int&);
  bool get_recovery_files(std::map<std::string, std::string>&, std::string&);
  std::string get_recovery_template(std::string = "XXXXXX");

  std::string katoob_get_default_font();
  std::vector<std::string> split(const std::string&, const char);
  bool lock_file(int, std::string&);
  bool unlock_file(int, std::string&);
  bool file_is_locked(int fd);
  bool file_is_locked(const std::string&);
};

#endif /* __UTILS_HH__ */

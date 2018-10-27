/*
 * maemo-wrapper.hh
 * This file is part of katoob
 *
 * Copyright (C) 2007, 2008 Mohammed Sameer
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

#ifndef __MAEMO_WRAPPER_HH__
#define __MAEMO_WRAPPER_HH__

#include <string>
#include <gconfmm/client.h>
#include <glibmmconfig.h>

class MaemoProxy {
public:
  static bool get_info(std::string&, int&, bool&, std::string&, std::string&);
private:
  static void signal_value_changed_cb(const Glib::ustring&, const Gnome::Conf::Value&);

  static std::string _host, _user, _pass;
  static int _port;
  static bool _auth, _use_proxy;
  static Glib::RefPtr<Gnome::Conf::Client> cc;
};

#if defined (ENABLE_MAEMO) and !defined(ENABLE_DBUS)
#error dbus is needed for maemo.
#endif

#if GLIBMM_MINOR_VERSION < 14

namespace Glib {
  typedef enum {
    KEY_FILE_NONE = 0,
    KEY_FILE_KEEP_COMMENTS = 1 << 0,
    KEY_FILE_KEEP_TRANSLATIONS = 1 << 1
  } KeyFileFlags;

  class KeyFileError {
  public:
    typedef enum {
      UNKNOWN_ENCODING,
      PARSE,
      NOT_FOUND,
      KEY_NOT_FOUND,
      GROUP_NOT_FOUND,
      INVALID_VALUE
    } Code;

    KeyFileError(GError *);
    ~KeyFileError();
    std::string what();
    Code code();
  private:
    GError *_err;
  };

  class KeyFile {
  public:
    KeyFile();
    ~KeyFile();

    std::string to_data();
    bool load_from_file(const std::string&, const Glib::KeyFileFlags&) throw(Glib::KeyFileError);
    void set_value(const Glib::ustring&, const Glib::ustring&, const Glib::ustring&);

    void set_string_list(const Glib::ustring&, const Glib::ustring&, const Glib::ArrayHandle<Glib::ustring>&);
    Glib::ArrayHandle<Glib::ustring> get_string_list(const Glib::ustring&, const Glib::ustring&);

    std::string get_string(const Glib::ustring&, const Glib::ustring&);
    int get_integer(const Glib::ustring&, const Glib::ustring&);
    bool get_boolean(const Glib::ustring&, const Glib::ustring&);
    double get_double(const Glib::ustring&, const Glib::ustring&);
    void set_integer(const Glib::ustring&, const Glib::ustring&, int);
    void set_string(const Glib::ustring&, const Glib::ustring&, const std::string&);
    void set_double(const Glib::ustring&, const Glib::ustring&, double);
  private:
    GKeyFile *_file;
  };
};

#endif

#endif /* __MAEMO_WRAPPER_HH__ */

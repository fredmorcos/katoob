/*
 * maemo-wrapper.cc
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

#include "maemo-wrapper.hh"

// TODO: wrap the maemo connection API.

bool MaemoProxy::get_info(std::string& host, int& port, bool& auth, std::string& user, std::string& pass) {
  // TODO: We only use the HTTP proxy.
  // TODO: We are ignoring error.
  //  GConfClient *cc = gconf_client_get_default();
  if (!cc) {
    cc = Gnome::Conf::Client::get_default_client();
    cc->signal_value_changed().connect(sigc::ptr_fun(&MaemoProxy::signal_value_changed_cb));
    std::auto_ptr<Glib::Error> err;
    cc->add_dir("/system/http_proxy", Gnome::Conf::CLIENT_PRELOAD_ONELEVEL, err);
    _host = cc->get_string("/system/http_proxy/host", err);
    _user = cc->get_string("/system/http_proxy/authentication_user", err);
    _pass = cc->get_string("/system/http_proxy/authentication_password", err);
    _port = cc->get_int("/system/http_proxy/port", err);
    _auth = cc->get_bool("/system/http_proxy/use_authentication", err);
    _use_proxy = cc->get_bool("/system/http_proxy/use_http_proxy", err);
  }
  host = _host;
  port = _port;
  auth = _auth;
  user = _user;
  pass = _pass;
  return _use_proxy;
}

void MaemoProxy::signal_value_changed_cb(const Glib::ustring& key, const Gnome::Conf::Value& val) {
  if (key == "/system/http_proxy/use_http_proxy") {
    _use_proxy = val.get_bool();
  }
  else if (key == "/system/http_proxy/host") {
    _host = val.get_string();
  }
  else if (key == "/system/http_proxy/port") {
    _port = val.get_int();
  }
  else if (key == "/system/http_proxy/use_authentication") {
    _auth = val.get_bool();
  }
  else if (key == "/system/http_proxy/authentication_user") {
    _user = val.get_string();
  }
  else if (key == "/system/http_proxy/authentication_password") {
    _pass = val.get_string();
  }
}

Glib::RefPtr<Gnome::Conf::Client> MaemoProxy::cc;
std::string MaemoProxy::_host, MaemoProxy::_user, MaemoProxy::_pass;
int MaemoProxy::_port;
bool MaemoProxy::_auth, MaemoProxy::_use_proxy;


#if GLIBMM_MINOR_VERSION < 14

namespace Glib {
  KeyFileError::KeyFileError(GError *e) : _err(e) {}
  KeyFileError::~KeyFileError() {
    g_error_free(_err);
  }
  std::string KeyFileError::what() {
    return _err->message;
  }

  KeyFileError::Code KeyFileError::code() {
    return static_cast<Code>(_err->code);
  }

  KeyFile::KeyFile() {
    _file = g_key_file_new();
  }

  KeyFile::~KeyFile() {
    if (_file) {
      g_key_file_free(_file);
    }
  }

  std::string KeyFile::to_data() {
    std::string ret;
    char *data = g_key_file_to_data(_file, NULL, NULL);
    if (data) {
      ret = data;
      g_free(data);
    }

    return ret;
  }

  bool KeyFile::load_from_file(const std::string& file, const Glib::KeyFileFlags& flags) throw(Glib::KeyFileError) {
    GError *error = NULL;
    bool ret = g_key_file_load_from_file(_file, file.c_str(), GKeyFileFlags(flags), &error);
    if (error) {
      throw(KeyFileError(error));
      return ret;
    }
    return ret;
  }

  void KeyFile::set_value(const Glib::ustring& gp, const Glib::ustring& k, const Glib::ustring& v) {
    g_key_file_set_value(_file, gp.c_str(), k.c_str(), v.c_str());
  }

  void KeyFile::set_string_list(const Glib::ustring& gp, const Glib::ustring& k, const Glib::ArrayHandle<Glib::ustring>& list) {
    g_key_file_set_string_list(_file, gp.c_str(), k.c_str(), list.data(), list.size());
  }

  Glib::ArrayHandle<Glib::ustring> KeyFile::get_string_list(const Glib::ustring& gp, const Glib::ustring& k) {
    GError *error = NULL;
    gchar** ret = NULL;
    gsize len = 0;
    ret = g_key_file_get_string_list(_file, gp.c_str(), k.c_str(), &len, &error);
    if (error) {
      throw(KeyFileError(error));
    }
    return Glib::ArrayHandle<Glib::ustring>(ret, len, Glib::OWNERSHIP_DEEP);
  }


  std::string KeyFile::get_string(const Glib::ustring& gp, const Glib::ustring& k) {
    std::string ret;
    GError *error = NULL;

    char *res = g_key_file_get_string(_file, gp.c_str(), k.c_str(), &error);
    if (error) {
      throw(KeyFileError(error));
    }
    ret = res;
    g_free(res);
    return ret;
  }

  int KeyFile::get_integer(const Glib::ustring& gp, const Glib::ustring& k) {
    int ret;
    GError *error = NULL;
    ret = g_key_file_get_integer(_file, gp.c_str(), k.c_str(), &error);
    if (error) {
      throw(KeyFileError(error));
    }
    return ret;
  }

  bool KeyFile::get_boolean(const Glib::ustring& gp, const Glib::ustring& k) {
    bool ret;
    GError *error = NULL;
    ret = g_key_file_get_boolean(_file, gp.c_str(), k.c_str(), &error);
    if (error) {
      throw(KeyFileError(error));
    }
    return ret;
  }

  double KeyFile::get_double(const Glib::ustring& gp, const Glib::ustring& k) {
    double ret;
    GError *error = NULL;
    ret = g_key_file_get_double(_file, gp.c_str(), k.c_str(), &error);
    if (error) {
      throw(KeyFileError(error));
    }
    return ret;
  }

  void KeyFile::set_integer(const Glib::ustring& gp, const Glib::ustring& k, int v) {
    g_key_file_set_integer(_file, gp.c_str(), k.c_str(), v);
  }

  void KeyFile::set_string(const Glib::ustring& gp, const Glib::ustring& k, const std::string& v) {
    g_key_file_set_string(_file, gp.c_str(), k.c_str(), v.c_str());
  }

  void KeyFile::set_double(const Glib::ustring& gp, const Glib::ustring& k, double v) {
    g_key_file_set_double(_file, gp.c_str(), k.c_str(), v);
  }
};

#endif

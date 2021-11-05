/*
 * maemo-wrapper.cc
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

#include "maemo-wrapper.hh"

// TODO: wrap the maemo connection API.

bool MaemoProxy::get_info(std::string &host,
                          int &port,
                          bool &auth,
                          std::string &user,
                          std::string &pass)
{
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

void MaemoProxy::signal_value_changed_cb(const Glib::ustring &key, const Gnome::Conf::Value &val)
{
  if (key == "/system/http_proxy/use_http_proxy") {
    _use_proxy = val.get_bool();
  } else if (key == "/system/http_proxy/host") {
    _host = val.get_string();
  } else if (key == "/system/http_proxy/port") {
    _port = val.get_int();
  } else if (key == "/system/http_proxy/use_authentication") {
    _auth = val.get_bool();
  } else if (key == "/system/http_proxy/authentication_user") {
    _user = val.get_string();
  } else if (key == "/system/http_proxy/authentication_password") {
    _pass = val.get_string();
  }
}

Glib::RefPtr<Gnome::Conf::Client> MaemoProxy::cc;
std::string MaemoProxy::_host, MaemoProxy::_user, MaemoProxy::_pass;
int MaemoProxy::_port;
bool MaemoProxy::_auth, MaemoProxy::_use_proxy;

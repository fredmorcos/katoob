/*
 * maemo-wrapper.hh
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

#pragma once

#ifdef ENABLE_MAEMO
#include <gconfmm/client.h>
#include <string>

class MaemoProxy {
 public:
  static bool get_info(std::string &, int &, bool &, std::string &, std::string &);

 private:
  static void signal_value_changed_cb(const Glib::ustring &, const Gnome::Conf::Value &);

  static std::string _host, _user, _pass;
  static int _port;
  static bool _auth, _use_proxy;
  static Glib::RefPtr<Gnome::Conf::Client> cc;
};
#endif

#if defined(ENABLE_MAEMO) and !defined(ENABLE_DBUS)
#error dbus is needed for maemo.
#endif

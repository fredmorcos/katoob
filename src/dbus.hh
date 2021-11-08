/*
 * dbus.hh
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

#include <sigc++/signal.h>
#include <string>
#include <vector>

#define DBUS_API_SUBJECT_TO_CHANGE 1
#include <dbus/dbus-glib-lowlevel.h>
#include <dbus/dbus.h>

class DBus {
 public:
  DBus();
  ~DBus();

  static auto ping() -> bool;
  void start();

  static auto open_files(std::vector<std::string> &) -> bool;

  friend auto katoob_dbus_message_handler(DBusConnection *, DBusMessage *, void *)
      -> DBusHandlerResult;

  auto signal_open_files_event() -> sigc::signal<void, std::vector<std::string> &>;

 protected:
  auto got_message(DBusConnection *, DBusMessage *) -> DBusHandlerResult;

 private:
  auto open_files(DBusConnection *, DBusMessage *) -> DBusHandlerResult;
  static auto pong(DBusConnection *, DBusMessage *) -> DBusHandlerResult;
  static auto connect(DBusConnection **) -> bool;
  DBusConnection *server;
  bool _ok;

  sigc::signal<void, std::vector<std::string> &> signal_open_files;
};

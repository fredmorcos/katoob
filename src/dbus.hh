/*
 * dbus.hh
 * This file is part of katoob
 *
 * Copyright (C) 2002-2007 Mohammed Sameer
 * Copyright (C) 2008-2018 Frederic-Gerald Morcos
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

#pragma once

#include <vector>
#include <string>
#include <sigc++/signal.h>

#define DBUS_API_SUBJECT_TO_CHANGE 1
#include <dbus/dbus.h>
#include <dbus/dbus-glib-lowlevel.h>

class DBus {
public:
  DBus();
  ~DBus();
  bool ping();
  void start();

  bool open_files(std::vector<std::string>&);

  friend DBusHandlerResult katoob_dbus_message_handler(DBusConnection *, DBusMessage *, void *);

  sigc::signal<void, std::vector<std::string>& >signal_open_files;

protected:
  DBusHandlerResult got_message(DBusConnection *, DBusMessage *);

private:
  DBusHandlerResult open_files(DBusConnection *, DBusMessage *);
  DBusHandlerResult pong(DBusConnection *, DBusMessage *);
  bool connect(DBusConnection **);
  DBusConnection *server;
  bool _ok;
};

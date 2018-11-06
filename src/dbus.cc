/*
 * dbus.cc
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

#include <iostream>
#include "dbus.hh"

#define BUS_NAME "org.foolab.katoob"
#define BUS_PATH "/org/foolab/katoob"

#define OPEN_FILES "OpenFiles"
#define PING "Ping"

DBusHandlerResult katoob_dbus_message_handler(DBusConnection *connection,
                                              DBusMessage *message,
                                              void *user_data)
{
  return static_cast<DBus *>(user_data)->got_message(connection, message);
}

DBus::DBus() :
  server(NULL) {
}

DBus::~DBus() {
  if (server) {
    dbus_connection_unref(server);
  }
}

void DBus::start() {
  //  assert(server = NULL);
  if (!connect(&server)) {
    return;
  }

  DBusObjectPathVTable katoob_dbus_vtable =
    {NULL, katoob_dbus_message_handler, NULL, NULL, NULL, NULL};

  if (!dbus_connection_register_object_path(server, BUS_PATH, &katoob_dbus_vtable, this)) {
    std::cerr << "Not enough memory to register dbus object path." << std::endl;
    return;
  }
}

bool DBus::connect(DBusConnection **conn) {
  DBusError error;
  dbus_error_init(&error);
  *conn = dbus_bus_get (DBUS_BUS_SESSION, &error);
  if (!*conn) {
    std::cerr << error.message << std::endl;
    dbus_error_free (&error);
    return false;
  }

  dbus_connection_set_exit_on_disconnect(*conn, false);
  dbus_connection_setup_with_g_main(*conn, NULL);
  dbus_bus_request_name(*conn, BUS_NAME, 0, &error);
  if (dbus_error_is_set(&error)) {
    // TODO: handle the last call properly ??
    std::cerr << error.message << std::endl;
    dbus_error_free (&error);
    return false;
  }
  dbus_error_free (&error);
  return true;
}

DBusHandlerResult DBus::got_message(DBusConnection *connection, DBusMessage *message) {
  if ((!connection) || (!message)) {
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
  }

  std::string method = dbus_message_get_member(message);

  if (method == PING) {
    return pong(connection, message);
  } else if (method == OPEN_FILES) {
    return open_files(connection, message);
  } else {
    std::cerr << "Katoob: Unhandled DBus event: " << method << std::endl;
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
  }
}

DBusHandlerResult DBus::open_files(DBusConnection *connection, DBusMessage *message) {
  DBusError error;
  bool done = true;
  int len;
  char **list;

  dbus_error_init(&error);

  if (!dbus_message_get_args(message, &error, DBUS_TYPE_ARRAY, DBUS_TYPE_STRING, &list, &len, DBUS_TYPE_INVALID)) {
    std::cerr << error.message << std::endl;
    dbus_error_free (&error);
    done = false;
  }

  DBusMessage *reply = dbus_message_new_method_return(message);
  if (!reply) {
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
  }

  dbus_message_append_args(reply, DBUS_TYPE_BOOLEAN, &done, DBUS_TYPE_INVALID);
  dbus_connection_send(connection, reply, NULL);
  dbus_message_unref(reply);

  std::vector<std::string> files;
  for (int i = 0; i < len;i++) {
    files.push_back(list[i]);
  }
  // dbus_connection_flush
  signal_open_files.emit(files);
  return DBUS_HANDLER_RESULT_HANDLED;
}

DBusHandlerResult DBus::pong(DBusConnection *connection, DBusMessage *message) {
  DBusError error;
  bool done = true;

  dbus_error_init(&error);

  DBusMessage *reply = dbus_message_new_method_return(message);
  if (!reply) {
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
  }

  dbus_message_append_args(reply, DBUS_TYPE_BOOLEAN, &done, DBUS_TYPE_INVALID);
  dbus_connection_send(connection, reply, NULL);
  dbus_message_unref(reply);
  return DBUS_HANDLER_RESULT_HANDLED;
}

bool DBus::ping() {
  DBusConnection *con;
  DBusMessage *msg, *res;
  DBusError error;
  dbus_error_init(&error);
  const char *_ping = PING;

  con = dbus_bus_get(DBUS_BUS_SESSION, &error);
  if (!con) {
    std::cerr << error.message << std::endl;
    dbus_error_free(&error);
    return false;
  }
  msg = dbus_message_new_method_call (BUS_NAME, BUS_PATH, BUS_NAME, PING);
  dbus_message_set_auto_start(msg, FALSE);

  dbus_message_append_args(msg, DBUS_TYPE_STRING, &_ping, DBUS_TYPE_INVALID);
  res = dbus_connection_send_with_reply_and_block(con, msg, 2000, &error);
  dbus_message_unref(msg);
  if (res == NULL) {
    std::cerr << error.message << std::endl;
    dbus_error_free(&error);
    return false;
  }

  if (dbus_message_get_type (res) == DBUS_MESSAGE_TYPE_ERROR) {
    dbus_set_error_from_message(&error, res);
    std::cerr << error.message << std::endl;
    dbus_message_unref (res);
    dbus_error_free (&error);
    return false;
  }

  /* Assume it worked */
  dbus_message_unref(res);
  return true;
}

bool DBus::open_files(std::vector<std::string>& _files) {
  gchar **files = g_new(gchar *, _files.size()+1);
  for (unsigned i = 0; i < _files.size(); i++) {
    // We need to add absolute paths
    if (g_path_is_absolute(_files[i].c_str())) {
      files[i] = g_strdup(_files[i].c_str());
    }
    else {
      char *dir = g_get_current_dir();
      char *file = g_build_filename(dir, _files[i].c_str(), NULL);
      g_free(dir);
      files[i] = g_strdup(file);
      g_free(file);
    }
  }
  files[_files.size()] = NULL;

  DBusConnection *con;
  DBusMessage *msg, *res;
  DBusError error;
  dbus_error_init(&error);

  con = dbus_bus_get(DBUS_BUS_SESSION, &error);
  if (!con) {
    std::cerr << error.message << std::endl;
    dbus_error_free(&error);
    g_strfreev(files);
    return false;
  }
  msg = dbus_message_new_method_call (BUS_NAME, BUS_PATH, BUS_NAME, OPEN_FILES);
  dbus_message_set_auto_start(msg, FALSE);

  dbus_message_append_args(msg, DBUS_TYPE_ARRAY, DBUS_TYPE_STRING, &files, g_strv_length(files), DBUS_TYPE_INVALID);
  res = dbus_connection_send_with_reply_and_block(con, msg, 2000, &error);
  dbus_message_unref(msg);
  if (res == NULL) {
    std::cerr << error.message << std::endl;
    dbus_error_free(&error);
    g_strfreev(files);
    return false;
  }

  if (dbus_message_get_type (res) == DBUS_MESSAGE_TYPE_ERROR) {
    dbus_set_error_from_message(&error, res);
    std::cerr << error.message << std::endl;
    dbus_message_unref (res);
    dbus_error_free (&error);
    g_strfreev(files);
    return false;
  }

  /* Assume it worked */
  dbus_message_unref(res);
  g_strfreev(files);
  return true;
}

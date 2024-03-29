/*
 * dbus.cc
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

#include <config.h>

#include <iostream>
//#include <cassert>
#include "dbus.hh"

#define OPEN_FILES "OpenFiles"
#define PING       "Ping"

auto katoob_dbus_message_handler(DBusConnection *connection, DBusMessage *message, void *user_data)
    -> DBusHandlerResult
{
  return static_cast<DBus *>(user_data)->got_message(connection, message);
}

DBus::DBus(): server(nullptr), _ok(false)
{
}

DBus::~DBus()
{
  if (server != nullptr) {
    dbus_connection_unref(server);
  }
}

void DBus::start()
{
  //  assert(server = NULL);
  if (!connect(&server)) {
    return;
  }

  DBusObjectPathVTable katoob_dbus_vtable =
      {nullptr, katoob_dbus_message_handler, nullptr, nullptr, nullptr, nullptr};

  if (dbus_connection_register_object_path(server, APP_PATH, &katoob_dbus_vtable, this) == 0) {
    std::cerr << "Not enough memory to register dbus object path." << std::endl;
    return;
  }
}

auto DBus::connect(DBusConnection **conn) -> bool
{
  DBusError error;
  dbus_error_init(&error);
  *conn = dbus_bus_get(DBUS_BUS_SESSION, &error);
  if (*conn == nullptr) {
    std::cerr << error.message << std::endl;
    dbus_error_free(&error);
    return false;
  }

  dbus_connection_set_exit_on_disconnect(*conn, 0);
  dbus_connection_setup_with_g_main(*conn, nullptr);
  dbus_bus_request_name(*conn, APP_ID, 0, &error);
  if (dbus_error_is_set(&error) != 0) {
    // TODO: handle the last call properly ??
    std::cerr << error.message << std::endl;
    dbus_error_free(&error);
    return false;
  }
  dbus_error_free(&error);
  return true;
}

auto DBus::got_message(DBusConnection *connection, DBusMessage *message) -> DBusHandlerResult
{
  if ((connection == nullptr) || (message == nullptr)) {
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
  }

  std::string method = dbus_message_get_member(message);

  if (method == PING) {
    return pong(connection, message);
  }

  if (method == OPEN_FILES) {
    return open_files(connection, message);
  }

  std::cerr << "Katoob: Unhandled DBus event: " << method << std::endl;
  return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

auto DBus::open_files(DBusConnection *connection, DBusMessage *message) -> DBusHandlerResult
{
  DBusError error;
  bool done = true;
  int len;
  char **list;

  dbus_error_init(&error);

  if (!dbus_message_get_args(message,
                             &error,
                             DBUS_TYPE_ARRAY,
                             DBUS_TYPE_STRING,
                             &list,
                             &len,
                             DBUS_TYPE_INVALID)) {
    std::cerr << error.message << std::endl;
    dbus_error_free(&error);
    done = false;
  }

  DBusMessage *reply = dbus_message_new_method_return(message);
  if (reply == nullptr) {
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
  }

  dbus_message_append_args(reply, DBUS_TYPE_BOOLEAN, &done, DBUS_TYPE_INVALID);
  dbus_connection_send(connection, reply, nullptr);
  dbus_message_unref(reply);

  std::vector<std::string> files;
  files.reserve(len);

  for (int i = 0; i < len; i++) {
    files.emplace_back(list[i]);
  }

  // dbus_connection_flush
  signal_open_files.emit(files);
  return DBUS_HANDLER_RESULT_HANDLED;
}

auto DBus::signal_open_files_event() -> sigc::signal<void, std::vector<std::string> &>
{
  return signal_open_files;
}

auto DBus::pong(DBusConnection *connection, DBusMessage *message) -> DBusHandlerResult
{
  DBusError error;
  bool done = true;

  dbus_error_init(&error);

  DBusMessage *reply = dbus_message_new_method_return(message);
  if (reply == nullptr) {
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
  }

  dbus_message_append_args(reply, DBUS_TYPE_BOOLEAN, &done, DBUS_TYPE_INVALID);
  dbus_connection_send(connection, reply, nullptr);
  dbus_message_unref(reply);
  return DBUS_HANDLER_RESULT_HANDLED;
}

auto DBus::ping() -> bool
{
  DBusConnection *con;
  DBusMessage *msg;
  DBusMessage *res;
  DBusError error;
  dbus_error_init(&error);
  const char *_ping = PING;

  con = dbus_bus_get(DBUS_BUS_SESSION, &error);
  if (con == nullptr) {
    std::cerr << error.message << std::endl;
    dbus_error_free(&error);
    return false;
  }
  msg = dbus_message_new_method_call(APP_ID, APP_PATH, APP_ID, PING);
  dbus_message_set_auto_start(msg, FALSE);

  dbus_message_append_args(msg, DBUS_TYPE_STRING, &_ping, DBUS_TYPE_INVALID);
  res = dbus_connection_send_with_reply_and_block(con, msg, 2000, &error);
  dbus_message_unref(msg);
  if (res == nullptr) {
    std::cerr << error.message << std::endl;
    dbus_error_free(&error);
    return false;
  }

  if (dbus_message_get_type(res) == DBUS_MESSAGE_TYPE_ERROR) {
    dbus_set_error_from_message(&error, res);
    std::cerr << error.message << std::endl;
    dbus_message_unref(res);
    dbus_error_free(&error);
    return false;
  }

  /* Assume it worked */
  dbus_message_unref(res);
  return true;
}

auto DBus::open_files(std::vector<std::string> &_files) -> bool
{
  gchar **files = g_new(gchar *, _files.size() + 1);
  for (unsigned i = 0; i < _files.size(); i++) {
    // We need to add absolute paths
    if (g_path_is_absolute(_files[i].c_str())) {
      files[i] = g_strdup(_files[i].c_str());
    } else {
      char *dir = g_get_current_dir();
      char *file = g_build_filename(dir, _files[i].c_str(), NULL);
      g_free(dir);
      files[i] = g_strdup(file);
      g_free(file);
    }
  }
  files[_files.size()] = nullptr;

  DBusConnection *con;
  DBusMessage *msg;
  DBusMessage *res;
  DBusError error;
  dbus_error_init(&error);

  con = dbus_bus_get(DBUS_BUS_SESSION, &error);
  if (con == nullptr) {
    std::cerr << error.message << std::endl;
    dbus_error_free(&error);
    g_strfreev(files);
    return false;
  }
  msg = dbus_message_new_method_call(APP_ID, APP_PATH, APP_ID, OPEN_FILES);
  dbus_message_set_auto_start(msg, FALSE);

  dbus_message_append_args(msg,
                           DBUS_TYPE_ARRAY,
                           DBUS_TYPE_STRING,
                           &files,
                           g_strv_length(files),
                           DBUS_TYPE_INVALID);
  res = dbus_connection_send_with_reply_and_block(con, msg, 2000, &error);
  dbus_message_unref(msg);
  if (res == nullptr) {
    std::cerr << error.message << std::endl;
    dbus_error_free(&error);
    g_strfreev(files);
    return false;
  }

  if (dbus_message_get_type(res) == DBUS_MESSAGE_TYPE_ERROR) {
    dbus_set_error_from_message(&error, res);
    std::cerr << error.message << std::endl;
    dbus_message_unref(res);
    dbus_error_free(&error);
    g_strfreev(files);
    return false;
  }

  /* Assume it worked */
  dbus_message_unref(res);
  g_strfreev(files);
  return true;
}

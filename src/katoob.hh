/*
 * katoob.hh
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

#ifndef __KATOOB_HH__
#define __KATOOB_HH__

#include <vector>
#include <gtkmm/main.h>

#ifdef ENABLE_MAEMO
#include <libosso.h>
#endif

// forward declaration
class Window;
class Conf;
class Encodings;
class DBus;

/**
 * \brief This is our application entry point.
 */
class Katoob : public Gtk::Main {
public:
  Katoob(int argc, char *argv[]) throw();
  ~Katoob();

  void run();

  void quit_cb();
private:
#ifdef ENABLE_MAEMO
  static void hw_event_handler(osso_hw_state_t *, gpointer);
#endif

  void create_window();
  void parse(int argc, char *argv[]);
  void print_usage();
  void print_help();
  void print_version();

  /** \brief This is our signal callback. */
  static void signal_cb(int);

  /** \brief An instance of the Encodings class. */
  Encodings *encodings;

  /** \brief An instance of the Conf class. */
  Conf *conf;

#ifdef ENABLE_DBUS
  /** \brief An instance of the DBus class (If compiled with DBus support). */
  DBus *dbus;
#endif /* ENABLE_DBUS */

  /** \brief our Windows are stored here */
  static std::vector<Window *> children;

  /** \brief the files we are passed are stored here by our Katoob::parse function. */
  std::vector<std::string> files;

#ifdef ENABLE_MAEMO
  /** \brief the osso_context returned by osso_initialize() (Only if maemo support is enabled). */
  osso_context_t* osso_context;
#endif
};

#endif /* __KATOOB_HH__ */

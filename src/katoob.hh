/*
 * katoob.hh
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

#include "conf.hh"
#include "encodings.hh"
#include "window.hh"
#include <gtkmm.h>
#include <vector>

#ifdef ENABLE_DBUS
#include "dbus.hh"
#endif

/**
 * \brief This is our application entry point.
 */
class Katoob: public Gtk::Main {
 public:
  Katoob(int argc, char *argv[]);
  ~Katoob();

  int run();
  void window();

  void quit_cb();

 private:
  void parse(int argc, char *argv[]);
  void usage();
  void help();
  void version();

  /** \brief This is our signal callback. */
  static void signal_cb(int);

  /** \brief An instance of the Encodings class. */
  Encodings encodings;

  /** \brief An instance of the Conf class. */
  Conf conf;

#ifdef ENABLE_DBUS
  /** \brief An instance of the DBus class (If compiled with DBus support). */
  DBus dbus;
#endif /* ENABLE_DBUS */

  /** \brief our Windows are stored here */
  static std::vector<Window *> children;

  /** \brief the files we are passed are stored here by our Katoob::parse function. */
  std::vector<std::string> files;
};

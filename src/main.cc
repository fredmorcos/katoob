/*
 * main.cc
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
#include <system_error>
#include <cstring>
#include <errno.h>
#include <config.h>
#include <glibmm/thread.h>
#include "katoob.hh"
#include "macros.h"
//#include <csignal>

/**
 * \brief Our entry point.
 *
 * Our entry point. Will setup gettext, initialize gthread,
 * construct a Katoob instance, ask Katoob to create a Window and ask Katoob
 * to run its main loop.
 */
int main(int argc, char *argv[]) {
  (void) bindtextdomain(PACKAGE, PACKAGE_LOCALE_DIR);
  (void) bind_textdomain_codeset(PACKAGE, "UTF-8");
  (void) textdomain(PACKAGE);

  //  signal(SIGPIPE, SIG_IGN);

  Katoob katoob(argc, argv);
  katoob.window();
  return katoob.run();
}

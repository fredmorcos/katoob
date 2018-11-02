/*
 * main.cc
 * This file is part of katoob
 *
 * Copyright (C) 2006, 2007 Mohammed Sameer
 * Copyright (C) 2008 Frederic-Gerald Morcos
 * Copyright (C) 2018 Frederic-Gerald Morcos
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

#include <config.h>
#include <glibmm/thread.h>
#include "katoob.hh"
#include "macros.h"

/**
 * \brief Our entry point.
 *
 * Our entry point. Will setup gettext, initialize gthread,
 * construct a Katoob instance, ask Katoob to create a Window and ask Katoob
 * to run its main loop.
 */
int main(int argc, char *argv[]) {
#ifdef ENABLE_NLS
  bindtextdomain (PACKAGE, PACKAGE_LOCALE_DIR);
  bind_textdomain_codeset (PACKAGE, "UTF-8");
  textdomain (PACKAGE);
#endif

 try {
   Katoob app = Katoob(argc, argv);
   app.run();
   return 0;
 } catch (Glib::Error& err) {
   return err.code();
 }
}

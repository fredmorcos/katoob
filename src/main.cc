/*
 * main.cc
 * This file is part of katoob
 *
 * Copyright (C) 2006, 2007 Mohammed Sameer
 * 		 2008 Frederic-Gerald Morcos
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

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
#ifdef ENABLE_NLS
  bindtextdomain (PACKAGE, PACKAGE_LOCALE_DIR);
  bind_textdomain_codeset (PACKAGE, "UTF-8");
  textdomain (PACKAGE);
#endif

  Katoob *katoob = NULL;

#ifdef ENABLE_MAEMO
 try {
#endif
   katoob = new Katoob(argc, argv);
#ifdef ENABLE_MAEMO
 }
 catch (Glib::Error& er) {
   if (katoob) {
     delete katoob;
   }
   return er.code();
 }
#endif

 katoob->run();

 delete katoob;

 return 0;
}


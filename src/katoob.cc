/*
 * katoob.cc
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

#include <config.h>
#include <iostream>
#include "katoob.hh"
#include "network.hh"
#include "dialogs.hh"
#include "macros.h"
//#include "utils.hh"
#ifdef ENABLE_MAEMO
#include <hildonmm/init.h>
#include <hildonmm/program.h>
#include <hildon-fmmm/init.h>
#endif
#include <csignal>

/**
 * \brief constructor.
 *
 * The constructor will initialize Gtk::Main, call Katoob::parse to parse the command
 * line arguments we get.
 * If compiled with DBus sypport, it will try to check if there's a running instance
 * of katoob. If it finds one, we will message it to open Katoob::files.
 * If we can send the message, we will exit. Otherwise we
 * will start DBus::start
 */
Katoob::Katoob(int argc, char *argv[]) :
  Gtk::Main(argc, argv), conf(encodings) {
#ifdef ENABLE_MAEMO
  osso_context = NULL;

  osso_context = osso_initialize(PACKAGE, VERSION, true, NULL);
  if(!osso_context) {
    std::cerr << "osso_initialize() failed." << std::endl;
    return;
  }

  // We listen to any libosso events.
  /* Add handler for hardware D-BUS messages */
  int result = osso_hw_set_event_cb(osso_context, NULL, Katoob::hw_event_handler, NULL);
  if (result != OSSO_OK) {
    g_print("Error setting HW state callback (%d)\n", result);
    osso_deinitialize(osso_context);
    osso_context = NULL;
    return;
  }

  /* Add handler for Exit D-BUS messages */
  /*
  result = osso_application_set_exit_cb(Katoob::osso_context, exit_event_handler, NULL);
  if (result != OSSO_OK) {
    g_print("Error setting exit callback (%d)\n", result);
    osso_deinitialize(osso_context);
    osso_context = NULL;
    return;
  }
  */
  Hildon::init();
  Hildon::fm_init();
#endif

  Network net(conf);

  parse(argc, argv);

#ifdef ENABLE_DBUS
  if (dbus.ping()) {
    if (dbus.open_files(files)) {
      exit(0);
    }
    else {
      dbus.start();
    }
  }
  else {
    dbus.start();
  }
#endif
#ifdef ENABLE_HIGHLIGHT
  SourceManager::init();
#endif

  Glib::set_application_name(PACKAGE);
  // Let's connect our signals.

  int signals[] = {SIGILL,  // Illegal instruction.
		   SIGABRT, // Abort signal from abort()
		   SIGFPE, // Floating point exception
		   SIGTERM, // Termination signal
		   SIGSEGV, // Invalid memory reference (Segmentation violation)
		   SIGINT, // Interrupt from keyboard
		   SIGBUS, // Bus error (bad memory access)
		   SIGXCPU, // CPU time limit exceeded
		   SIGXFSZ, // File size limit exceeded
		   0x0
  };

  int *sig = signals;
  while (*sig) {
    signal(*sig, signal_cb);
    sig++;
  }

  signal(SIGPIPE, SIG_IGN); // Broken pipe: write to pipe with no readers
  signal(SIGHUP, SIG_IGN); //  Hangup detected on controlling terminal or death of controlling process
}

/**
 * \brief destructor.
 */
Katoob::~Katoob() {
#ifdef ENABLE_MAEMO
  if (osso_context) {
    osso_deinitialize(osso_context);
  }
#endif
  for (unsigned x = 0; x < children.size(); x++) {
    delete children[x];
  }
  children.clear();
  Network::destroy();
}

/**
 * \brief signal(7) handler.
 *
 * This handles any signal(7) we might get.
 * \param signum the signal number.
 */
void Katoob::signal_cb(int signum) {
  static volatile int s = 0;
  if (s) {
    return;
  }
  ++s;

  std::cerr << "We received a signal (" << signum << "): " << strsignal(signum) << std::endl;
  for (unsigned x = 0; x < children.size(); x++) {
    children[x]->autosave();
  }
  //  katoob_error(Utils::substitute(_("Katoob crashed (%s). Katoob tried to save all the open documents. They will be recovered the next time you run Katoob."), strsignal(signum)));
  exit(255);
}

/**
 * \brief parse the command line arguments.
 *
 * Parse the command line arguments and take actions accordingly.
 * If we are passed one or more files, we will put them in Katoob::files.
 * \param argc the argc parameter passed to main().
 * \param argv the argv parameter passed to main().
 */
void Katoob::parse(int argc, char *argv[]) {
  for (int x = 1; x < argc; x++) {
    if ((!strcmp(argv[x], "-u")) || (!strcmp(argv[x], "--usage"))) {
      usage();
      exit(0);
    }
    if ((!strcmp(argv[x], "-h")) || (!strcmp(argv[x], "--help"))) {
      help();
      exit(0);
    }
    if ((!strcmp(argv[x], "-v")) || (!strcmp(argv[x], "--version"))) {
      version();
      exit(0);
    }
    files.push_back(argv[x]);
  }
}

/**
 * \brief run the main loop (We call Gtk::Main::run()).
 * \return always 0
 */
int Katoob::run() {
  Gtk::Main::run();
  return 0;
}

/**
 * \brief create a new Window
 */
void Katoob::window() {
  // TODO: Use open_files() instead of passing them to the constructor ??
  Window *win = new Window(conf, encodings, files);
  win->signal_quit.connect(sigc::mem_fun(*this, &Katoob::quit_cb));
  children.push_back(win);
#ifdef ENABLE_DBUS
  dbus.signal_open_files.connect(sigc::mem_fun(win, &Window::open_files));
#endif
#ifdef ENABLE_MAEMO
#ifdef ENABLE_DBUS
  dbus.signal_request_top.connect(sigc::mem_fun(win, &Window::signal_request_top_cb));
#endif
  Hildon::Program::get_instance()->add_window(*win);
#endif
}

/**
 * \brief print usage (--usage).
 */
void Katoob::usage() {
  std::cout << "usage: katoob [--help] [--version] [--usage] [file1 file2 file3... ]" << std::endl;
}

/**
 * \brief print our version (--version).
 */
void Katoob::version() {
  std::cout << PACKAGE << " " << VERSION << std::endl;
}

/**
 * \brief print the help (--help).
 */
void Katoob::help() {
  std::cout << "usage: katoob  [OPTIONS] [FILES_TO_OPEN]" << std::endl
	    << "  -h, --help       Show this help message" << std::endl
	    << "  -v, --version    Display version information" << std::endl
	    << "  -u, --usage      Display brief usage message" << std::endl;
}

/**
 * \brief quit the Gtk::Main loop by calling Gtk::Main::quit()
 */
void Katoob::quit_cb() {
  Gtk::Main::quit();
}

#ifdef ENABLE_MAEMO
bool Katoob::ok() {
  return osso_context != NULL;
}

int Katoob::get_error() {
  return OSSO_ERROR;
}

void Katoob::hw_event_handler(osso_hw_state_t *state, gpointer data) {
  if ((state->shutdown_ind) || (state->save_unsaved_data_ind)) {
    for (unsigned x = 0; x < children.size(); x++) {
      children[x]->autosave();
    }
    if (state->shutdown_ind) {
      Gtk::Main::quit();
    }
  }

    if (state->memory_low_ind) {
      // Maybe trim our closed windows list ?
    }
}
/*
void Katoob::exit_event_handler(gboolean die_now, gpointer data) {
  for (unsigned x = 0; x < children.size(); x++) {
    children[x]->autosave();
  }
  Gtk::Main::quit();
}
*/
#endif

std::vector<Window *> Katoob::children;

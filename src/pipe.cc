/*
 * pipe.cc
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <unistd.h>
#include <cerrno>
#include "pipe.hh"
#include <sys/types.h>
#include <sys/wait.h>
#include "macros.h"
#include "utils.hh"

/**
 * \brief constructor.
 * \param command the command to execute without any arguments.
 */
Pipe::Pipe(const std::string& command) : _ok(false) {
  std::string res;

  std::string::size_type w = command.find(" ");

  // If we get nothing, We will check for the whole string.
  if (w == std::string::npos) {
    res = Glib::find_program_in_path(command);
  }
  else {
    res = Glib::find_program_in_path(command.substr(0, w));
  }

  if (res == "") {
    _err = Utils::substitute(_("I can't execute %s. I can't find it in your path"), command);
  }
  else {
    _ok = true;
  }
}

/** \brief destructor. */
Pipe::~Pipe() {

}

/**
 * \brief execute a command and read its output.
 * \param command the command with all its arguments.
 * \param out a reference to a std::string to store the output.
 * \param err a reference to a std::string to store any error.
 * \return true upon success or false otherwise.
 */
bool Pipe::exec(const std::string& command, std::string& out, std::string& err) {
  FILE *p = popen(command.c_str(), "r");
  if (!p) {
    err = strerror(errno);
    return false;
  }

  size_t n;
  char c;

  while (true) {
    // TODO: Bad, One character at a time.
    n = fread(&c, 1, 1, p);
    if (n == 0) {
      break;
    }
    if (n != 1) {
      pclose(p);
      err = strerror(errno);
      return false;
    }
    out += c;
  }
  int status;
  wait(&status);
  if (status != 0) {
    // TODO: Possible some commands might not return 0
    err = _("Couldn't get the required content.");
    return false;
  }
  int x = pclose(p);
  if (errno == ECHILD) {
    return true;
  }
  // ECHILD
  // Probably it will always return -1 ?
  if (x == -1) {
    err = strerror(errno);
    return false;
  }
  return true;
}

/**
 * \brief check whether the command can be executed or not.
 *
 * Usually, this should be called after creating a Pipe object to check that the command
 * passed to the constructor can be executed.
 *
 * \param err a reference to a std::string to store any error.
 * \return true if the command can be executed. false otherwise.
 */
bool Pipe::ok(std::string& err) {
  if (_ok) {
    return true;
  }
  else {
    err = _err;
    return false;
  }
}

/**
 * \brief execute a ccommand directly and captures its output.
 * \param command1 the command to execute without any arguments.
 * \param command2 the command with all its arguments.
 * \param out a reference to a std::string to store the output.
 * \param error a reference to a std::string to store any error.
 * \return true upon success or false otherwise.
 */
bool Pipe::exec(const std::string& command1, const std::string& command2, std::string& out, std::string& error) {
  Pipe pipe(command1);
  if (!pipe.ok(error)) {
    return false;
  }
  if (pipe.exec(command2, out, error)) {
    return true;
  }
  return false;
}

/*
 * tempfile.cc
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

#include "tempfile.hh"
#include "utils.hh"
#include <cerrno>
#include <glibmm.h>
#ifdef _WIN32
#include <io.h>
#endif
TempFile::TempFile():
 _ok(false),
 name(Glib::get_tmp_dir() + Utils::get_dir_separator() + "katoobXXXXXX")
{
  fd = Glib::mkstemp(name);
  if (fd == -1) {
    err = strerror(errno);
  } else {
    _ok = true;
  }
}

TempFile::~TempFile()
{
  close(fd);
  unlink(name.c_str());
}

bool TempFile::write(const std::string &what, std::string &err)
{
  ssize_t n = ::write(fd, what.c_str(), what.size());
  if (n == -1) {
    err = strerror(errno);
    return false;
  }
  return true;
}

bool TempFile::read(std::string &what, std::string &err)
{
  // TODO: We are reading character by character.
  int n;
  char c;
  while (true) {
    n = ::read(fd, &c, 1);

    if (n == 0) {
      return true;
    }
    if (n == -1) {
      err = strerror(errno);
      return false;
    }
    what += c;
  }
  return true;
}

bool TempFile::ok(std::string &e)
{
  e = err;
  return _ok;
}

std::string &TempFile::get_name()
{
  return name;
}

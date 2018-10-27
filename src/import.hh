/*
 * import.hh
 * This file is part of katoob
 *
 * Copyright (C) 2006, 2008 Mohammed Sameer
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

#ifndef __IMPORT_HH__
#define __IMPORT_HH__

#include <glibmm/ustring.h>

// TODO: Asynchronous.

class Import {
public:
  Import(const std::string& _name, bool(*_func)(std::string&, std::string&))
    : name(_name), func(_func) {}
  std::string name;
  bool(*func)(std::string&, std::string&);
};

void import_init(std::vector<Import *>&);
bool katoob_import_html(std::string&, std::string&);
#ifdef HAVE_GZIP
bool katoob_import_gz(std::string&, std::string&);
#endif
#ifdef HAVE_BZIP2
bool katoob_import_bz2(std::string&, std::string&);
#endif
#ifdef HAVE_FRIBIDI
bool katoob_import_bidi_shape(std::string&, std::string&);
#endif

#endif /* __IMPORT_HH__ */

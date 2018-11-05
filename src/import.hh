/*
 * import.hh
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

#pragma once

#include <vector>
#include <glibmm/ustring.h>

// TODO: Asynchronous.

class Import {
public:
  std::string name;
  bool(*func)(std::string&, std::string&);
};

void import_init(std::vector<Import>&);
bool katoob_import_html(std::string&, std::string&);
#ifdef HAVE_GZIP
bool katoob_import_gz(std::string&, std::string&);
#endif
#ifdef HAVE_BZIP2
bool katoob_import_bz2(std::string&, std::string&);
#endif

bool katoob_import_bidi_shape(std::string&, std::string&);

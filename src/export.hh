/*
 * export.hh
 * This file is part of katoob
 *
 * Copyright (C) 2002-2007 Mohammed Sameer
 * Copyright (C) 2008-2021 Frederic-Gerald Morcos
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
#include <string>
#include <glibmm/ustring.h>

// TODO: Asynchronous.

class Export {
public:
  std::string name;
  bool(*func)(Glib::ustring&, std::string&, std::string&);
  bool lines;
};

void export_init(std::vector<Export>&);
bool katoob_export_plain(Glib::ustring&, std::string&, std::string&);
bool katoob_export_gz(Glib::ustring&, std::string&, std::string&);
bool katoob_export_bz2(Glib::ustring&, std::string&, std::string&);
bool katoob_export_bidi_shape(Glib::ustring&, std::string&, std::string&);

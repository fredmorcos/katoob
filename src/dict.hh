/*
 * dict.hh
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

#include <string>
#include <vector>
#include <map>
#include "conf.hh"

// TODO: Auth not implemented
// TODO: strategies not implemented.
// TODO: Make the errors in DICTClient translatable.

namespace Dict {
  std::string construct_uri(Conf&, const std::string&);
  std::string construct_lsdb_uri(Conf&, const std::string&, int);
  bool parse_defs(const std::string&, std::vector<std::string>&);
  bool parse_dbs(const std::string&, std::map<std::string, std::string>&);
};

/*
 * isocodes.hh
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

#include <expat.h>
#include <map>
#include <string>

class IsoCodes {
public:
  IsoCodes();
  //  ~IsoCodes();
  //  bool ok(std::string&);
  std::string translate(const std::string&);
  void country_codes(const char *, const char **);
  void lang_codes(const char *, const char **);

private:
  void filter(const std::string&,
              const char **,
              const std::string&,
              const std::string&,
              const std::string&,
              std::map<std::string, std::string>&,
              const char *);
  static std::map<std::string, std::string> _lang_codes, _country_codes;
};

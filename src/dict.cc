/*
 * dict.cc
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

#include "dict.hh"
#include "macros.h"
#include "utils.hh"

auto Dict::construct_uri(Conf &conf, const std::string &word) -> std::string
{
  std::string host = conf.get("dict_host", "dict.arabeyes.org");
  std::string book = conf.get("dict_db", "arabic");

  const int defaultPort = 2628;
  int port = conf.get("dict_port", defaultPort);

  return Utils::substitute("dict://%s:%i/d:%s:%s", host, port, word, book);
}

auto Dict::construct_lsdb_uri(const std::string &host, int port) -> std::string
{
  return Utils::substitute("dict://%s:%i/show db", host, port);
}

auto Dict::parse_defs(const std::string &str, std::vector<std::string> &res) -> bool
{
  std::size_t num = 0;

  if (str.empty()) {
    return false;
  }

  std::vector<std::string> parts = Utils::split(str, '\n');
  if (parts.empty()) {
    return false;
  }

  for (unsigned x = 0; x < parts.size(); x++) {
    if (parts[x].size() > 3) {
      std::string code = parts[x].substr(0, 3);

      if (code == "550") {
        res.emplace_back(_("Invalid daatabase. Please reconfigure the dictionary in use."));
        return false;
      }

      if (code == "552") {
        res.emplace_back(_("No matches were found"));
        return false;
      }

      if (code == "150") {
        std::string::size_type idx = parts[x].find(' ', 0);
        if (idx == std::string::npos) {
          return false;
        }

        std::string::size_type idx2 = parts[x].find(' ', idx + 1);
        if (idx2 == std::string::npos) {
          return false;
        }

        num = atoi(parts[x].substr(idx + 1, idx2 - idx - 1).c_str());

        if (num == 0) {
          return false;
        }
      } else if (code == "151") {
        std::string def;
        ++x;
        while (x < parts.size()) {
          if (parts[x][0] != '.') {
            def += parts[x];
            ++x;
          } else {
            res.push_back(def);
            def.clear();
            //	    ++x;
            break;
          }
        }
      }
    }
  }

  return res.size() == num;
}

auto Dict::parse_dbs(const std::string &str, std::map<std::string, std::string> &res) -> bool
{
  if (str.empty()) {
    return false;
  }

  std::vector<std::string> parts = Utils::split(str, '\n');
  if (parts.empty()) {
    return false;
  }

  for (unsigned x = 0; x < parts.size(); x++) {
    if (parts[x].size() > 3) {
      if (parts[x].substr(0, 3) == "554") {
        res[_("No databases were found.")] = "";
        return false;
      }

      if (parts[x].substr(0, 3) == "110") {
        std::string::size_type idx = parts[x].find(' ', 0);
        if (idx == std::string::npos) {
          return false;
        }

        std::string::size_type idx2 = parts[x].find(' ', idx + 1);
        if (idx2 == std::string::npos) {
          return false;
        }

        int num = atoi(parts[x].substr(idx + 1, idx2 - idx - 1).c_str());
        if (num == 0) {
          res[_("No databases were found.")] = "";
          return false;
        }
        while (num != 0) {
          std::string line = parts[++x];
          std::string::size_type sp = line.find(' ', 0);
          std::string db = line.substr(0, sp);
          std::string desc = line.substr(++sp, std::string::npos);

          sp = desc.find_first_of('\"');
          if (sp != std::string::npos) {
            desc = desc.substr(++sp);
          }

          sp = desc.find_last_of('\"');
          if (sp != std::string::npos) {
            desc = desc.substr(0, sp);
          }
          res[db] = desc;
          --num;
        }
        return true;
      }
    }
  }

  return false;
}

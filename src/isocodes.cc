/*
 * isocodes.cc
 * This file is part of katoob
 *
 * Copyright (C) 2007 Mohammed Sameer
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

// #ifdef HAVE_CONFIG_H
#include <config.h>
// #endif /* HAVE_CONFIG_H */

#include "isocodes.hh"
#include "utils.hh"
#include <cassert>
#include "macros.h"

#define LANG_CODES "iso_639"
#define COUNTRY_CODES "iso_3166"
#define ISOCODESLOCALEDIR ISOCODES_PREFIX "/share/locale"

static void __country_codes(void *data, const char *el, const char **attr) {
  return static_cast<IsoCodes *>(data)->country_codes(el, attr);
}

static void __lang_codes(void *data, const char *el, const char **attr) {
  return static_cast<IsoCodes *>(data)->lang_codes(el, attr);
}

IsoCodes::IsoCodes() {
  std::string str;
  std::string file;
  if ((_lang_codes.size() == 0) || (_country_codes.size() == 0)) {
    XML_Parser p = XML_ParserCreate(NULL);
    if (!p) {
      //      error = _("Couldn't allocate memory for parser");
      //      _ok = false;
      return;
    }
    if (_lang_codes.size() == 0) {
      bindtextdomain (LANG_CODES, ISOCODESLOCALEDIR);
      bind_textdomain_codeset (LANG_CODES, "UTF-8");
      file = Utils::substitute("%s/share/xml/iso-codes/%s.xml", ISOCODES_PREFIX, LANG_CODES);
      Utils::katoob_read(file, str);

      XML_SetUserData(p, this);
      XML_SetElementHandler(p, __lang_codes, NULL);
      XML_Parse(p, str.c_str(), str.size(), 1);
      XML_ParserReset(p, NULL);
    }
    if (_country_codes.size() == 0) {
      bindtextdomain(COUNTRY_CODES, ISOCODESLOCALEDIR);
      bind_textdomain_codeset (COUNTRY_CODES, "UTF-8");
      file = Utils::substitute("%s/share/xml/iso-codes/%s.xml", ISOCODES_PREFIX, COUNTRY_CODES);
      Utils::katoob_read(file, str);

      XML_SetUserData(p, this);
      XML_SetElementHandler(p, __country_codes, NULL);
      XML_Parse(p, str.c_str(), str.size(), 1);
    }
    XML_ParserFree(p);
  }
}

void IsoCodes::country_codes(const char *elem, const char **attrs) {
  return filter(elem, attrs, "iso_3166_entry", "alpha_2_code", "name", _country_codes, COUNTRY_CODES);
}

void IsoCodes::lang_codes(const char *elem, const char **attrs) {
  return filter(elem, attrs, "iso_639_entry", "iso_639_1_code", "name", _lang_codes, LANG_CODES);
}

void IsoCodes::filter(const std::string& elem, const char **attrs, const std::string& tag, const std::string& attr1, const std::string& attr2, std::map<std::string, std::string>& mp, const char *domain) {
  if (elem != tag) {
    return;
  }

  const char *a1 = NULL, *a2 = NULL;
  for (int i = 0; attrs[i]; i+=2) {
    if (attr1 == attrs[i]) {
      a1 = attrs[i+1];
    }
    else if (attr2 == attrs[i]) {
      a2 = dgettext(domain, attrs[i+1]);
    }
  }

  if (a1 && a2) {
    mp[a1] = a2;
  }
}

std::string IsoCodes::translate(const std::string& str) {
  std::string::size_type loc = str.find("_", 0);
  if (loc != std::string::npos) {
    std::string l(str, 0, loc);
    std::string c(str, ++loc);

    std::map<std::string, std::string>::iterator iter1 = _lang_codes.find(l);
    std::map<std::string, std::string>::iterator iter2 = _country_codes.find(c);

    if ((iter1 != _lang_codes.end()) && (iter2 != _country_codes.end())) {
      return Utils::substitute(_("%s (%s)"), iter1->second, iter2->second);
    }
    else {
      return str;
    }
  }
  else {
    std::map<std::string, std::string>::iterator iter = _lang_codes.find(str);
    if (iter == _lang_codes.end()) {
      return str;
    }
    else {
      return iter->second;
    }
  }

  std::map<std::string, std::string>::iterator iter = _lang_codes.find(str);
  if (iter == _lang_codes.end()) {
    return str;
  }

  return iter->second;
}

std::map<std::string, std::string> IsoCodes::_lang_codes, IsoCodes::_country_codes;

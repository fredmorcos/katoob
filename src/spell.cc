/*
 * spell.cc
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

#include "spell.hh"
#include <cassert>
#include "macros.h"

Spell::Spell() :
  broker(NULL),
  dict(NULL) {
  broker = enchant_broker_init();
}

Spell::~Spell() {
  if (dict) {
    enchant_broker_free_dict(broker, dict);
    dict = NULL;
  }
  if (broker) {
    enchant_broker_free(broker);
  }
  broker = NULL;
}

bool Spell::ok(std::string& error) {
  if (broker) {
    return true;
  }
  else {
    error = _("Failed to initialize the spell checker");
    return false;
  }
}

bool Spell::set_lang(std::string& lang, std::string& error) {
  EnchantDict *_dict = enchant_broker_request_dict(broker, lang.c_str());
  if (!_dict) {
    const char *err = enchant_broker_get_error(broker);
    if (err) {
      error = err;
    }
    else {
      error = _("Failed to set the requested dictionary.");
    }
    return false;
  }
  if (dict) {
    enchant_broker_free_dict(broker, dict);
  }
  dict = _dict;
  return true;
}

bool Spell::check(std::string& word) {
  assert(dict != NULL);
  return enchant_dict_check(dict, word.c_str(), -1) == 0;
}

void Spell::suggest(std::string& word, std::vector<std::string>& sugg) {
  assert(dict != NULL);

  size_t n;
  char ** words = enchant_dict_suggest(dict, word.c_str(), -1, &n);
  if ((!n) || (!words)) {
    return;
  }
  for (unsigned x = 0; x < n; x++) {
    sugg.push_back(words[x]);
  }
  enchant_dict_free_string_list(dict, words);
}

void Spell::replace(std::string& a, std::string& b) {
  assert(dict != NULL);
  enchant_dict_store_replacement(dict, a.c_str(), a.size(), b.c_str(), b.size());
}

void Spell::to_session(std::string& a) {
  assert(dict != NULL);

  enchant_dict_add_to_session(dict, a.c_str(), a.size());
}

void Spell::to_personal(std::string& s) {
  assert(dict != NULL);

  enchant_dict_add(dict, s.c_str(), s.size());
}

void _dict_describe_cb(const char * const lang_tag,
		       const char * const provider_name,
		       const char * const provider_desc,
		       const char * const provider_file,
		       void * user_data) {
  static_cast<std::vector<std::string> *>(user_data)->push_back(lang_tag);
}

void katoob_spell_list_available(std::vector<std::string>& dicts) {
  EnchantBroker *broker = enchant_broker_init();
  enchant_broker_list_dicts(broker, _dict_describe_cb, &dicts);
  enchant_broker_free(broker);
}

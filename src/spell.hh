/*
 * spell.hh
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

#ifndef __SPELL_HH__
#define __SPELL_HH__

#include <enchant-2/enchant.h>
#include <string>
#include <vector>

class Spell {
public:
  Spell();
  ~Spell();
  void suggest(std::string&, std::vector<std::string>&);
  bool check(std::string&);
  bool ok(std::string&);
  bool set_lang(std::string&, std::string&);
  void replace(std::string&, std::string&);
  void to_personal(std::string&);
  void to_session(std::string&);

private:
  // TODO: Can we only use one ?
  EnchantBroker *broker;
  EnchantDict *dict;
};

void katoob_spell_list_available(std::vector<std::string>&);

#endif /* __SPELL_HH__ */

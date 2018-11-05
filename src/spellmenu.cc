/*
 * spellmenu.cc
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

#include "spellmenu.hh"
#include "spell.hh"
#include <cassert>

SpellMenu::SpellMenu() {
  katoob_spell_list_available(dicts);
  for (unsigned x = 0; x < dicts.size(); x++) {
    append_text(iso_codes.translate(dicts[x]));
  }
}

const Glib::ustring SpellMenu::get_active_text() {
  int x = get_active_row_number();
  assert(x != -1);
  return dicts[x];
}

void SpellMenu::set_active_text(const Glib::ustring& str) {
  return Gtk::ComboBoxText::set_active_text(iso_codes.translate(str));
}

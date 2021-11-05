/*
 * spellmenu.cc
 *
 * This file is part of Katoob.
 *
 * Copyright (C) 2008-2021 Fred Morcos <fm+Katoob@fredmorcos.com>
 * Copyright (C) 2002-2007 Mohammed Sameer <msameer@foolab.org>
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

#include "spell.hh"
#include "spellmenu.hh"

#ifdef ENABLE_ISOCODES
#include <cassert>
#endif

SpellMenu::SpellMenu()
{
  katoob_spell_list_available(dicts);
  for (unsigned x = 0; x < dicts.size(); x++) {
#ifdef ENABLE_ISOCODES
    append_text(iso_codes.translate(dicts[x]));
#else
    append_text(dicts[x]);
#endif
  }
}

const Glib::ustring SpellMenu::get_active_text()
{
#ifdef ENABLE_ISOCODES
  int x = get_active_row_number();
  assert(x != -1);
  return dicts[x];
#else
  return Gtk::ComboBoxText::get_active_text();
#endif
}

void SpellMenu::set_active_text(const Glib::ustring &str)
{
#ifdef ENABLE_ISOCODES
  return Gtk::ComboBoxText::set_active_text(iso_codes.translate(str));
#else
  return Gtk::ComboBoxText::set_active_text(str);
#endif
}

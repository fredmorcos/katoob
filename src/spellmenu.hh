/*
 * spellmenu.hh
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

#ifndef __SPELLMENU_HH__
#define __SPELLMENU_HH__

#include <gtkmm/comboboxtext.h>
#ifdef ENABLE_ISOCODES
#include "isocodes.hh"
#endif

class SpellMenu : public Gtk::ComboBoxText {
public:
  SpellMenu();
  const Glib::ustring get_active_text();
  void set_active_text(const Glib::ustring&);
  //  void set_sensitive(bool);
  //  void append_text(const Glib::ustring&);
private:
  std::vector<std::string> dicts;
#ifdef ENABLE_ISOCODES
  IsoCodes iso_codes;
#endif
};

#endif /* __SPELLMENU_HH__ */

/*
 * printsettings.cc
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

#include "printsettings.hh"
#include <iostream>

Glib::RefPtr<PrintSettings> PrintSettings::create(Conf& conf) {
  return Glib::RefPtr<PrintSettings>(new PrintSettings(conf));
}

PrintSettings::PrintSettings(Conf& conf): _conf(conf) {
  reset();
}

void PrintSettings::reset() {
  // We will load the keys.
  sigc::slot<void, const Glib::ustring&, const Glib::ustring&> slot = sigc::mem_fun(*this, &PrintSettings::foreach_load);
  setting_foreach(slot);
}

void PrintSettings::foreach_load(const Glib::ustring& key, const Glib::ustring& value) {
  // TODO:
  std::cout << "HERE" << std::endl;
  std::cout << "key: " << key << " value: " << value << std::endl;
}

void PrintSettings::foreach_save(const Glib::ustring& key, const Glib::ustring& value) {
  // TODO:
  std::cout << "HERE" << std::endl;
  std::cout << "key: " << key << " value: " << value << std::endl;
}

void PrintSettings::save() {
  sigc::slot<void, const Glib::ustring&, const Glib::ustring&> slot = sigc::mem_fun(*this, &PrintSettings::foreach_save);
  setting_foreach(slot);
}

PrintSettings::~PrintSettings() {
}

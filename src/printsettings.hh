/*
 * printsettings.hh
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

#ifndef __PRINTSETTINGS_HH__
#define __PRINTSETTINGS_HH__

#include <gtkmm/printsettings.h>
#include "conf.hh"

class PrintSettings : public Gtk::PrintSettings {
public:
  static Glib::RefPtr<PrintSettings> create(Conf&);
  PrintSettings(Conf&);
  ~PrintSettings();
  void reset();
  void save();

private:
  void foreach_load(const Glib::ustring&, const Glib::ustring&);
  void foreach_save(const Glib::ustring&, const Glib::ustring&);
  Conf& _conf;
};

#endif /* __PRINTSETTINGS_HH__ */

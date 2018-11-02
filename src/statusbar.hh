/*
 * statusbar.hh
 * This file is part of katoob
 *
 * Copyright (C) 2006, 2007, 2008 Mohammed Sameer
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

#ifndef __STATUSBAR_HH__
#define __STATUSBAR_HH__

#include <gtkmm/box.h>
#include <gtkmm/statusbar.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>
#include "conf.hh"

class Statusbar : public Gtk::HBox {
public:
  Statusbar(Conf&);
  ~Statusbar();
  void set_encoding(std::string);
  void set_overwrite(bool);
  void set_position(int, int);
  void set_modified(bool);
  void reset_gui();
  void show(bool);

private:
  Conf& _conf;

  Gtk::Statusbar sbar;
  Gtk::Image red, green;
  Gtk::Label enc, tips, overwrite;
};

#endif /* __STATUSBAR_HH__ */
/*
 * label.hh
 * This file is part of katoob
 *
 * Copyright (C) 2006 Mohammed Sameer
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

#ifndef __LABEL_HH__
#define __LABEL_HH__

#include <string>
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/button.h>
#include "conf.hh"

class Label : public Gtk::HBox {
public:
  Label(Conf&);
  ~Label();
  void set_text(int);
  void set_text(const std::string&);
  std::string get_text();
  void set_readonly(bool, bool = false);
  void set_modified(bool, bool = false);
  void set_normal();

  void reset_gui();

  sigc::signal<void> signal_close_clicked;

private:
  Gtk::Label label;
  Conf& _conf;
  Gtk::Button close;
  bool _readonly, _modified;
};

#endif /* __LABEL_HH__ */

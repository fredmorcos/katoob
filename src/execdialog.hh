/*
 * execdialog.hh
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

#ifndef __EXECDIALOG_HH__
#define __EXECDIALOG_HH__

#include <gtkmm/dialog.h>
#include <gtkmm/label.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/comboboxentrytext.h>
#include "conf.hh"

class ExecDialog : public Gtk::Dialog {
public:
  ExecDialog(Conf&);
  ~ExecDialog();
  bool run();
  std::string get_command();
  bool get_new_buffer();
private:
  Gtk::Label label, tips;
  Gtk::CheckButton new_buffer;
  Gtk::ComboBoxEntryText command;
  Gtk::HBox box;
};

#endif /* __EXECDIALOG_HH__ */

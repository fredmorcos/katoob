/*
 * execdialog.cc
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include "execdialog.hh"
#include <gtkmm/stock.h>
#include "macros.h"

ExecDialog::ExecDialog(Conf& conf) {
  set_modal();
  Gtk::VBox *vbox = Gtk::Dialog::get_vbox();
  vbox->pack_start(box);
  vbox->pack_start(tips);
  vbox->pack_start(new_buffer);
  box.pack_start(label);
  box.pack_start(command);

  label.set_text(_("Command to execute"));
  tips.set_text(_("'%s' will be substituted with a temporary filename\ncontaining the text from the document."));
  box.set_border_width(10);

  set_border_width(10);

  new_buffer.set_use_underline();
  new_buffer.set_label(_("_Show the results in a new tab"));

  add_button(Gtk::Stock::CLOSE, Gtk::RESPONSE_CLOSE);
  add_button(Gtk::Stock::OK, Gtk::RESPONSE_ACCEPT);

  std::vector<std::string> cmds = conf.get_list("exec-cmd");
  for (unsigned x = 0; x < cmds.size(); x++) {
    command.append_text(cmds[x]);
  }
  if (cmds.size() > 0) {
    command.set_active_text(cmds[0]);
  }

  new_buffer.set_active(conf.get("exec_cmd_in_new", true));
}

ExecDialog::~ExecDialog() {

}

bool ExecDialog::run() {
  show_all();
  if (Gtk::Dialog::run() == Gtk::RESPONSE_ACCEPT) {
    Gtk::Entry *entry = command.get_entry();
    if (entry->get_text().size() == 0) {
      return false;
    }
    else {
      return true;
    }
  }
  return false;
}

std::string ExecDialog::get_command() {
  return command.get_entry()->get_text();
}

bool ExecDialog::get_new_buffer() {
  return new_buffer.get_active();
}

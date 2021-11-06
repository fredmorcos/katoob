/*
 * filedialog.cc
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

#include "dialogs.hh"
#include "filedialog.hh"
#include "macros.h"
#include <gtkmm.h>

FileDialog::FileDialog(const std::string &title,
                       Gtk::FileChooserAction action,
                       int enc,
                       Conf &_conf,
                       Encodings &_enc):
 SimpleFileDialog(title, action, _conf),
 _encodings(_enc)
{
  if (action == FILE_OPEN) {
    set_select_multiple(true);
  }

  label.set_text(_("Encoding"));
  box.pack_start(label);
  label.set_padding(20, 0);

  for (int x = 0; x < _enc.size(); x++) {
    if (((action == FILE_OPEN) && x != _enc.utf8()) || (action == FILE_SAVE)) {
      cbox.append_text(_enc.at(x));
    }
  }

  box.pack_start(cbox);

  set_extra_widget(box);
  box.show_all();

  if ((action == FILE_OPEN) && (enc == -1)) {
    enc = _enc.default_open();
  } else if ((action == FILE_SAVE) && (enc == -1)) {
    enc = _enc.default_save();
  }

  if (action == FILE_OPEN) {
    set_current_folder(_conf.open_dir());
  } else {
    set_current_folder(_conf.save_dir());
  }

  cbox.set_active(enc);
}

FileDialog::~FileDialog()
{
}

int FileDialog::encoding()
{
  //  return cbox.get_active();
  std::string str = cbox.get_active_text();
  return _encodings.get(str);
}

bool FileDialog::run()
{
  bool st = Gtk::FileChooserDialog::run() == Gtk::RESPONSE_OK;

  hide();
  return st;
}

std::vector<std::string> FileDialog::get()
{
  return get_filenames();
}

SimpleFileDialog::SimpleFileDialog(const std::string &title,
                                   Gtk::FileChooserAction action,
                                   Conf &_conf):
 Gtk::FileChooserDialog(title, action)
{
  // TODO: We want to handle non-local only oneday.
  set_local_only(true);

  if (action == FILE_OPEN) {
    set_current_folder(_conf.open_dir());
  } else {
    set_current_folder(_conf.save_dir());
  }

  add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  if (action == FILE_OPEN) {
    add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);
  } else {
    add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);
  }

  show_all();
}

SimpleFileDialog::~SimpleFileDialog()
{
}

bool SimpleFileDialog::run()
{
  bool st = Gtk::FileChooserDialog::run() == Gtk::RESPONSE_OK;

  hide();

  if (st) {
    std::string f = get();
    if (Glib::file_test(f, Glib::FILE_TEST_IS_DIR)) {
      katoob_error(_("This is a directory."));
      return false;
    }
    // TODO: More checks.
  }

  return st;
}

std::string SimpleFileDialog::get()
{
  return get_filename();
}

std::string
SimpleFileDialog::get_file(const std::string &title, Gtk::FileChooserAction action, Conf &_conf)
{
  std::string file;
  SimpleFileDialog dialog(title, action, _conf);
  if (dialog.run()) {
    return dialog.get();
  } else {
    return file;
  }
}

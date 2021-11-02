/*
 * filedialog.hh
 * This file is part of katoob
 *
 * Copyright (C) 2002-2007 Mohammed Sameer
 * Copyright (C) 2008-2021 Frederic-Gerald Morcos
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

#pragma once

#include "conf.hh"
#include "encodings.hh"
#include <gtkmm.h>

#define FILE_OPEN Gtk::FILE_CHOOSER_ACTION_OPEN
#define FILE_SAVE Gtk::FILE_CHOOSER_ACTION_SAVE

class SimpleFileDialog : protected Gtk::FileChooserDialog {
protected:
  SimpleFileDialog(const std::string &, Gtk::FileChooserAction, Conf &);
  ~SimpleFileDialog();
  bool run();
  std::string get();

public:
  static std::string get_file(const std::string &, Gtk::FileChooserAction,
                              Conf &);
};

class FileDialog : private SimpleFileDialog {
public:
  FileDialog(const std::string &, Gtk::FileChooserAction, int, Conf &,
             Encodings &);
  ~FileDialog();
  int encoding();
  bool run();
  std::vector<std::string> get();

private:
  Gtk::Label label;
  Gtk::HBox box;
  Gtk::ComboBoxText cbox;
  Encodings &_encodings;
  //  std::string file;
};

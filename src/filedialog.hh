/*
 * filedialog.hh
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

#pragma once

#include "conf.hh"
#include "encodings.hh"
#include <gtkmm.h>

#ifdef ENABLE_MAEMO
#include <hildon-fmmm/file-chooser-dialog.h>
#include <hildonmm/caption.h>
#endif

#define FILE_OPEN Gtk::FILE_CHOOSER_ACTION_OPEN
#define FILE_SAVE Gtk::FILE_CHOOSER_ACTION_SAVE

#ifdef ENABLE_MAEMO
class SimpleFileDialog: protected Hildon::FileChooserDialog {
#else
class SimpleFileDialog: protected Gtk::FileChooserDialog {
#endif

 protected:
  SimpleFileDialog(const std::string &, Gtk::FileChooserAction, Conf &);
  ~SimpleFileDialog();
  bool run();
  std::string get();

 public:
  static std::string get_file(const std::string &, Gtk::FileChooserAction, Conf &);
};

class FileDialog: private SimpleFileDialog {
 public:
  FileDialog(const std::string &, Gtk::FileChooserAction, int, Conf &, Encodings &);
  ~FileDialog();
  int encoding();
  bool run();
  std::vector<std::string> get();

 private:
#ifdef ENABLE_MAEMO
  // We are using a caption for maemo otherwise the dialog looks like shit.
  // I also get a crash if I don't use a pointer. No idea why.
  Hildon::Caption *caption;
#else
  Gtk::Label label;
  Gtk::HBox box;
#endif
  Gtk::ComboBoxText cbox;
  Encodings &_encodings;
  //  std::string file;
};

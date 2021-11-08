/*
 * print.hh
 *
 * This file is part of Katoob.
 *
 * Copyright © 2008-2021 Fred Morcos <fm+Katoob@fredmorcos.com>
 * Copyright © 2002-2007 Mohammed Sameer <msameer@foolab.org>
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

#include "applets.hh"
#include "conf.hh"
#include "document.hh"
#include "pagesetup.hh"
#include "previewdialog.hh"
#include "printsettings.hh"
#include <gtkmm.h>

class Print: public Gtk::PrintOperation {
 public:
  static Glib::RefPtr<Print>
  create(Conf &, Document *, Glib::RefPtr<PageSetup> &, Glib::RefPtr<PrintSettings> &);
  virtual ~Print();
  bool run(std::string &, Gtk::PrintOperationAction);

 protected:
  Print(Conf &, Document *, Glib::RefPtr<PageSetup> &, Glib::RefPtr<PrintSettings> &);

  // Printing part.
  void on_begin_print(const Glib::RefPtr<Gtk::PrintContext> &context);
  void on_draw_page(const Glib::RefPtr<Gtk::PrintContext> &, std::size_t nr);

  // Custom widget.
  Gtk::Widget *on_create_custom_widget();
  void on_custom_widget_apply(Gtk::Widget *);

  // Preview part.
  bool on_preview(const Glib::RefPtr<Gtk::PrintOperationPreview> &,
                  const Glib::RefPtr<Gtk::PrintContext> &,
                  Gtk::Window *);
  void on_preview_window_hide();

  void on_done(Gtk::PrintOperationResult);

 private:
  int get_n_pages();
  Glib::RefPtr<Pango::Layout> get_layout();

  Conf &_conf;
  Glib::RefPtr<Pango::Layout> layout;
  Document *_doc;

#ifdef ENABLE_PRINT
  PrintApplet applet;
#endif

  PreviewDialog *_preview;
  Glib::RefPtr<PageSetup> &_page_setup;
  Glib::RefPtr<PrintSettings> &_settings;
  std::vector<std::vector<int> > pages;
};

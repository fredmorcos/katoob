/*
 * previewdialog.hh
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

#pragma once

#include <gtkmm.h>

class PreviewDialog : public Gtk::Dialog {
public:
  static PreviewDialog *create(const Glib::RefPtr<Gtk::PrintOperationPreview> &,
                               const Glib::RefPtr<Gtk::PrintContext> &,
                               Gtk::Window *);
  void run();
  sigc::signal<int> signal_get_n_pages;
  sigc::signal<Glib::RefPtr<Pango::Layout>> signal_get_layout;

private:
  PreviewDialog(const Glib::RefPtr<Gtk::PrintOperationPreview> &,
                const Glib::RefPtr<Gtk::PrintContext> &, Gtk::Window *);
  //  ~PreviewDialog();

  void recalculate_gui();

  void on_back_clicked();
  void on_rewind_clicked();
  void on_forward_clicked();
  void on_ff_clicked();
  void on_signal_value_changed();

  void on_hide();

  void signal_area_realize_cb();
  bool signal_area_expose_event_cb(GdkEventExpose *);

  void signal_preview_ready_cb(const Glib::RefPtr<Gtk::PrintContext> &);
  void signal_preview_got_page_size_cb(const Glib::RefPtr<Gtk::PrintContext> &,
                                       const Glib::RefPtr<Gtk::PageSetup> &);

  Glib::RefPtr<Gtk::PrintOperationPreview> preview;
  Glib::RefPtr<Gtk::PrintContext> context;

  int n_pages;

  Gtk::HBox hbox;

  Gtk::Adjustment adj;
  Gtk::SpinButton pages;

  Gtk::Label label;

  Gtk::DrawingArea area;

  Gtk::Button back;
  Gtk::Button forward;
  Gtk::Button rewind;
  Gtk::Button ff;
  Gtk::ScrolledWindow sw;
};

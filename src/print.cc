/*
 * print.cc
 * This file is part of katoob
 *
 * Copyright (C) 2006, 2007 Mohammed Sameer
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

#include "print.hh"
#include "macros.h"

Print::Print(Conf& conf, Document *doc, Glib::RefPtr<PageSetup>& page_setup, Glib::RefPtr<PrintSettings>& settings) :
  _conf(conf),
  _doc(doc),
  applet(conf),
  _page_setup(page_setup),
  _settings(settings) {

  set_allow_async(false);
  set_show_progress(true);

  //  std::string path = Utils::prepend_home_dir("katoob.pdf");
  //  set_export_filename(_conf.print_get("pdf_entry", path));
  //  std::cout << _conf.print_get("pdf_entry", path) << std::endl;
  set_print_settings(_settings);
  set_default_page_setup(_page_setup->get_page_setup());
#ifndef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
  signal_begin_print().connect(sigc::mem_fun(this, &Print::on_begin_print));
  signal_draw_page().connect(sigc::mem_fun(this, &Print::on_draw_page));
  signal_preview().connect(sigc::mem_fun(this, &Print::on_preview), false);
  signal_create_custom_widget().connect(sigc::mem_fun(this, &Print::on_create_custom_widget));
  signal_custom_widget_apply().connect(sigc::mem_fun(this, &Print::on_custom_widget_apply));
  signal_done().connect(sigc::mem_fun(this, &Print::on_done));
#endif
}

Print::~Print() {
}

Glib::RefPtr<Print> Print::create(Conf& conf, Document *doc, Glib::RefPtr<PageSetup>& page_setup, Glib::RefPtr<PrintSettings>& settings) {
  return Glib::RefPtr<Print>(new Print(conf, doc, page_setup, settings));
}

bool Print::run(std::string& error, Gtk::PrintOperationAction print_action) {
  Gtk::PrintOperationResult res;
#ifdef GLIBMM_EXCEPTIONS_ENABLED
  try {
    res = Gtk::PrintOperation::run(print_action);
  }
  catch (Glib::Error& err) {
    error = err.what();
    return false;
  }

#else
  std::auto_ptr<Glib::Error> e;
  res = Gtk::PrintOperation::run(print_action, e);
  if (e.get()) {
    error = e->what();
    return false;
  }
#endif

  switch (res) {
  case Gtk::PRINT_OPERATION_RESULT_ERROR:
    // We should not reach this.
    return false;

  case Gtk::PRINT_OPERATION_RESULT_APPLY:
    return true;

  case Gtk::PRINT_OPERATION_RESULT_CANCEL:
  case Gtk::PRINT_OPERATION_RESULT_IN_PROGRESS:
  default:
    // Should never get the last two!
    return false;
  }
}

void Print::on_begin_print(const Glib::RefPtr<Gtk::PrintContext>& context) {
  // You must handle this signal, because this is where you create and set up
  // a Pango::Layout using the provided Gtk::PrintContext, and break up your
  // printing output into pages.

  layout = context->create_pango_layout();
  Pango::FontDescription font = Pango::FontDescription(_conf.print_get("print_font", "Sans Regular 12"));
  layout->set_font_description(font);

  const double width = context->get_width();
  const double height = context->get_height();
  layout->set_width(static_cast<int>(width * Pango::SCALE));

  // Now let's set the number of pages.
  layout->set_text(_doc->get_text());
  const int line_count = layout->get_line_count();
  Glib::RefPtr<Pango::LayoutLine> layout_line;
  double page_height = 0;

  std::vector<int> _nr;

  for (int line = 0; line < line_count; line++) {
    Pango::Rectangle ink_rect, logical_rect;
    layout_line = layout->get_line(line);
    layout_line->get_extents(ink_rect, logical_rect);
    const double line_height = logical_rect.get_height() / 1024.0;
    if (page_height + line_height > height) {
      pages.push_back(_nr);
      _nr.clear();
      _nr.push_back(line);
      page_height = 0;
    }
    else if (page_height + line_height == height) {
      _nr.push_back(line);
      pages.push_back(_nr);
      _nr.clear();
      page_height += line_height;
    }
    else {
      _nr.push_back(line);
      page_height += line_height;
    }
  }

  if (_nr.size() > 0) {
    pages.push_back(_nr);
  }

  set_n_pages(pages.size());
}

void Print::on_draw_page(const Glib::RefPtr<Gtk::PrintContext>& context, int nr) {
  // You must handle this signal, which provides a PrintContext and a page number.
  // The PrintContext should be used to create a Cairo::Context into which the provided
  // page should be drawn. To render text, iterate over the Pango::Layout you created
  // in the begin_print handler.

  if ((!context) || (!layout)) {
    return;
  }

  if (nr > pages.size()) {
    // This shouldn't happen but just in case.
    return;
  }

  std::vector<int>& lines = pages[nr];
  // TODO: Is this correct ??
  if (lines.size() == 0) {
    return;
  }

  Cairo::RefPtr<Cairo::Context> ctx = context->get_cairo_context();

  // TODO: configurable.
  ctx->set_source_rgb(0, 0, 0);

  Glib::RefPtr<Pango::LayoutLine> layout_line;
  int y = 0;
  for (unsigned x = 0; x < lines.size(); x++) {
    int i = lines[x];
    layout_line = layout->get_line(i);
    Glib::RefPtr<Pango::LayoutLine> layout_line = layout->get_line(i);
    Pango::Rectangle logical_rect = layout_line->get_logical_extents();

    y += logical_rect.get_height();
    ctx->move_to(0, y / 1024.0);

    layout_line->show_in_cairo_context(ctx);
  }
}

bool Print::on_preview(const Glib::RefPtr<Gtk::PrintOperationPreview>& preview, const Glib::RefPtr<Gtk::PrintContext>& context, Gtk::Window *parent) {
  _preview = PreviewDialog::create(preview, context, parent);
  _preview->signal_get_n_pages.connect(sigc::mem_fun(this, &Print::get_n_pages));
  _preview->signal_get_layout.connect(sigc::mem_fun(this, &Print::get_layout));
  _preview->signal_hide().connect(sigc::mem_fun(this, &Print::on_preview_window_hide));
  _preview->run();
  return true;
}

void Print::on_preview_window_hide() {
  if (_preview) {
    delete _preview;
    _preview = NULL;
  }
}

Gtk::Widget* Print::on_create_custom_widget() {
  set_custom_tab_label(_("Other"));
  applet.get_box().show_all();
  return &applet.get_box();
}

void Print::on_custom_widget_apply(Gtk::Widget *) {
  // Note: the returned widget is the VBox we created in on_create_custom_widget().
  // We don't need to use it, because we can access the applet directly.
  applet.apply();
  _settings->reset();
}

void Print::on_done(Gtk::PrintOperationResult result) {
  // TODO:
  //  _preview.clear();
}

int Print::get_n_pages() {
  return pages.size();
}

Glib::RefPtr<Pango::Layout> Print::get_layout() {
  return layout;
}

/*
 * previewdialog.cc
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

#include <gtkmm/stock.h>
#include "previewdialog.hh"
#include "macros.h"
#include "utils.hh"

PreviewDialog::PreviewDialog(const Glib::RefPtr<Gtk::PrintOperationPreview>& prvw, const Glib::RefPtr<Gtk::PrintContext>& ctx, Gtk::Window* parent) :
  preview(prvw),
  context(ctx),
  n_pages(0),
  adj(1, 1, 1),
  pages(adj),
  label(""),
  back(Gtk::Stock::GO_BACK),
  forward(Gtk::Stock::GO_FORWARD),
  rewind(Gtk::Stock::GOTO_FIRST),
  ff(Gtk::Stock::GOTO_LAST) {

  if (parent) {
    set_transient_for(*parent);
  }

  set_title(_("Print preview"));
  Gtk::VBox *vbox = get_vbox();

  vbox->pack_start(hbox, false, false);
  sw.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  sw.add(area);
  vbox->pack_start(sw, true, true);

  label.set_text(Utils::substitute(_("of %i"), 1));

  rewind.set_relief(Gtk::ReliefStyle(Gtk::RELIEF_NONE));
  back.set_relief(Gtk::ReliefStyle(Gtk::RELIEF_NONE));
  forward.set_relief(Gtk::ReliefStyle(Gtk::RELIEF_NONE));
  ff.set_relief(Gtk::ReliefStyle(Gtk::RELIEF_NONE));

  hbox.pack_start(rewind, false, false, 5);
  hbox.pack_start(back, false, false, 5);
  hbox.pack_start(forward, false, false, 5);
  hbox.pack_start(ff, false, false, 5);

  hbox.pack_start(pages, false, false);
  hbox.pack_start(label, false, false, 20);

  Glib::RefPtr<Gdk::Screen> sc = Gdk::Screen::get_default();

  set_size_request(sc->get_width()/2, sc->get_height()/2);
  area.set_double_buffered(false);

  pages.signal_value_changed().connect(sigc::mem_fun(*this, &PreviewDialog::on_signal_value_changed));

  back.signal_clicked().connect(sigc::mem_fun(*this, &PreviewDialog::on_back_clicked));
  forward.signal_clicked().connect(sigc::mem_fun(*this, &PreviewDialog::on_forward_clicked));
  ff.signal_clicked().connect(sigc::mem_fun(*this, &PreviewDialog::on_ff_clicked));
  rewind.signal_clicked().connect(sigc::mem_fun(*this, &PreviewDialog::on_rewind_clicked));

  area.signal_realize().connect(sigc::mem_fun(*this, &PreviewDialog::signal_area_realize_cb));
  area.signal_expose_event().connect(sigc::mem_fun(*this, &PreviewDialog::signal_area_expose_event_cb));

  preview->signal_ready().connect(sigc::mem_fun(*this, &PreviewDialog::signal_preview_ready_cb));
  preview->signal_got_page_size().connect(sigc::mem_fun(*this, &PreviewDialog::signal_preview_got_page_size_cb));
  add_button(Gtk::Stock::CLOSE, Gtk::RESPONSE_CLOSE)->signal_clicked().connect(sigc::mem_fun(this, &PreviewDialog::hide));
  recalculate_gui();
#ifndef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
  signal_hide().connect(sigc::mem_fun(this, &PreviewDialog::on_hide));
#endif
}

/*
PreviewDialog::~PreviewDialog() {

}
*/

void PreviewDialog::on_hide() {
  preview->end_preview();
  preview.clear();
  context.clear();
#ifdef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
  Gtk::Window::on_hide();
#endif
}

void PreviewDialog::run() {
  show_all();
}

void PreviewDialog::on_signal_value_changed() {
  area.queue_draw();
  recalculate_gui();
}

void PreviewDialog::on_back_clicked() {
  pages.set_value(pages.get_value_as_int()-1);
}

void PreviewDialog::on_rewind_clicked() {
  pages.set_value(1);
}

void PreviewDialog::on_forward_clicked() {
  pages.set_value(pages.get_value_as_int()+1);
}

void PreviewDialog::on_ff_clicked() {
  double s, e;
  pages.get_range(s, e);
  pages.set_value(e);
}

void PreviewDialog::recalculate_gui() {
  int c = pages.get_value_as_int();
  back.set_sensitive(!(c == 1));
  rewind.set_sensitive(!(c == 1));
  forward.set_sensitive(!(c == n_pages));
  ff.set_sensitive(!(c == n_pages));
}

PreviewDialog *PreviewDialog::create(const Glib::RefPtr<Gtk::PrintOperationPreview>& preview,
                                     const Glib::RefPtr<Gtk::PrintContext>& context, Gtk::Window* parent)
{
  return new PreviewDialog(preview, context, parent);
}

void PreviewDialog::signal_area_realize_cb() {
  Glib::RefPtr<Gdk::Window> win = area.get_window();
  if (win) {
    Cairo::RefPtr<Cairo::Context> ctx = win->create_cairo_context();
    if (ctx) {
      if (context) {
	// We will change the DPI later anyway.
	context->set_cairo_context(ctx, 72, 72);
      }
    }
  }
}

bool PreviewDialog::signal_area_expose_event_cb(GdkEventExpose *KATOOB_UNUSED(event)) {
  if (n_pages == 0) {
    return false;
  }
  area.get_window()->clear();
  if (preview) {
    preview->render_page(pages.get_value_as_int()-1);
  }
  return true;
}

void PreviewDialog::signal_preview_ready_cb(const Glib::RefPtr<Gtk::PrintContext>& KATOOB_UNUSED(ctx)) {
  n_pages =  signal_get_n_pages.emit();
  pages.set_range(1, n_pages);
  label.set_text(Utils::substitute(_("of %i"), n_pages));
  area.queue_draw();
}

void PreviewDialog::signal_preview_got_page_size_cb(const Glib::RefPtr<Gtk::PrintContext>& ctx, const Glib::RefPtr<Gtk::PageSetup>& setup) {
  Gtk::PaperSize paper_size = setup->get_paper_size();

  area.set_size_request((int)ceil(paper_size.get_width(Gtk::UNIT_POINTS)), (int)ceil(paper_size.get_height(Gtk::UNIT_POINTS)));

  // Avoid getting an odd allocation.
  if(area.is_realized()) {
    Cairo::RefPtr<Cairo::Context> cairo_ctx = area.get_window()->create_cairo_context();
      context->set_cairo_context(cairo_ctx, ctx->get_dpi_x(), ctx->get_dpi_y());
      Glib::RefPtr<Pango::Layout> layout = signal_get_layout.emit();
      if(layout) {
	layout->update_from_cairo_context(cairo_ctx);
      }
  }
}

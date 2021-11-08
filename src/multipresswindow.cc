/*
 * multipresswindow.cc
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

#include <config.h>

#include "multipresswindow.hh"
#include <cassert>
#include <gdk/gdkkeysyms.h>

MultipressWindow::MultipressWindow(): Gtk::Window(), loop(Glib::MainLoop::create())
{
  set_position(Gtk::WIN_POS_CENTER);
  set_decorated(false);
  set_modal(true);
#ifndef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
  signal_key_press_event().connect(sigc::mem_fun(this, &MultipressWindow::on_key_press_event),
                                   false);
  signal_expose_event().connect(sigc::mem_fun(this, &MultipressWindow::on_expose_event), false);
#endif
}

MultipressWindow::~MultipressWindow()
{
}

void MultipressWindow::set_values(const std::vector<std::string> &values)
{
  assert(values.size() > 0);
  _values = values;
}

void MultipressWindow::set_timeout(int timeout)
{
  assert(timeout > 0);
  _timeout = timeout;
}

void MultipressWindow::get()
{
  assert(_timeout > 0);
  assert(_values.size() > 0);

  clear(true);

  show_all();
  grab_focus();

  // Add the timer.
  timeout_conn =
      Glib::signal_timeout().connect(sigc::mem_fun(*this, &MultipressWindow::timeout_cb), _timeout);
  // enter the main loop.
  loop->run();

  hide();
  timeout_conn.disconnect();
  return;
}

void MultipressWindow::clear(bool reset)
{
  Glib::RefPtr<Gdk::Window> w = get_window();
  if (w) {
    w->clear();
  }
  if (reset) {
    _pos = 0;
    _to_draw = _values[0];
  }
}

void MultipressWindow::set_key(const std::string &key)
{
  _key = key;
}

bool MultipressWindow::timeout_cb()
{
  // If we reach this, then the user needs this key.
  loop->quit();
  signal_insert_key(_to_draw);
  return false;
}

bool MultipressWindow::on_key_press_event(GdkEventKey *event)
{
  if (event->length != 1) {
    signal_invalid_key.emit(event);
    loop->quit();
#ifdef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
    return Window::on_key_press_event(event);
#else
    return false;
#endif
  }

  gunichar ch = gdk_keyval_to_unicode(event->keyval);
  _tmp_key = std::string(1, ch);
  if (!ch) {
    signal_invalid_key.emit(event);
    loop->quit();
#ifdef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
    return Window::on_key_press_event(event);
#else
    return false;
#endif
  }

  if (ch == GDK_Return) {
    // force an accept.
    loop->quit();
    signal_insert_key(_to_draw);
#ifdef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
    return Window::on_key_press_event(event);
#else
    return false;
#endif
  } else if (ch == GDK_Escape) {
    // force a reject.
    loop->quit();
    // TODO: Bad.
    Glib::ustring _str(1, ch);
    std::string str(_str);

    signal_insert_key.emit(str);
#ifdef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
    return Window::on_key_press_event(event);
#else
    return false;
#endif
  }

  if (_tmp_key != _key) {
    // get the old key and insert it.
    signal_insert_key.emit(_to_draw);

    // handle the new one.
    _key = _tmp_key;

    if (signal_change_key.emit(_tmp_key)) {
      clear(true);
      show_next();
    } else {
      signal_invalid_key.emit(event);
      loop->quit();
#ifdef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
      return Window::on_key_press_event(event);
#else
      return false;
#endif
    }
  } else {
    // show the next value.
    show_next();
  }

  // reset the timer.
  timeout_conn.disconnect();
  timeout_conn =
      Glib::signal_timeout().connect(sigc::mem_fun(*this, &MultipressWindow::timeout_cb), _timeout);
#ifdef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
  return Window::on_key_press_event(event);
#else
  return true;
#endif
}

void MultipressWindow::show_next()
{
  if (_pos + 1 < _values.size()) {
    _pos++;
  } else {
    _pos = 0;
  }
  _to_draw = _values[_pos];

  // force a redraw.
  clear(false);
  // NOTE: UGLY
  on_expose_event(NULL);
  // int x, y, w, h, depth;
  // get_window()->get_geometry(x, y, w, h, depth);
  // get_window()->invalidate_rect(Gdk::Rectangle(x, y, w, h), false);
}

bool MultipressWindow::on_expose_event(GdkEventExpose *event)
{
  Glib::RefPtr<Gdk::GC> gc = Gdk::GC::create(get_window());
  Glib::RefPtr<Pango::Layout> layout = create_pango_layout(_to_draw);

  layout->set_markup(std::string("<span size=\"xx-large\">" + _to_draw + "</span>"));
  int x, y, w, h, depth, lw, lh;
  get_window()->get_geometry(x, y, w, h, depth);
  layout->get_pixel_size(lw, lh);
  get_window()->draw_layout(gc, ((w / 2) - (lw / 2)), ((h / 2) - (lh / 2)), layout);
  return true;
}

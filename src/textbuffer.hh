/*
 * textbuffer.hh
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
#include "sourcemanager.hh"
#include <gtkmm.h>
#include <gtksourceview/gtksource.h>

class TextBuffer : public Gtk::TextBuffer {
public:
  TextBuffer(Conf &);
  ~TextBuffer();
  static Glib::RefPtr<TextBuffer> create(Conf &conf) {
    return Glib::RefPtr<TextBuffer>(new TextBuffer(conf));
  }

  int get_mark_insert_position() { return _insert->get_iter().get_offset(); }
  int get_mark_insert_line() { return _insert->get_iter().get_line(); }
  int get_erase_line() { return _erase; }
  std::string &get_deleted() { return _deleted; }
  void clear_deleted() { _deleted.clear(); }

  void set_highlight(bool);
  void set_language(GtkSourceLanguage *);

protected:
  void on_insert(const Gtk::TextBuffer::iterator &, const Glib::ustring &, int);
  Glib::RefPtr<Mark> _insert;
  void on_erase(const Gtk::TextBuffer::iterator &,
                const Gtk::TextBuffer::iterator &);

private:
#ifndef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
  sigc::connection on_insert_conn;
#endif
  std::string _deleted;
  int _erase;
  Conf &_conf;
};

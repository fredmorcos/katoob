/*
 * textbuffer.cc
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

#include "textbuffer.hh"
#include "utils.hh"

#ifdef ENABLE_EMULATOR
#include "emulator.hh"
#endif

#ifdef ENABLE_MULTIPRESS
#include "multipress.hh"
#endif

TextBuffer::TextBuffer(Conf &conf):
#ifdef ENABLE_HIGHLIGHT
 Gtk::TextBuffer(GTK_TEXT_BUFFER(gtk_source_buffer_new(NULL)))
#endif
 ,
 _conf(conf)
{
  _insert = create_mark(begin());
  _erase = 0;
#ifdef ENABLE_HIGHLIGHT
  // We don't want gtksourceview to manage the undo for us. We are doing it anyway.
  gtk_source_buffer_set_max_undo_levels(GTK_SOURCE_BUFFER(gobj()), 0);
#endif
#ifndef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
  on_insert_conn = signal_insert().connect(sigc::mem_fun(this, &TextBuffer::on_insert), false);
  signal_erase().connect(sigc::mem_fun(this, &TextBuffer::on_erase), false);
#endif
}

TextBuffer::~TextBuffer()
{
  // Don't delete!
  //  delete_mark(_insert);
}

void TextBuffer::on_insert(const Gtk::TextBuffer::iterator &pos,
                           const Glib::ustring &text,
                           int bytes)
{
  move_mark(_insert, pos);
  Glib::ustring utf8;
  // Here we will handle the lam-alef inserts.
  // According to the xfree86 guys, It's not possible to map 1 key to 2 letter.
  // I'm not sure whether it's not possible or it's that they don't like it.
  // A workaround by Isam Bayazidi of Arabeyes was to map the key to the Unicode
  // isolated presentation form.
  // So what we save in the files is the presentation form not the lam+alef
  // characters.
  // There is no LAM-ALEF character in the unicode standard, As it's a
  // presentation form for the lam+alef characters togeather.
  // Though it works, But when converting the UTF-8 or unicode encoding to
  // cp1256 it breaks.
  // Here we'll try to catch the presentation form and insert lam followed by alef.
  // Look at: /usr/X11R6/lib/X11/xkb/symbols/ar
  // Or: /usr/share/X11/xkb/symbols/ara
  // Or: /etc/X11/xkb/symbols/ar
  // Finally: THIS SUCK A LOT.
  // Someone should contact the xorg guys and see.
  if (Utils::is_lam_alef(text, utf8)) {
#ifdef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
    Gtk::TextBuffer::on_insert(pos, utf8, utf8.bytes());
#else
    on_insert_conn.block();
    insert(pos, utf8);
    on_insert_conn.unblock();
    g_signal_stop_emission_by_name(gobj(), "insert-text");
#endif
  }
#if defined(ENABLE_EMULATOR) || defined(ENABLE_MULTIPRESS)
  else if (text.size() == 1) {
    std::string val;
    if (Emulator::get_active()) {
      if (Emulator::get(text, val)) {
        // insert.
#ifdef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
        Gtk::TextBuffer::on_insert(pos, val, val.size());
#else
        on_insert_conn.block();
        insert(pos, val);
        on_insert_conn.unblock();
        g_signal_stop_emission_by_name(gobj(), "insert-text");
#endif
      }
    } else if (Multipress::get_active()) {
      if (Multipress::get(text, _conf.get("multipress_timeout", 1000))) {
        // We do nothing here. Multipress will call us later.
        g_signal_stop_emission_by_name(gobj(), "insert-text");
      }
    } else {
#ifdef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
      Gtk::TextBuffer::on_insert(pos, text, bytes);
#endif
    }
  }
#endif
  else {
#ifdef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
    Gtk::TextBuffer::on_insert(pos, text, bytes);
#endif
  }
}

void TextBuffer::on_erase(const Gtk::TextBuffer::iterator &start,
                          const Gtk::TextBuffer::iterator &end)
{
  _erase = end.get_line();
  _deleted = get_text(start, end, false);
#ifdef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
  Gtk::TextBuffer::on_erase(start, end);
#endif
}

#ifdef ENABLE_HIGHLIGHT
void TextBuffer::set_highlight(bool s)
{
  gtk_source_buffer_set_highlight_syntax(GTK_SOURCE_BUFFER(gobj()), s ? TRUE : FALSE);
}

void TextBuffer::set_language(GtkSourceLanguage *lang)
{
  gtk_source_buffer_set_language(GTK_SOURCE_BUFFER(gobj()), lang);
}
#endif

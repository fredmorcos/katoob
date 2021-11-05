/*
 * document.hh
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
#include "label.hh"
#include "undoredo.hh"
#include <gtkmm.h>
#include <map>
#include <string>

#ifdef ENABLE_SPELL
#include "spell.hh"
#endif

#ifdef ENABLE_HIGHLIGHT
#include "sourceview.hh"
#else
#include "textview.hh"
#endif

class Document: public Gtk::ScrolledWindow {
#ifdef ENABLE_MAEMO
  friend void __tap_and_hold(GtkWidget *, gpointer);
#endif

 public:
  Document(Conf &, Encodings &, int);
  Document(Conf &, Encodings &, int, std::string &);
  Document(Conf &, Encodings &, int, int);
  ~Document();

  Label &get_label()
  {
    return _label;
  }

  bool ok()
  {
    return _ok;
  }

  bool has_file()
  {
    return _file.size() == 0 ? false : true;
  }

  bool has_selection();
  int get_line_count();

  void autosave();

  bool get_readonly()
  {
    return !_text_view.get_editable();
  };
  bool get_modified()
  {
    return _text_view.get_buffer()->get_modified();
  }

  bool save();
  bool save(std::string &, int, bool);

  int get_encoding()
  {
    return _encoding;
  }

  bool set_encoding(int, bool, std::string &);

#ifdef ENABLE_HIGHLIGHT
  void set_highlight(const std::string &);
#endif

  std::string get_title()
  {
    return _label.get_text();
  }

  void cut()
  {
    _text_view.get_buffer()->cut_clipboard(Gtk::Clipboard::get(), !_readonly);
  }
  void copy()
  {
    _text_view.get_buffer()->copy_clipboard(Gtk::Clipboard::get());
  }
  void paste()
  {
    _text_view.get_buffer()->paste_clipboard(Gtk::Clipboard::get(), !_readonly);
  }
  void erase()
  {
    _text_view.get_buffer()->erase_selection(true, !_readonly);
  }

  std::string &get_file()
  {
    return _file;
  }

  void scroll_to(int);

  void get_lines(std::vector<std::string> &, int = -1, int = -1);
  void get_lines(std::vector<Glib::ustring> &, int = -1, int = -1);
  void get_selection(std::vector<std::string> &);

  void select_all();

  void set_wrap_text(bool);
  void line_numbers(bool);

  void emit_signals();

  void insert(const std::string &);

  void grab_focus();
  bool has_focus();

  bool revert(std::string &);

  /* Our signal */
  sigc::signal<void, bool> signal_modified_set;
  sigc::signal<void, bool> signal_can_undo;
  sigc::signal<void, bool> signal_can_redo;
  sigc::signal<void, bool> signal_readonly_set;
  sigc::signal<void, std::string> signal_file_changed;
  sigc::signal<void, int, int> signal_cursor_moved;
  sigc::signal<void, int> signal_encoding_changed;
  sigc::signal<void, bool> signal_overwrite_toggled;
  sigc::signal<void, std::string> signal_title_changed;
  sigc::signal<void, bool> signal_wrap_text_set;
  sigc::signal<void, bool> signal_line_numbers_set;
  sigc::signal<void, std::string> signal_text_view_request_file_open;
#ifdef ENABLE_HIGHLIGHT
  sigc::signal<void, std::string> signal_highlight_set;
#endif /* ENABLE_HIGHLIGHT */
  sigc::signal<void, std::string> signal_dict_lookup_request;
#ifdef ENABLE_SPELL
  sigc::signal<void, bool> signal_auto_spell_set;
  sigc::signal<void, std::string> signal_dictionary_changed;
#endif

  void undo();
  void redo();

  // Our search interface.
  bool get_search_match_case()
  {
    return _search_match_case;
  }
  bool get_search_wrap()
  {
    return _search_wrap;
  }
  bool get_search_backwards()
  {
    return _search_backwards;
  }
  bool get_search_whole_word()
  {
    return _search_whole_word;
  }
  bool get_search_from_beginning()
  {
    return _search_from_beginning;
  }
  std::string &get_search_text()
  {
    return _search_text;
  }
  std::string &get_replace_text()
  {
    return _replace_text;
  }

  void set_search_match_case(bool st)
  {
    _search_match_case = st;
  }
  void set_search_wrap(bool st)
  {
    _search_wrap = st;
  }
  void set_search_backwards(bool st)
  {
    _search_backwards = st;
  }
  void set_search_whole_word(bool st)
  {
    _search_whole_word = st;
  }
  void set_search_from_beginning(bool st)
  {
    _search_from_beginning = st;
  }
  // TODO: Take references ?
  void set_search_text(std::string str)
  {
    _search_text = str;
  }
  void set_replace_text(std::string str)
  {
    _replace_text = str;
  }

  bool search();
  bool search_next();
  void replace();

#ifdef ENABLE_SPELL
  void set_auto_spell(bool);

  // Interaction with the spell dialog.
  void spell_dialog_mode();
  bool spell_dialog_helper_has_misspelled(std::string &word);
  void spell_dialog_helper_get_suggestions(std::string &, std::vector<std::string> &);
  void spell_dialog_helper_add_to_personal(std::string &);
  void spell_dialog_helper_add_to_session(std::string &);
  bool spell_dialog_helper_check(std::string &);
  void spell_dialog_helper_replace(std::string &, std::string &);
  void spell_dialog_helper_recheck();
  bool set_dictionary(std::string &, std::string &);
  std::string get_dictionary()
  {
    return spell_dict;
  }
#endif
  bool is_empty()
  {
    return (_text_view.get_buffer()->begin().get_offset() ==
            _text_view.get_buffer()->end().get_offset());
  }

  void reset_gui();

  std::string get_text()
  {
    return _text_view.get_buffer()->get_text();
  }
  void set_text(std::string &);

 private:
  Label _label;

  friend void _on_move_cursor(GtkTextView *, GtkMovementStep, gint, gboolean, gpointer);
  friend void _on_toggle_overwrite(GtkTextView *, gpointer user_data);

  void signal_text_view_request_file_open_cb(std::string);

#ifdef ENABLE_HIGHLIGHT
  SourceView _text_view;
#else
  TextView _text_view;
#endif
  Gtk::TextWindowType numbers_right;
#ifndef ENABLE_HIGHLIGHT
  Gtk::TextWindowType numbers_left;
#endif

  void get_lines(Gtk::TextIter &, Gtk::TextIter &, std::vector<std::string> &);
  void get_lines(Gtk::TextIter &, Gtk::TextIter &, std::vector<Glib::ustring> &);
  void set_readonly(bool);
  void set_modified(bool);
  void create_ui();
  bool create(const std::string & = "");
  void set_file(std::string &);

  void set_tab_width();

  std::string _tmp_file;
  int _tmp_file_fd;

  std::string _file;
  Conf &_conf;
  Encodings &_encodings;
  bool _ok;
  bool _modified;
  int _encoding;
  bool _readonly;
  bool _line_numbers;

  /* Undo/Redo */
  bool can_undo()
  {
    return _undo.size() == 0 ? false : true;
  }
  bool can_redo()
  {
    return _redo.size() == 0 ? false : true;
  }

  void undo(KatoobDoType, const std::string &, int);
  void redo(KatoobDoElem *);
  void undo(KatoobDoElem *);

  bool do_undo;

  void connect_signals();

  int calculate_column(Gtk::TextIter &);

  void block_do();
  void unblock_do();
  void clear_do();
  void clear_do(std::vector<KatoobDoElem *> &);

  // Our search members and methods.
  bool gtk_search();
  bool nongtk_search();
  bool is_whole_word(Gtk::TextIter &s, Gtk::TextIter &e)
  {
    return (s.starts_word() && e.ends_word());
  }
  void highlight(Gtk::TextIter &, Gtk::TextIter &);

  bool expose_event_cb(GdkEventExpose *);
  void paint_line_numbers(Glib::RefPtr<Gdk::Window> &, GdkEventExpose *);
  //  void get_line_bounds(const Gtk::TextIter&, Gtk::TextIter&, Gtk::TextIter&);
#ifdef ENABLE_SPELL
  Spell spell;
  bool do_spell;
  std::vector<bool> lines;
  sigc::connection spell_worker_conn;
  Gtk::TextIter _spell_start, _spell_end;
  int __spell_start, __spell_end;
  Glib::RefPtr<Gtk::TextMark> _spell_mark;
  bool spell_checker_worker();
  bool spell_checker_has_lines();
  void spell_checker_get_line(Gtk::TextIter &);
  void spell_checker_check(const Gtk::TextIter &);
  bool
  spell_checker_check_word(const Gtk::TextIter &, std::string &, int &, int &, bool mark = true);
  bool spell_checker_get_next(Gtk::TextIter &, Gtk::TextIter &, std::string &, int &, int &);
  void spell_checker_connect_worker();
  void spell_checker_on_insert(const Gtk::TextIter &, int);
  void spell_checker_on_erase(const Gtk::TextIter &, const Gtk::TextIter &);
  Glib::RefPtr<Gtk::TextTag> misspelled_tag;
  //  Glib::RefPtr<Gtk::TextTag> spelled_tag;
  void spell_menu_item_activate_cb(std::string, std::string, Gtk::TextIter &, Gtk::TextIter &);
  void spell_menu_add_to_dictionary_cb(std::string, Gtk::TextIter &, Gtk::TextIter &);
  std::string get_next_misspelled();
  std::string spell_dict;
#endif

  void dict_menu_item_activated(std::string &);

  bool _search_wrap;
  bool _search_backwards;
  bool _search_whole_word;
  bool _search_match_case;
  bool _search_from_beginning;
  std::string _search_text;
  std::string _replace_text;

  std::vector<KatoobDoElem *> _undo;
  std::vector<KatoobDoElem *> _redo;

  /* Our connections */
  sigc::connection insert_conn;
  sigc::connection erase_conn;
  //  sigc::connection expose_event_conn;

  unsigned long __on_move_cursor;
  unsigned long __on_toggle_overwrite;

  bool _overwrite;

  /* Signal handlers */
  void on_insert(const Gtk::TextBuffer::iterator &, const Glib::ustring &, int);
  void on_erase(const Gtk::TextBuffer::iterator &, const Gtk::TextBuffer::iterator &);
  void on_move_cursor();
  void on_toggle_overwrite();
  void on_mark_set_cb(const Gtk::TextBuffer::iterator &,
                      const Glib::RefPtr<Gtk::TextBuffer::Mark> &);

#ifdef ENABLE_HIGHLIGHT
  std::string _highlight;
#endif

 protected:
  void on_populate_popup_cb(Gtk::Menu *);
#ifdef ENABLE_MAEMO
  void _tap_and_hold();
#endif
};

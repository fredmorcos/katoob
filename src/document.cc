/*
 * document.cc
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

#include "document.hh"
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <cerrno>
#include <cstring>
#include <string>
#include <iostream>
#include <sstream>
#include <cassert>
#include <gtkmm/enums.h>
#include <gtkmm/image.h>
#include <gtkmm/stock.h>
#include "dialogs.hh"
#include "macros.h"
#include "textbuffer.hh"
#include "utils.hh"

// TODO:
// highlight current line
// right click on a word -> spell check word
// right click on a line -> spell check line
// right click on a selection -> spell check selection.
// Conf& _conf for search dialog ?
// connect the emulator to all our dialogs.
// custom print won't work ?
// for searching, We need a way to handle the tashkeel and kashida.
// unix/windows/mac line feeds.
// syntax highlighting mode -> spell checker should check comments
// and strings only!

void _on_move_cursor(GtkTextView *textview,
		     GtkMovementStep arg1,
		     gint arg2,
		     gboolean arg3,
		     gpointer user_data) {
  static_cast<Document *>(user_data)->on_move_cursor();
}

void _on_toggle_overwrite(GtkTextView *textview, gpointer user_data) {
  static_cast<Document *>(user_data)->on_toggle_overwrite();
}

Document::Document(Conf& conf, Encodings& encodings, int num) :
  _label(conf),
  _conf(conf),
  _encodings(encodings),
  _ok(true),
  _modified(false),
  _encoding(_encodings.utf8()),
  _readonly(false),
  _line_numbers(false),
  __on_move_cursor(0),
  __on_toggle_overwrite(0),
  _overwrite(false)
{
  _label.set_text(num);
  if (!create()) {
    _ok = false;
  }
}

Document::Document(Conf& conf, Encodings& encodings, int encoding, std::string& file):
  _label(conf),
  _conf(conf),
  _encodings(encodings),
  _ok(false),
  _readonly(false),
  _line_numbers(false),
  __on_move_cursor(0),
  __on_toggle_overwrite(0),
  _overwrite(false)
{
  std::string contents;

  if (Glib::file_test(file, Glib::FILE_TEST_IS_DIR)) {
    katoob_error(file + _(" Is a directory."));
    return;
  }

  // If the file is not there, We will pretend that we did open it.
  if ((Glib::file_test(file, Glib::FILE_TEST_EXISTS)) &&
      (Glib::file_test(file, Glib::FILE_TEST_IS_REGULAR))) {
    try {
      contents = Glib::file_get_contents(file);
    } catch (Glib::FileError& err) {
      katoob_error(err.what());
      return;
    }
  }

  _file = file;

  // is the file writable ?
  _readonly = !Utils::katoob_file_is_writable(file);

  std::string contents2;
  int enc = _encodings.convert_to(contents, contents2, encoding);
  if (enc == -1) {
    _ok = false;
    katoob_error(Utils::substitute(_("Couldn't detect the encoding of %s"), file));
    return;
  }
  _encoding = enc;
  if (!create(enc == _encodings.utf8() ? contents : contents2)) {
    _ok = false;
    return;
  }

#ifdef GLIBMM_EXCEPTIONS_ENABLED
  try {
    _label.set_text(Glib::filename_to_utf8(Glib::path_get_basename(file)));
  }
  catch (Glib::ConvertError& err) {
    _label.set_text(Glib::filename_display_basename(file));
  }
#else
  std::auto_ptr<Glib::Error> error;
  _label.set_text(Glib::filename_to_utf8(Glib::path_get_basename(file), error));
  if (error.get()) {
    _label.set_text(Glib::filename_display_basename(file));
  }
#endif

  _ok = true;
  set_modified(false);

  // auto highlight.
  if (conf.get("highlight_auto", true)) {
    set_highlight(SourceManager::get_language_for_file(file));
  }
}

Document::Document(Conf& conf, Encodings& encodings, int num, int encoding) :
  _label(conf),
  _conf(conf),
  _encodings(encodings),
  _ok(false),
  _readonly(false),
  _line_numbers(false),
  __on_move_cursor(0),
  __on_toggle_overwrite(0),
  _overwrite(false) {
  // TODO: Bad, We are reading character by character.
  std::string contents;
  char ch;
  while (std::cin.get(ch)) {
    contents += ch;
  }

  std::string contents2;
  int enc = _encodings.convert_to(contents, contents2, encoding);
  if (enc == -1) {
    _ok = false;
    std::string str(_("Couldn't detect the encoding of the text."));
    katoob_error(str);
    return;
  }
  _encoding = enc;
  if (!create(enc == _encodings.utf8() ? contents : contents2)) {
    _ok = false;
    return;
  }

  _label.set_text(num);
  _ok = true;
  set_modified(true);
}

Document::~Document() {
  clear_do();

  // Disconnect our handlers.
  if (__on_move_cursor) {
    g_signal_handler_disconnect(G_OBJECT(_text_view.gobj()), __on_move_cursor);
  }
  if (__on_toggle_overwrite) {
    g_signal_handler_disconnect(G_OBJECT(_text_view.gobj()), __on_toggle_overwrite);
  }

  // let's remove our temporary file.
  std::string error;
  if (!Utils::unlock_file(_tmp_file_fd, error)) {
    std::cerr << "Failed to unlock " << _tmp_file << " " << error << std::endl;
  }
  close(_tmp_file_fd);
  unlink(_tmp_file.c_str());
}

void Document::connect_signals() {
  insert_conn = _text_view.get_buffer()->signal_insert().connect(sigc::mem_fun(this, &Document::on_insert));
  erase_conn = _text_view.get_buffer()->signal_erase().connect(sigc::mem_fun(*this, &Document::on_erase));

  _text_view.signal_populate_popup().connect(sigc::mem_fun(*this, &Document::on_populate_popup_cb));
  _text_view.get_buffer()->signal_mark_set().connect(sigc::mem_fun(*this, &Document::on_mark_set_cb));

  _text_view.signal_text_view_request_file_open.connect(sigc::mem_fun(*this, &Document::signal_text_view_request_file_open_cb));

  // TODO: How can one access the move-cursor signal ?
  __on_move_cursor = g_signal_connect_after(G_OBJECT(_text_view.gobj()),
					    "move-cursor",
					    G_CALLBACK (_on_move_cursor),
					    this);
  __on_toggle_overwrite = g_signal_connect (G_OBJECT(_text_view.gobj()),
					    "toggle-overwrite",
					    G_CALLBACK (_on_toggle_overwrite),
					    this);
}

bool Document::has_selection() {
  Gtk::TextIter a, b;
  return _text_view.get_buffer()->get_selection_bounds(a, b);
}

void Document::block_do() {
  // We can't block the signal otherwise our spell checker invalidation
  // methods won't be called.
  do_undo = false;
}

void Document::unblock_do() {
  do_undo = true;
}

void Document::clear_do() {
  clear_do(_undo);
  clear_do(_redo);
}

void Document::clear_do(std::vector<KatoobDoElem *>& v) {
  while (true) {
    if (v.size() == 0) {
	break;
    }
    KatoobDoElem *e = v.back();
    v.pop_back();
    delete e;
  }
}

int Document::get_line_count() {
  return _text_view.get_buffer()->get_char_count() ? _text_view.get_buffer()->get_line_count() : 0;
}

bool Document::create(const std::string& str) {
  if (!Utils::create_recovery_file(_tmp_file, _tmp_file_fd)) {
    katoob_error(Utils::substitute(_("Failed to create the autorecovery file %s"), _tmp_file));
    return false;
  }

  std::string error;
  if (!Utils::lock_file(_tmp_file_fd, error)) {
    katoob_error(Utils::substitute("Failed to lock the temporary file: %s", error));
    return false;
  }

  create_ui();
  connect_signals();

  if (str.size() == 0) {
    return true;
  }

  // We must connect the signals first so that the spell checker can create its lines
  // We will block do_undo
  if (do_undo) {
    do_undo = false;
    set_text(const_cast<std::string&>(str));
    do_undo = true;
  }
  else {
    set_text(const_cast<std::string&>(str));
  }

  return true;
}

void Document::set_text(std::string& str) {
  _text_view.get_buffer()->set_text(str);
}

void Document::create_ui() {
  _text_view.signal_expose_event().connect(sigc::mem_fun(*this, &Document::expose_event_cb));

  //#ifdef ENABLE_HIGHLIGHT
  //  _text_view.set_buffer(SourceBuffer::create());
  //#else
  _text_view.set_buffer(TextBuffer::create(_conf));
  //#endif
  set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  set_shadow_type(Gtk::SHADOW_IN);
  _text_view.set_left_margin(10);
  _text_view.set_right_margin(10);
  _text_view.set_flags(Gtk::CAN_FOCUS);

  set_readonly(_readonly);

  numbers_right = Gtk::TEXT_WINDOW_RIGHT;
  numbers_left = Gtk::TEXT_WINDOW_LEFT;

  grab_focus();

  std::string error;

  do_spell = _conf.get("spell_check", true);
  spell_dict = _conf.get("default_dict", "en");

  // NOTE: We don't check because we might not have a speller pointer created
  // Because aspell wanted to load the locale dictionary and failed.
  // If we check here, We will be getting the previous error which is a result
  // of aspell not an operation we did!
  /*
  if (!spell.ok(error))
    {
      katoob_error(error);
      do_spell = false;
    }
  */
  //  else {
  if (!spell.set_lang(spell_dict, error)) {
    katoob_error(error);
    do_spell = false;
  }
  //  }
  // We create the first line.
  lines.push_back(false);
  spell_checker_connect_worker();

  misspelled_tag = _text_view.get_buffer()->create_tag();
  misspelled_tag->property_underline() = Pango::UNDERLINE_ERROR;

  // TODO: Make these configurable ?
  _search_from_beginning = true;
  _search_wrap = true;
  add(_text_view);
  show_all();
  reset_gui();
}

void Document::grab_focus() {
  _text_view.grab_focus();
}

bool Document::has_focus() {
  return _text_view.has_focus();
}

bool Document::save() {
  return save(_file, _encoding, false);
}

void Document::set_modified(bool m) {
  if (_text_view.get_buffer()->get_modified() != m) {
    _text_view.get_buffer()->set_modified(m);
    signal_modified_set.emit(m);
  }
}

void Document::set_readonly(bool r) {
  _text_view.set_editable(!r);
  signal_readonly_set.emit(r);
}

void Document::scroll_to(int x) {
  Gtk::TextIter iter = _text_view.get_buffer()->get_iter_at_line(--x);
  _text_view.get_buffer()->place_cursor(iter);
  _text_view.scroll_to(iter);
  /*
    _text_view.get_buffer()->move_mark(_text_view.get_buffer()->get_insert(), iter);
  */
}

bool Document::save(std::string& ofile, int enc, bool replace) {
  std::string txt = _text_view.get_buffer()->get_text();
  std::string str, err;

  if (enc == _encodings.utf8()) {
    if (Utils::katoob_write(_conf, ofile, txt, err)) {
      set_modified(false);
      if (replace) {
	set_readonly(false);
	set_file(ofile);
      }
      return true;
    }
    else {
      katoob_error(err);
      return false;
    }
  }
  else if (_encodings.convert_from(txt, str, enc) != -1) {
    if (Utils::katoob_write(_conf, ofile, str, err)) {
      set_modified(false);
      if (replace) {
	set_readonly(false);
	set_file(ofile);

	// NOTE: We are doing it manually without calling set_encoding() because it will also mark
	// the Document as modified but it's not.
	_encoding = enc;
	signal_encoding_changed.emit(enc);
      }
      return true;
    }
    else {
      katoob_error(err);
      return false;
    }
  }
  else {
    katoob_error(str);
    return false;
  }
}

void Document::set_file(std::string& nfile) {
  _file = nfile;

#ifdef GLIBMM_EXCEPTIONS_ENABLED
  std::string f;
  try {
    f = Glib::filename_to_utf8(Glib::path_get_basename(nfile));
  }
  catch (...) {
    // We shouldn't be here.
    f = Glib::filename_display_basename(nfile);
  }
#else
  std::auto_ptr<Glib::Error> error;
  std::string f = Glib::filename_to_utf8(Glib::path_get_basename(nfile), error);
  if (error.get()) {
  // We should not be getting here anyway but just in case ?
    f = Glib::filename_display_basename(nfile);
  }
#endif

  _label.set_text(f);
  signal_file_changed.emit(nfile);
  signal_title_changed.emit(f);
}

void Document::on_insert(const Gtk::TextBuffer::iterator& iter , const Glib::ustring& str, int len) {
  // Let's add to our undo stack.
  if (do_undo) {
    undo(KATOOB_DO_INSERT, str, Glib::RefPtr<TextBuffer>::cast_dynamic(_text_view.get_buffer())->get_mark_insert_position());
  }

  spell_checker_on_insert(iter, len);
  spell_checker_connect_worker();

  on_move_cursor();
  signal_modified_set.emit(true);
}

void Document::on_erase(const Gtk::TextBuffer::iterator& start, const Gtk::TextBuffer::iterator& end) {
  if (do_undo) {
    Glib::RefPtr<TextBuffer> b =  Glib::RefPtr<TextBuffer>::cast_dynamic(_text_view.get_buffer());
    undo(KATOOB_DO_DELETE, b->get_deleted(), start.get_offset());
    b->clear_deleted();
  }

  spell_checker_on_erase(start, end);
  spell_checker_connect_worker();

  on_move_cursor();
  signal_modified_set.emit(true);
}

void Document::on_mark_set_cb(const Gtk::TextBuffer::iterator& iter, const Glib::RefPtr<Gtk::TextBuffer::Mark>& mark) {
  if ((mark) && (mark == _text_view.get_buffer()->get_insert())) {
    on_move_cursor();
  }
}

void Document::on_toggle_overwrite() {
  // NOTE: The gtk API gives me false on the first time which is strange.
  _overwrite = !_overwrite;
  signal_overwrite_toggled.emit(_overwrite);
}

void Document::on_move_cursor() {
  Gtk::TextIter iter = _text_view.get_buffer()->get_iter_at_mark(_text_view.get_buffer()->get_insert());
  signal_cursor_moved.emit(calculate_column(iter)+1, iter.get_line()+1);
}

int Document::calculate_column(Gtk::TextIter& iter) {
  // TODO: How to get the actual tab width ?

  int column = 0;
  Gtk::TextIter start = iter;

  start.set_line_offset(0);

  while (start != iter) {
    column += start.get_char() == '\t' ? 8 : 1;
    start.forward_char();
  }

  return column;
}

void Document::insert(const std::string& str) {
  _text_view.get_buffer()->insert_at_cursor(str);
}

void Document::undo() {
  if (_undo.size() == 0) {
    return;
  }

  KatoobDoElem *e = _undo.back();

  _undo.pop_back();

  undo(e);

  if (_undo.size() == 0) {
    signal_can_undo.emit(false);
  }
}

void Document::redo() {
  if (_redo.size() == 0) {
    return;
  }

  KatoobDoElem *e = _redo.back();

  _redo.pop_back();

  redo(e);

  int x = _redo.size();
  if(x == 0) {
    signal_can_redo.emit(false);
  }
  else if (x == 1) {
    signal_can_redo.emit(true);
  }
}

void Document::undo(KatoobDoType a, const std::string& t, int p) {
  KatoobDoElem *e = new KatoobDoElem(a, t, p);
  _undo.push_back(e);
  if (_undo.size() == 1) {
    signal_can_undo.emit(true);
  }

  // Did we exceed ?
  unsigned s = _conf.get("undono", 0);

  if (s == 0) {
    // unlimited.
    return;
  }

  while (s < _undo.size()) {
    KatoobDoElem *_e = _undo.front();
    delete _e;
    _undo.erase(_undo.begin());
  }
}

/* TODO: Combine the next 2 */
void Document::redo(KatoobDoElem *e) {
  block_do();

  // We are dealing with char offsets.
  Gtk::TextIter iter = _text_view.get_buffer()->get_iter_at_offset(e->pos());
  if (e->action() == KATOOB_DO_INSERT) {
    // We will delete.
    Gtk::TextIter iter2 = iter;
    int n = iter2.get_offset();
    n += Glib::ustring(e->text()).size();
    iter2.set_offset(n);
    _text_view.get_buffer()->erase(iter, iter2);
  }
  else {
    // We will insert.
    _text_view.get_buffer()->insert(iter, e->text());
  }

  // Change action.
  e->toggle();

  _undo.push_back(e);
  unblock_do();

  // Did we exceed ?
  unsigned s = _conf.get("undono", 0);

  if (s == 0) {
    // unlimited.
    if (_undo.size() == 1) {
      signal_can_undo.emit(true);
    }
    return;
  }

  while (s < _undo.size()) {
    KatoobDoElem *_e = _undo.front();
    delete _e;
    _undo.erase(_undo.begin());
  }
  if (_undo.size() == 1) {
    signal_can_undo.emit(true);
  }
}

void Document::undo(KatoobDoElem *e) {
  block_do();

  // We are dealing with char offsets.
  Gtk::TextIter iter = _text_view.get_buffer()->get_iter_at_offset(e->pos());
  if (e->action() == KATOOB_DO_INSERT) {
    // We will delete.
    Gtk::TextIter iter2 = iter;
    int n = iter2.get_offset();
    n += Glib::ustring(e->text()).size();
    iter2.set_offset(n);
    _text_view.get_buffer()->erase(iter, iter2);
  }
  else {
    // We will insert.
    _text_view.get_buffer()->insert(iter, e->text());
  }

  // Change action.
  e->toggle();

  _redo.push_back(e);
  unblock_do();

  // Did we exceed ?
  unsigned s = _conf.get("undono", 0);

  if (s == 0) {
    // unlimited.
    if (_redo.size() == 1) {
      signal_can_redo.emit(true);
    }
    return;
  }

  while (s < _redo.size()) {
    KatoobDoElem *_e = _redo.front();
    delete _e;
    _redo.erase(_redo.begin());
  }
  if (_redo.size() == 1) {
    signal_can_redo.emit(true);
  }
}

void Document::get_lines(std::vector<std::string>& l, int x, int y) {
  Gtk::TextIter start, end;

  if (x == -1) {
    start = _text_view.get_buffer()->begin();
  }
  else {
    start = _text_view.get_buffer()->get_iter_at_line(x);
  }

  if (y == -1) {
    end = _text_view.get_buffer()->end();
  }
  else {
    end = _text_view.get_buffer()->get_iter_at_line(y);
  }
  get_lines(start, end, l);
}

void Document::get_lines(std::vector<Glib::ustring>& l, int x, int y) {
  Gtk::TextIter start, end;

  if (x == -1) {
    start = _text_view.get_buffer()->begin();
  }
  else {
    start = _text_view.get_buffer()->get_iter_at_line(x);
  }

  if (y == -1) {
    end = _text_view.get_buffer()->end();
  }
  else {
    end = _text_view.get_buffer()->get_iter_at_line(y);
  }
  get_lines(start, end, l);
}

void Document::get_selection(std::vector<std::string>& l) {
  Gtk::TextIter start, end;
  _text_view.get_buffer()->get_bounds(start, end);
  get_lines(start, end, l);
}

void Document::get_lines(Gtk::TextIter& start, Gtk::TextIter& end, std::vector<std::string>& l) {
  Gtk::TextIter dummy;
  while (start <= end) {
    dummy = start;
    if (!dummy.ends_line()) {
      dummy.forward_to_line_end();
    }

    l.push_back(_text_view.get_buffer()->get_text(start, dummy, false));

    if (!start.forward_line()) {
      break;
    }
  }
}

void Document::get_lines(Gtk::TextIter& start, Gtk::TextIter& end, std::vector<Glib::ustring>& l) {
  Gtk::TextIter dummy;
  while (start <= end) {
    dummy = start;
    if (!dummy.ends_line()) {
      dummy.forward_to_line_end();
    }

    l.push_back(_text_view.get_buffer()->get_text(start, dummy, false));

    if (!start.forward_line()) {
      break;
    }
  }
}

void Document::select_all() {
  Gtk::TextIter start, end;

  _text_view.get_buffer()->get_bounds(start, end);
  _text_view.get_buffer()->move_mark_by_name ("selection_bound", start);
  _text_view.get_buffer()->move_mark_by_name ("insert", end);
}

void Document::set_wrap_text(bool wrap) {
  _text_view.set_wrap_mode(wrap ? Gtk::WRAP_WORD : Gtk::WRAP_NONE);
}

void Document::line_numbers(bool show) {
// #ifdef ENABLE_HIGHLIGHT
//   //  _text_view.set_show_line_numbers(show);
//   // If we will be showing it on the left side then we need to inform gtksourceview.

//   _line_numbers = show;

//   if (show) {
//     if (_conf.get("numbers_right", true)) {
//       _text_view.set_border_window_size(numbers_right, 20);
//     }
//     else {
//       _text_view.set_border_window_size(numbers_right, 0);
//     }
//     if (_conf.get("numbers_left", true)) {
//       _text_view.se_left_line_numbers(true);
//     }
//     else {
//       _text_view.se_left_line_numbers(false);
//     }
//   }
//   else {
//     _text_view.set_border_window_size(numbers_right, 0);
//     _text_view.se_left_line_numbers(false);
//   }
//#else
  _line_numbers = show;

  if (show) {
    if (_conf.get("numbers_right", true)) {
      _text_view.set_border_window_size(numbers_right, 20);
    }
    else {
      _text_view.set_border_window_size(numbers_right, 0);
    }
    if (_conf.get("numbers_left", true)) {
      _text_view.set_border_window_size(numbers_left, 20);
    }
    else {
      _text_view.set_border_window_size(numbers_left, 0);
    }
  }
  else {
    _text_view.set_border_window_size(numbers_right, 0);
    _text_view.set_border_window_size(numbers_left, 0);
  }
  //#endif
}

void Document::emit_signals() {
  signal_can_undo.emit(can_undo());
  signal_can_redo.emit(can_redo());
  signal_modified_set.emit(get_modified());
  signal_readonly_set.emit(get_readonly());
  signal_file_changed.emit(_file);
  signal_encoding_changed.emit(_encoding);
  signal_overwrite_toggled.emit(_overwrite);
  on_move_cursor();
  signal_title_changed.emit(get_title());
  signal_auto_spell_set.emit(do_spell);
  signal_dictionary_changed.emit(spell_dict);

  signal_highlight_set.emit(_highlight);

  signal_wrap_text_set.emit((_text_view.get_wrap_mode() == Gtk::WRAP_NONE ? false : true));
  signal_line_numbers_set.emit(_line_numbers);
}

bool Document::search() {
  assert(_search_text.size() > 0);

  if (_search_match_case) {
    // Gtk will handle this.
    return gtk_search();
  }
  else {
    return nongtk_search();
  }
}

// TODO: Combine the next 2.
bool Document::gtk_search() {
  bool found;
  Gtk::TextIter iter, start, end;
  while (true) {
    if (_search_from_beginning)
      {
	if (_search_backwards) {
	  iter = _text_view.get_buffer()->end();
	}
	else {
	  iter = _text_view.get_buffer()->begin();
	}
      }
    else {
      Gtk::TextIter dummy1, dummy2;
      if (_text_view.get_buffer()->get_selection_bounds(dummy1, dummy2)) {
	if (_search_backwards) {
	  iter = dummy1;
	}
	else {
	  iter = dummy2;
	}
      }
      else {
	iter = _text_view.get_buffer()->get_iter_at_mark(_text_view.get_buffer()->get_insert());
      }
    }

    if (_search_backwards) {
      found = iter.backward_search(_search_text, Gtk::TextSearchFlags(Gtk::TEXT_SEARCH_TEXT_ONLY|Gtk::TEXT_SEARCH_VISIBLE_ONLY), start, end);
    }
    else {
      found = iter.forward_search(_search_text, Gtk::TextSearchFlags(Gtk::TEXT_SEARCH_TEXT_ONLY|Gtk::TEXT_SEARCH_VISIBLE_ONLY), start, end);
    }

    if ((found) && ((_search_whole_word) && (is_whole_word(start, end)))) {
      highlight(start, end);
      _search_from_beginning = false;
      return true;
    }
    else if ((found) && (!_search_whole_word)) {
      highlight(start, end);
      _search_from_beginning = false;
      return true;
    }
    else if ((_search_wrap) && (!_search_from_beginning)) {
      _search_from_beginning = true;
      return gtk_search();
    }
    else {
      return false;
    }
  }
}

bool Document::nongtk_search() {
  // NOTE: Gtk doesn't have a case insensitive search.
  // TODO: Is there a way better than this ?
  // I can iterate through the buffer but is the white soace used to split
  // words in all languages ??
  // TODO: Cache this ?
  bool found;
  Gtk::TextIter iter, start, end;
  // Let's construct our buffer
  Glib::ustring _str = _text_view.get_buffer()->get_text();
  Glib::ustring str = _str.uppercase();
  _str.clear();
  Glib::ustring _s = _search_text;
  Glib::ustring s = _s.uppercase();
  _s.clear();
  Glib::RefPtr<Gtk::TextBuffer> buffer = Gtk::TextBuffer::create();
  buffer->set_text(str);
  str.clear();

  iter = buffer->get_iter_at_line(0);
  Gtk::TextIter _start = _text_view.get_buffer()->get_iter_at_line(0);
  Gtk::TextIter _end = _start;

  while (true) {
    if (_search_from_beginning) {
      if (_search_backwards) {
	iter = buffer->end();
      }
      else {
	iter = buffer->begin();
      }
    }
    else {
      Gtk::TextIter dummy1, dummy2;
      if (_text_view.get_buffer()->get_selection_bounds(dummy1, dummy2)) {
	if (_search_backwards) {
	  iter.set_offset(dummy1.get_offset());
	}
	else {
	  iter.set_offset(dummy2.get_offset());
	}
      }
      else {
	iter.set_offset(_text_view.get_buffer()->get_iter_at_mark(_text_view.get_buffer()->get_insert()).get_offset());
      }
    }

    if (_search_backwards) {
      found = iter.backward_search(s, Gtk::TextSearchFlags(Gtk::TEXT_SEARCH_TEXT_ONLY|Gtk::TEXT_SEARCH_VISIBLE_ONLY), start, end);
    }
    else {
      found = iter.forward_search(s, Gtk::TextSearchFlags(Gtk::TEXT_SEARCH_TEXT_ONLY|Gtk::TEXT_SEARCH_VISIBLE_ONLY), start, end);
    }

    if (found) {
      _start.set_offset(start.get_offset());
      _end.set_offset(end.get_offset());
    }
    if ((found) && ((_search_whole_word) && (is_whole_word(_start, _end)))) {
      highlight(_start, _end);
      _search_from_beginning = false;
      return true;
    }
    else if ((found) && (!_search_whole_word)) {
      highlight(_start, _end);
      _search_from_beginning = false;
      return true;
    }
    else if ((_search_wrap) && (!_search_from_beginning)) {
      _search_from_beginning = true;
      return nongtk_search();
    }
    else {
      return false;
    }
  }
}

void Document::highlight(Gtk::TextIter& start, Gtk::TextIter& end) {
  _text_view.get_buffer()->place_cursor(start);
  _text_view.get_buffer()->move_mark(_text_view.get_buffer()->get_insert(), end);
  _text_view.scroll_to(end);
}

bool Document::search_next() {
  return search();
}

void Document::replace() {
  Gtk::TextIter _s, _e;
  _text_view.get_buffer()->get_selection_bounds(_s, _e);
  int offset = _s.get_offset();

  _text_view.get_buffer()->erase_selection();
  insert(_replace_text);
  if (_search_backwards) {
    _s = _text_view.get_buffer()->get_iter_at_offset(offset);
    _text_view.get_buffer()->place_cursor(_s);
  }
}

bool Document::expose_event_cb(GdkEventExpose *event) {
  if (!_line_numbers) {
    return false;
  }
  // TODO: Should we connect/disconnect this when needed since the spell
  // is handled by an idle worker ??

  Glib::RefPtr<Gdk::Window> wl = _text_view.get_window(numbers_left);
  Glib::RefPtr<Gdk::Window> wr = _text_view.get_window(numbers_right);

  if ((!wl) && (!wr)) {
    return false;
  }

  // left.
  if ((wl) && (wl->gobj() == event->window)) {
    paint_line_numbers(wl, event);
  }

  if ((wr) && (wr->gobj() == event->window)) {
    paint_line_numbers(wr, event);
  }
  return false;
}

void Document::paint_line_numbers(Glib::RefPtr<Gdk::Window>& win, GdkEventExpose *event) {
  // Let's get the current line number.
  int current_line = _text_view.get_buffer()->get_iter_at_mark(_text_view.get_buffer()->get_insert()).get_line() + 1;

  win->clear();

  // We need to calculate the maximum width possible.
  Gtk::TextIter end = _text_view.get_buffer()->end();

  Gtk::TextWindowType w = _text_view.get_window_type(win);

  std::stringstream width;
  width << (end.get_line() + 1);
  Glib::RefPtr<Pango::Layout> layout = _text_view.create_pango_layout(width.str().c_str());
  int size = 0, dummy;
  layout->get_pixel_size(size, dummy);
  _text_view.set_border_window_size(w, size + 4);

  // Now let's paint.
  Gdk::Rectangle rect;
  _text_view.get_visible_rect(rect);

  Gtk::TextIter s, e;
  int top1, top2;
  _text_view.get_line_at_y (s, rect.get_y(), top1);
  _text_view.get_line_at_y (e, rect.get_y() + rect.get_height(), top2);

  int _s = s.get_line();
  int _e = e.get_line();

  //  Gtk::StateType st = _text_view.get_state();

  Glib::ustring dummy3;

  for (int x = _s; x <= _e;) {
    // We are reusing what's declared above!!
    end.set_line(x);
    _text_view.get_line_yrange(end, top1, top2);

    _text_view.buffer_to_window_coords(w, 0, top1, top1, top2);
    std::string w;
    if (current_line == x + 1) {
      w = Utils::substitute("<b>%i</b>", ++x);
      layout->set_markup(w.c_str());
    }
    else {
      w = Utils::substitute("%i", ++x);
      // NOTE: Why set_text() will keep it bold ?
      layout->set_markup(w.c_str());
    }

    // TODO: This one is not working !!!
    //_text_view.get_style()->paint_layout(win, st, false, rect, _text_view, dummy3, 0, top2, layout);
    gtk_paint_layout (dynamic_cast<Gtk::Widget&>(_text_view).gobj()->style, win->gobj(), (GtkStateType)GTK_WIDGET_STATE (_text_view.gobj()), FALSE, NULL, dynamic_cast<Gtk::Widget&>(_text_view).gobj(), NULL, 2, top2, layout->gobj());
  }
}

bool Document::set_encoding(int e, bool convert, std::string& err) {
  if (!convert) {
    _encoding = e;
  }
  else {
    std::string ntxt, txt, t = _text_view.get_buffer()->get_text();
    // Let's get it back to its original encoding.
    if (_encodings.convert_from(t, ntxt, _encoding) != -1) {
      // empty this memory.
      t.clear();

      // If the new encoding is utf8, We'll just insert it.
      if (e == _encodings.utf8()) {
	  _encoding = e;
	  _text_view.get_buffer()->set_text(ntxt);
      }
      // Now let's convert it to the new encoding.
      else if (_encodings.convert_to(ntxt, txt, e) != -1) {
	_encoding = e;
	_text_view.get_buffer()->set_text(txt);
      }
      else {
	err = txt;
	return false;
      }
    }
    else {
      err = ntxt;
      return false;
    }
  }

  signal_encoding_changed.emit(e);

  // We are emitting the signal by ourselves because a change in the encoding is not
  // considered a modification by Gtk::TextBuffer
  _text_view.get_buffer()->set_modified(true);
  signal_modified_set.emit(true);

  // TODO: Do we need to be able to undo/redo the change in the encoding ?
  // If yes:
  // we also need to save the previous encoding.
  // we also need to account for changes such as not having a file then saving so we will now have one, ... etc.
  //  if (do_undo)
  //    undo(KATOOB_DO_ENCODING, str, Glib::RefPtr<TextBuffer>::cast_dynamic(_text_view.get_buffer())->get_mark_insert_position());

  return true;
}

bool Document::revert(std::string& err) {
  std::string contents;
  if (!Utils::katoob_read(_file, contents)) {
    err = contents;
    return false;
  }

  if (_encoding == _encodings.utf8()) {
    _text_view.get_buffer()->set_text(contents);
  }
  else {
    std::string c;
    if (!_encodings.convert_to(contents, c, _encoding) == -1) {
      err = c;
      return false;
    }
    else {
      _text_view.get_buffer()->set_text(c);
    }
  }

  clear_do(_undo);
  clear_do(_redo);
  set_modified(false);
  signal_can_undo.emit(false);
  signal_can_redo.emit(false);
  return true;
}

void Document::dict_menu_item_activated(std::string& word) {
  signal_dict_lookup_request.emit(word);
}

void Document::on_populate_popup_cb(Gtk::Menu *menu) {
  Gtk::TextIter start, end;

  start = _text_view.get_buffer()->get_iter_at_mark(_text_view.get_buffer()->get_insert());
  end = start;

  if (!start.starts_word()) {
    start.backward_word_start();
  }
  if (end.inside_word()) {
    end.forward_word_end();
  }
  std::string word = _text_view.get_buffer()->get_text(start, end, false);

  if (word.size() > 0) {
    // if the word ends in a new line, we'll get rid of it.
    if (word[word.size()-1] == '\n') {
      word = word.substr(0, word.size()-1);
    }
    menu->items().push_back(Gtk::Menu_Helpers::SeparatorElem());
    Gtk::MenuItem *item;
    std::string str = _("Define ") + word;
    menu->items().push_back(Gtk::Menu_Helpers::MenuElem(str));
    item = &menu->items().back();
    item->signal_activate().connect(sigc::bind<std::string>(sigc::mem_fun(*this, &Document::dict_menu_item_activated), word));
  }

  if (get_readonly()) {
    return;
  }

  if (do_spell) {
    if (start.has_tag(misspelled_tag)) {
      // Misspelled word.
      menu->items().push_front(Gtk::Menu_Helpers::SeparatorElem());
      Gtk::Menu *spell_menu = Gtk::manage( new Gtk::Menu());
      std::string str(_("Spelling Suggestions"));

      Gtk::Image *image = Gtk::manage(new Gtk::Image(Gtk::StockID(Gtk::Stock::SPELL_CHECK), Gtk::IconSize(Gtk::ICON_SIZE_MENU)));
      menu->items().push_front(Gtk::Menu_Helpers::ImageMenuElem(str, *image, *spell_menu));
      // Let's build the suggestions menu.
      std::vector<std::string> suggestions;
      spell.suggest(word, suggestions);

      str += Utils::substitute(_("Add \"%s\" to dictionary"), word);
      image = Gtk::manage(new Gtk::Image(Gtk::StockID(Gtk::Stock::ADD), Gtk::IconSize(Gtk::ICON_SIZE_MENU)));
      spell_menu->items().push_back(Gtk::Menu_Helpers::ImageMenuElem(str, *image));
      Gtk::MenuItem *item = &spell_menu->items().back();
      item->signal_activate().connect(sigc::bind<std::string, Gtk::TextIter, Gtk::TextIter>(sigc::mem_fun(*this, &Document::spell_menu_add_to_dictionary_cb), word, start, end));
      spell_menu->items().push_back(Gtk::Menu_Helpers::SeparatorElem());

      if (suggestions.size() == 0) {
	// TODO: Use italics markup.
	std::string str = _("no suggestions");
	spell_menu->items().push_back(Gtk::Menu_Helpers::MenuElem(str));
	item = &spell_menu->items().back();
	item->set_sensitive(false);
      }
      else {
	for (unsigned x = 0; x < suggestions.size(); x++) {
	  // TODO: Make this configurable ?
	  if ((x != 0) && !(x%10)) {
	    spell_menu->items().push_back(Gtk::Menu_Helpers::SeparatorElem());
	    Gtk::Menu *spell_menu_sub = Gtk::manage( new Gtk::Menu());
	    std::string str(_("More..."));
	    spell_menu->items().push_back(Gtk::Menu_Helpers::MenuElem(str, *spell_menu_sub));
	    spell_menu = spell_menu_sub;
	  }
	  spell_menu->items().push_back(Gtk::Menu_Helpers::MenuElem(suggestions[x]));
	  item = &spell_menu->items().back();
	  item->signal_activate().connect(sigc::bind<std::string, std::string, Gtk::TextIter, Gtk::TextIter>(sigc::mem_fun(*this, &Document::spell_menu_item_activate_cb), word, suggestions[x], start, end));
	}
      }
    }
  }
}

void Document::spell_checker_connect_worker() {
  if (do_spell) {
    if (!spell_worker_conn.connected()) {
      spell_worker_conn = Glib::signal_idle().connect(sigc::mem_fun(*this, &Document::spell_checker_worker), G_PRIORITY_LOW);
    }
  }
  else {
    if (spell_worker_conn.connected()) {
      spell_worker_conn.disconnect();
    }
  }
}

bool Document::spell_checker_worker() {
  // Do we have lines to check ?
  if (spell_checker_has_lines()) {
    // Ask for a line
    Gtk::TextIter iter;
    spell_checker_get_line(iter);
    //      std::cout << "Got Line " << iter.get_line() << std::endl;
    spell_checker_check(iter);
    lines[iter.get_line()] = false;
  } else {
    spell_worker_conn.disconnect();
  }

  // We will always return true.
  return true;
}

bool Document::spell_checker_has_lines() {
  for (auto i = lines.begin(); i != lines.end(); i++) {
    if (*i) {
      return true;
    }
  }

  return false;
}

void Document::spell_checker_on_insert(const Gtk::TextIter& iter, int len) {
  int pos = Glib::RefPtr<TextBuffer>::cast_dynamic(_text_view.get_buffer())->get_mark_insert_line();
  int end = iter.get_line();
  if (pos == end) {
    // TODO: There's a possibility it might be valid if the line contains
    // White spaces for example.
    lines[pos] = true;
  }
  else {
    // Let's insert the lines.
    std::vector<bool>::iterator lines_iter = lines.begin();
    lines_iter += pos;
    lines_iter++;
    lines.insert(lines_iter, end - pos, true);
    lines[pos] = true;
  }
}

void Document::spell_checker_on_erase(const Gtk::TextIter& start, const Gtk::TextIter& end) {
  unsigned s = end.get_line();
  unsigned e = Glib::RefPtr<TextBuffer>::cast_dynamic(_text_view.get_buffer())->get_erase_line();

  if (s == e) {
    lines[s] = true;
  }
  else {
    std::vector<bool>::iterator _s, _e;
    _s = _e = lines.begin();
    _s += s;
    _s++;
    _e += e;
    _e++;
    lines.erase(_s, _e);
    lines[s] = true;
    if (lines.size() > s) {
      lines[++s] = true;
    }
  }
  // invalidate the word.
}

void Document::spell_checker_get_line(Gtk::TextIter& iter) {
  for (unsigned x = 0; x < lines.size(); x++) {
    if (lines[x]) {
      iter = _text_view.get_buffer()->get_iter_at_line(x);
      return;
    }
  }
#if 0
  Gdk::Rectangle rect;
  _text_view.get_visible_rect(rect);

  Gtk::TextIter s, e;
  int top1, top2;
  _text_view.get_line_at_y (s, rect.get_y(), top1);
  _text_view.get_line_at_y (e, rect.get_y() + rect.get_height(), top2);

  unsigned _s = s.get_line();
  unsigned _e = e.get_line();

  // TODO: Is there a better way ??
  Gtk::TextIter c = _text_view.get_buffer()->get_iter_at_mark(_text_view.get_buffer()->get_insert());

  unsigned _c = c.get_line();

  // is the cursor visible on the screen ?
  if ((_c >= _s) && (_c <= _e) && (lines[c.get_line()])) {
    iter = c;
    return;
  }
  // Now let's search the visible region.
  for (unsigned x = _s; x <= _e; x++) {
    if (lines[x]) {
      iter = _text_view.get_buffer()->get_iter_at_line(x);
      return;
    }
  }
  // Hell, We didn't find.
  // Let's search downwards.
  unsigned y = get_line_count();

  // We are repeating _e here!
  for (unsigned x = _e; x <= y; x++) {
    if (lines[x]) {
      iter = _text_view.get_buffer()->get_iter_at_line(x);
      return;
    }
  }
  // Damn, We didn't find. Let's search upwards.
  // We are repeating _s here.
  for (unsigned x = _s; x <= 0; x--) {
    if (lines[x]) {
      iter = _text_view.get_buffer()->get_iter_at_line(x);
      return;
    }
  }
  // We shouldn't reach this.
  assert("1 ~= 1");
#endif
}

void Document::spell_checker_check(const Gtk::TextIter& iter) {
  Gtk::TextIter start = iter, end = iter;

  start.set_line_offset(0);

  if (!end.ends_line()) {
    end.forward_to_line_end();
  }

  //  std::cout << "ENTERING " << start.get_line() << ":" << start.get_line_offset() << " " << end.get_line() << ":" << end.get_line_offset() << std::endl;
  /*
  if (!start.starts_word())
    {
      std::cout << "Doesn't start a word" << std::endl;
      if (start.inside_word() || start.ends_word())
	{
	  std::cout << "inside or ends" << std::endl;
	  start.backward_word_start();
	}
      else // probably a white space.
	{
	  std::cout << "White space" << std::endl;
	  if (start.forward_word_end())
	    start.backward_word_start();
	}
    }

  */
  // offset 0 is always inside a word. Probably this is a bug somewhere.
#if 0
  if (start.get_offset() == 0) {
    start.forward_word_end();
    start.backward_word_start();
  }
#endif
  //  word = start;
  std::string word;

  int s, e;

  while (spell_checker_get_next(start, end, word, s, e)) {
    spell_checker_check_word(iter, word, s, e);
  }
}

bool Document::spell_dialog_helper_check(std::string& word) {
  return spell.check(word);
}

void Document::spell_dialog_helper_replace(std::string& old_word, std::string& new_word) {
  spell.replace(old_word, new_word);
  Gtk::TextIter s, e;
  _text_view.get_buffer()->get_selection_bounds(s, e);
  _text_view.get_buffer()->erase(s, e);
  _text_view.get_buffer()->insert_at_cursor(new_word);
  _spell_end = _text_view.get_buffer()->end();
  _spell_start = _text_view.get_buffer()->get_iter_at_mark(_text_view.get_buffer()->get_insert());
}

void Document::spell_dialog_helper_add_to_personal(std::string& old_word) {
  spell.to_personal(old_word);
  Gtk::TextIter s, e;
  _text_view.get_buffer()->get_selection_bounds(s, e);
  _text_view.get_buffer()->remove_tag(misspelled_tag, s, e);
}

void Document::spell_dialog_helper_add_to_session(std::string& old_word) {
  spell.to_session(old_word);
  Gtk::TextIter s, e;
  _text_view.get_buffer()->get_selection_bounds(s, e);
  _text_view.get_buffer()->remove_tag(misspelled_tag, s, e);
}

bool Document::spell_dialog_helper_has_misspelled(std::string& word) {
  while (spell_checker_get_next(_spell_start, _spell_end, word, __spell_start, __spell_end)) {
    if (!spell_checker_check_word(_spell_start, word, __spell_start, __spell_end, false)) {
      return true;
    }
  }
  return false;
}

void Document::spell_dialog_helper_recheck() {
  if (do_spell) {
    for (unsigned x = 0; x < lines.size(); x++) {
      lines[x] = true;
    }
    spell_checker_connect_worker();
  }
}

void Document::spell_dialog_helper_get_suggestions(std::string& word, std::vector<std::string>& suggestions) {
  spell.suggest(word, suggestions);
}

void Document::spell_dialog_mode() {
  _text_view.get_buffer()->get_bounds(_spell_start, _spell_end);
}

bool Document::spell_checker_get_next(Gtk::TextIter& start, Gtk::TextIter& end, std::string& _word, int& s, int& e) {
  //  std::cout << "spell_checker_get_next" << std::endl;
  /*
    if (!start.starts_word())
    {
    std::cout << "Doesn't start a word" << std::endl;
    if (start.inside_word() || start.ends_word())
    {
    std::cout << "inside or ends" << std::endl;
	  if (start.get_offset() == 0)
	  start.backward_word_start();
	  }
	  else // probably a white space.
	  {
	  std::cout << "White space" << std::endl;
          if (start.forward_word_end())
	  start.backward_word_start();
	  }
	  }


  */

  Gtk::TextIter word = start;

  word.forward_word_end();

  if (!start.starts_word()) {
    //      std::cout << "Doesn't start a word" << std::endl;
    if (start.inside_word() || start.ends_word()) {
      //	  std::cout << "inside or ends" << std::endl;
      //	  if (start.get_offset() == 0)
      if (start.forward_word_end()) {
	start.backward_word_start();
      }
    }
    else {
      // probably a white space.
      //	  std::cout << "White space" << std::endl;
      if (start.forward_word_end()) {
	start.backward_word_start();
      }
    }
  }

  if (word > end) {
    start = word;
    return false;
  }

  if (word == start) {
    start = word;
    return false;
  }

  _word = _text_view.get_buffer()->get_text(start, word);

  s = start.get_offset();
  e = word.get_offset();
  //  if (first)
  start = word;

  return true;
}

bool Document::spell_checker_check_word(const Gtk::TextIter& iter,
                                        std::string& word,
                                        int& s, int& e, bool mark) {
  Gtk::TextIter _s = iter, _e = iter;
  _s.set_offset(s);
  _e.set_offset(e);

  bool st = spell.check(word);
  //  std::cout << "GOT WORD " << word << " " << st << std::endl;
  if (mark) {
    if (!st) {
      _text_view.get_buffer()->apply_tag(misspelled_tag, _s, _e);
    }
    else {
      _text_view.get_buffer()->remove_tag(misspelled_tag, _s, _e);
    }
  }
  else {
    if (!st) {
      highlight(_s, _e);
    }
  }
  return st;
}

void Document::set_auto_spell(bool st) {
  if (st == do_spell) {
    return;
  }

  std::string error;

  if (do_spell) {
    do_spell = st;
  }
  else {
    std::string error;
    if (!spell.set_lang(spell_dict, error)) {
      katoob_error(error);
      signal_auto_spell_set.emit(do_spell);
      return;
    }

#if 0
    if (!spell.ok(error)) {
      katoob_error(error);
      signal_auto_spell_set.emit(do_spell);
      return;
    }
#endif
    else {
      do_spell = st;
    }
  }
  spell_checker_connect_worker();
  if (!do_spell) {
    for (unsigned x = 0; x < lines.size(); x++) {
      lines[x] = true;
    }

    Gtk::TextIter start = _text_view.get_buffer()->begin();
    Gtk::TextIter end = _text_view.get_buffer()->end();
    _text_view.get_buffer()->remove_tag(misspelled_tag, start, end);
  }
  signal_auto_spell_set.emit(do_spell);
}

void Document::spell_menu_item_activate_cb(std::string old_word,
                                           std::string new_word,
                                           Gtk::TextIter& start,
                                           Gtk::TextIter& end)
{
  // TODO: This will cause the whole line to be rechecked.
  _text_view.get_buffer()->erase(start, end);
  _text_view.get_buffer()->insert_at_cursor(new_word);
  spell.replace(old_word, new_word);
}

void Document::spell_menu_add_to_dictionary_cb(std::string str,
                                               Gtk::TextIter& start,
                                               Gtk::TextIter& end)
{
  spell.to_personal(str);

  // TODO: We need to remove the tag from the word.
  // And to check the whole buffer.
  _text_view.get_buffer()->remove_tag(misspelled_tag, start, end);
  for (unsigned x = 0; x < lines.size(); x++) {
    lines[x] = true;
  }
  spell_checker_connect_worker();
}

bool Document::set_dictionary(std::string& dict, std::string& error) {
  if (spell.set_lang(dict, error)) {
    for (unsigned x = 0; x < lines.size(); x++) {
      lines[x] = true;
    }
    spell_checker_connect_worker();
    spell_dict = dict;
    return true;
  }
  return false;
}

void Document::reset_gui() {
  line_numbers(_conf.get("linenumbers", false));
  std::string df = Utils::katoob_get_default_font();

  if (_conf.get("default_font", true)) {
    Pango::FontDescription fd(df);
    _text_view.modify_font(fd);
  }
  else {
    std::string font = _conf.get("font", df);
    Pango::FontDescription fd(font);
    _text_view.modify_font(fd);
  }

  set_wrap_text(_conf.get("textwrap", true));
  do_undo = _conf.get("undo", true);

  unsigned s = _conf.get("undono", 0);

  if (s > 0) {
    while (s < _undo.size()) {
      KatoobDoElem *_e = _undo.front();
      delete _e;
      _undo.erase(_undo.begin());
    }
    if (_undo.size() == 1) {
      signal_can_undo.emit(true);
    }
  }

  if (s > 0) {
    while (s < _redo.size()) {
      KatoobDoElem *_e = _redo.front();
      delete _e;
      _redo.erase(_redo.begin());
    }
    if (_redo.size() == 1) {
      signal_can_redo.emit(true);
    }
  }

  _label.reset_gui();
  set_tab_width();
}

void Document::set_highlight(const std::string& x) {
  if (_highlight == x) {
    return;
  }

  _highlight = x;

  if (_highlight == "") {
    // none
    Glib::RefPtr<TextBuffer>::cast_dynamic(_text_view.get_buffer())->set_highlight(false);
  }
  else {
    Glib::RefPtr<TextBuffer>::cast_dynamic(_text_view.get_buffer())->set_language(SourceManager::get_language(_highlight));
    Glib::RefPtr<TextBuffer>::cast_dynamic(_text_view.get_buffer())->set_highlight(true);
  }
}

void Document::signal_text_view_request_file_open_cb(std::string filename) {
  signal_text_view_request_file_open.emit(filename);
}

void Document::set_tab_width() {
  // TODO: Cache.
  std::string tab;
  int tab_width = _conf.get("tab_width", 8);
  int counter = 0;
  int width, height;

  while (counter < tab_width) {
    tab += ' ';
    counter++;
  }
  Glib::RefPtr<Pango::Layout> layout = _text_view.create_pango_layout(tab);

  layout->get_pixel_size(width, height);

  Pango::TabArray tab_array(1);
  tab_array.set_tab(0, Pango::TAB_LEFT, width);

  _text_view.set_tabs(tab_array);
}

void Document::autosave() {
  if (ftruncate(_tmp_file_fd, 0) == -1) {
    std::cerr << "Failed to truncate temp file: " << std::strerror(errno) << std::endl;
    return;
  }

  if (lseek(_tmp_file_fd, 0, SEEK_SET) != 0) {
    std::cerr << "Failed to seek in temp file: " << std::strerror(errno) << std::endl;
  }

  if (write(_tmp_file_fd, _text_view.get_buffer()->get_text().c_str(), _text_view.get_buffer()->get_text().size()) == -1) {
    std::cerr << "Failed to write to temp file: " << std::strerror(errno) << std::endl;
  }
}

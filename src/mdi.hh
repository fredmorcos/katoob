/*
 * mdi.hh
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

#include "document.hh"
#include "encodings.hh"
#include "export.hh"
#include "import.hh"
#include "pagesetup.hh"
#include "printsettings.hh"
#include "replacedialog.hh"
#include <gtkmm.h>
#include <vector>

class MDI : public Gtk::Notebook {
public:
  MDI(Conf &, Encodings &);
  ~MDI();

  // Our methods.
  Document *create_document(std::string &, int = -1);
  Document *create_document();
  Document *get_active();
  bool close_all();
  bool close(int n = -1);

  /* Signal callbacks. */
  void create_cb() { create_document(); }
  void open_cb();
  void open_location_cb();
  void save_cb();
  void save_as_cb();
  void save_copy_cb();
  void revert_cb();

  void print_cb();
  void page_setup_cb();
  void print_preview_cb();

  void close_cb();

  void recent_cb(std::string &);

  void undo_cb();
  void redo_cb();
  void cut_cb();
  void copy_cb();
  void paste_cb();
  void erase_cb();
  void select_all_cb();
  void insert_file_cb();

  void set_wrap_text(bool);
  void set_line_numbers(bool);

  void save_all_cb();
  void close_all_cb();

  void set_auto_spell(bool);
  void do_spell();
  bool set_dictionary(std::string &, std::string &);

  void goto_line_cb();
  void goto_line_cb2(int);
  void find(std::string &);
  void find_cb();
  void find_next_cb();
  void replace_cb();

  void execute_cb();

  void signal_switch_page_cb(Gtk::Widget *const &, const guint &);

  bool set_encoding(int, int &);

  void activate(int);

  void import_cb(Import im);
  void export_cb(Export ex);

  /* Our signals. */
  sigc::signal<void> signal_recent_regenerate;
  sigc::signal<void, int> signal_reset_gui;

  // Signals we get from the Document class.
  sigc::signal<void, int, bool> signal_document_modified;
  sigc::signal<void, bool> signal_document_can_undo;
  sigc::signal<void, bool> signal_document_can_redo;
  sigc::signal<void, int, bool> signal_document_readonly;
  sigc::signal<void, std::string &> signal_document_file_changed;
  sigc::signal<void, int, int> signal_document_cursor_moved;
  sigc::signal<void, int> signal_document_encoding_changed;
  sigc::signal<void, bool> signal_document_overwrite_toggled;
  sigc::signal<void, std::string, int> signal_document_title_changed;
  sigc::signal<void, bool> signal_document_wrap_text;
  sigc::signal<void, bool> signal_document_line_numbers;

  sigc::signal<void, std::string> signal_document_dictionary_changed;

  sigc::signal<void, std::string> signal_document_highlight;

  sigc::signal<void, int> signal_doc_activated;
  sigc::signal<void, bool, bool, std::string> signal_document_added;
  sigc::signal<void, int> signal_document_removed;

  sigc::signal<void, bool> signal_document_spell_enabled;

  sigc::signal<void> signal_closed_document_erased;
  sigc::signal<void, std::string> signal_closed_document_added;
  void closed_document_activated_cb(int);

  void reset_gui();

  void signal_extra_button_clicked_cb(std::string);

  void set_highlight(std::string);

  void scan_temp();
  bool autosave();

private:
  void connect_signals(Document *);
  bool replace_dialog_signal_find_cb(ReplaceDialog *);
  void replace_dialog_signal_replace_cb(ReplaceDialog *);
  void replace_dialog_signal_replace_all_cb(ReplaceDialog *);

  // Signal handlers.
  void signal_document_modified_cb(bool);
  void signal_document_can_undo_cb(bool b) { signal_document_can_undo.emit(b); }
  void signal_document_can_redo_cb(bool b) { signal_document_can_redo.emit(b); }
  void signal_document_readonly_cb(bool);
  void signal_document_file_changed_cb(std::string s) {
    signal_document_file_changed.emit(s);
  }
  void signal_document_cursor_moved_cb(int c, int l) {
    signal_document_cursor_moved.emit(c, l);
  }
  void signal_document_encoding_changed_cb(int e) {
    signal_document_encoding_changed.emit(e);
  }
  void signal_document_overwrite_toggled_cb(bool b) {
    signal_document_overwrite_toggled.emit(b);
  }
  void signal_document_title_changed_cb(std::string);

  void signal_transfer_complete_cb(bool, const std::string &, const std::string,
                                   int, bool);
  void signal_dict_transfer_complete_cb(bool, const std::string &, std::string);

  void signal_document_highlight_cb(std::string x) {
    signal_document_highlight.emit(x);
  }

  void signal_document_spell_enabled_cb(bool s) {
    signal_document_spell_enabled.emit(s);
  }
  void signal_dictionary_changed_cb(std::string d) {
    signal_document_dictionary_changed.emit(d);
  }

  void signal_document_wrap_text_cb(bool w) {
    signal_document_wrap_text.emit(w);
  }
  void signal_document_line_numbers_cb(bool ln) {
    signal_document_line_numbers.emit(ln);
  }

  void signal_document_label_close_clicked_cb(Document *);

  void signal_document_dict_lookup_cb(std::string);

  void signal_text_view_request_file_open_cb(std::string);

  void add_document(Document *, bool signals = true);
  bool save();
  bool save(bool);
  bool save(Document *);

  std::vector<Document *> children;
  std::vector<Document *> closed_children;

  Conf &_conf;
  Encodings &_encodings;

  Glib::RefPtr<PageSetup> page_setup;
  Glib::RefPtr<PrintSettings> settings;
};

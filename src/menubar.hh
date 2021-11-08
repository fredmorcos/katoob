/*
 * menubar.hh
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

#include "conf.hh"
#include "encodings.hh"
#include "export.hh"
#include "import.hh"
#include <gtkmm.h>

class DocItem {
 public:
  DocItem(std::string &str, bool ro, bool m): _label(str), _ro(ro), _m(m)
  {
  }
  void set_readonly(bool ro)
  {
    _ro = ro;
  }
  bool get_readonly()
  {
    return _ro;
  }
  void set_modified(bool m)
  {
    _m = m;
  }
  bool get_modified()
  {
    return _m;
  }
  void set_label(std::string &str)
  {
    _label = str;
  }
  const std::string &get_label()
  {
    return _label;
  }

 private:
  std::string _label;
  bool _ro, _m;
};

struct ClosedDocItem {
  int n;
  std::string title;
};

class MenuBar: public Gtk::MenuBar {
 public:
  MenuBar(Conf &,
          Encodings &
#ifdef ENABLE_EMULATOR
          ,
          std::vector<std::string> &
#endif
#ifdef ENABLE_MULTIPRESS
          ,
          std::vector<std::string> &
#endif
  );
  ~MenuBar();

  void create_recent();

  void set_encoding(unsigned);

  void enable_undo(bool);
  void enable_redo(bool);
  void enable_wrap_text(bool);
  void enable_line_numbers(bool);
  void enable_auto_spell(bool);
  void set_read_only(int, bool);
  void reset_gui(bool);
#ifdef ENABLE_HIGHLIGHT
  void set_highlight(std::string);
#endif

  sigc::signal<void> signal_create_activate;
  sigc::signal<void> signal_open_activate;
  sigc::signal<void> signal_open_location_activate;
  sigc::signal<void> signal_save_activate;
  sigc::signal<void> signal_save_as_activate;
  sigc::signal<void> signal_save_copy_activate;
  sigc::signal<void> signal_revert_activate;
#ifdef ENABLE_PRINT
  sigc::signal<void> signal_print_activate;
  sigc::signal<void> signal_print_preview_activate;
  sigc::signal<void> signal_page_setup_activate;
#endif
  sigc::signal<void> signal_close_activate;
  sigc::signal<void> signal_quit_activate;

  sigc::signal<void> signal_undo_activate;
  sigc::signal<void> signal_redo_activate;
  sigc::signal<void> signal_cut_activate;
  sigc::signal<void> signal_copy_activate;
  sigc::signal<void> signal_paste_activate;
  sigc::signal<void> signal_erase_activate;
  sigc::signal<void> signal_select_all_activate;
  sigc::signal<void> signal_insert_file_activate;
  sigc::signal<void> signal_preferences_activate;

  sigc::signal<void> signal_find_activate;
  sigc::signal<void> signal_find_next_activate;
  sigc::signal<void> signal_replace_activate;
  sigc::signal<void> signal_goto_line_activate;

  sigc::signal<void, bool> signal_statusbar_activate;
  sigc::signal<void, bool> signal_wrap_text_activate;
  sigc::signal<void, bool> signal_line_numbers_activate;
  sigc::signal<void, bool> signal_toolbar_activate;
  sigc::signal<void, bool> signal_extended_toolbar_activate;
  sigc::signal<void> signal_text_activate;
  sigc::signal<void> signal_icons_activate;
  sigc::signal<void> signal_both_activate;
  sigc::signal<void> signal_beside_activate;

  sigc::signal<void> signal_execute_activate;
#ifdef ENABLE_SPELL
  sigc::signal<void> signal_spell_activate;
  sigc::signal<void, bool> signal_auto_spell_activate;
#endif

  sigc::signal<void> signal_save_all_activate;
  sigc::signal<void> signal_close_all_activate;

  sigc::signal<void> signal_about_activate;

  sigc::signal<void, std::string &> signal_recent_activate;

  sigc::signal<void, Import> signal_import_activate;
  sigc::signal<void, Export> signal_export_activate;

  sigc::signal<void, int, int> signal_lqyout_activate;
  sigc::signal<void, int> signal_encoding_activate;
  sigc::signal<void, int> signal_document_activate;
  sigc::signal<void, int> signal_closed_document_activate;
#ifdef ENABLE_HIGHLIGHT
  sigc::signal<void, std::string> signal_highlighter_activate;
#endif
#if defined(ENABLE_EMULATOR) || defined(ENABLE_MULTIPRESS)
  sigc::signal<void, int, int> signal_layout_activate;
#endif
  void document_add(std::string &, bool, bool);
  void document_remove(int);
  void document_set_active(int);
  void document_set_modified(int, bool);
  void document_set_readonly(int, bool);
  void document_set_label(int, std::string &);

  void signal_closed_document_erased_cb();
  void signal_closed_document_added(std::string);

  void reset_gui();

  Gtk::Menu get_menu();

 private:
  void file(Conf &);
  void edit(Conf &);
  void search(Conf &);
  void view(Conf &, Encodings &);
  void tools(Conf &
#ifdef ENABLE_EMULATOR
             ,
             std::vector<std::string> &
#endif
#ifdef ENABLE_MULTIPRESS
             ,
             std::vector<std::string> &
#endif
  );
  void documents(Conf &);
  void help(Conf &);
  void recent(Conf &);
  void encodings(Conf &);
#if defined(ENABLE_EMULATOR) || defined(ENABLE_MULTIPRESS)
  void build_submenu(Gtk::Menu *, std::vector<std::string> &, Gtk::RadioButtonGroup &, int);
#endif
#ifdef ENABLE_SPELL
  void signal_auto_spell_activate_cb();
#endif
  void signal_toolbar_activate_cb();
  void signal_extended_toolbar_activate_cb();
  void signal_statusbar_activate_cb();
  void signal_line_numbers_activate_cb();
  void signal_wrap_text_activate_cb();
  void signal_icons_activate_cb();
  void signal_text_activate_cb();
  void signal_beside_activate_cb();
  void signal_both_activate_cb();

  void signal_recent_activate_cb(std::string &);
#if defined(ENABLE_EMULATOR) || defined(ENABLE_MULTIPRESS)
  void signal_layout_activate_cb(int, int);
#endif
  void signal_encoding_activate_cb(unsigned);
  void signal_document_activate_cb(int);
  void signal_closed_document_activate_cb(int);

  void document_set_modified(Gtk::MenuItem &, bool);
  void document_set_readonly(Gtk::MenuItem &, bool);
  void document_set_normal(Gtk::MenuItem &);

  void documents_menu_clear();
  void documents_menu_build();

  void closed_documents_rebuild();

  void signal_import_activate_cb(Import);
  void signal_export_activate_cb(Export);

#ifdef ENABLE_HIGHLIGHT
  void create_highlighters();
  void signal_highlighter_activate_cb(std::string);
#endif /* ENABLE_HIGHLIGHT */

  Gtk::Menu *menu(char *, Gtk::Menu * = NULL);
  Gtk::MenuItem *item(Gtk::Menu *, const Gtk::StockID &, guint, Gdk::ModifierType);
  Gtk::MenuItem *item(Gtk::Menu *, const Gtk::StockID &);
  Gtk::MenuItem *item(Gtk::Menu *, const std::string &);
  Gtk::MenuItem *item(Gtk::Menu *, const std::string &, guint, Gdk::ModifierType);
  Gtk::MenuItem *check_item(Gtk::Menu *, const std::string &);
  Gtk::MenuItem *radio_item(Gtk::Menu *, Gtk::RadioButtonGroup &, const std::string &);

  void separator(Gtk::Menu *);

  Gtk::Menu *file_menu, *edit_menu, *search_menu, *view_menu, *tools_menu, *documents_menu,
      *help_menu, *opened_menu, *closed_menu;

  /* File */
  Gtk::Menu *recent_menu, *_import_menu, *_export_menu, *toolbars_menu, *_encoding_menu;
#ifdef ENABLE_EMULATOR
  Gtk::Menu *_emulator_menu;
#endif
#if defined(ENABLE_EMULATOR) || defined(ENABLE_MULTIPRESS)
  Gtk::Menu *_input_menu;
#endif
#ifdef ENABLE_MULTIPRESS
  Gtk::Menu *_multipress_menu;
#endif
#ifdef ENABLE_HIGHLIGHT
  Gtk::Menu *highlight;
#endif

  Gtk::MenuItem *recent_menu_item;

  Gtk::MenuItem *_create;
  Gtk::MenuItem *_open;
  Gtk::MenuItem *_open_location;
  Gtk::MenuItem *_save;
  Gtk::MenuItem *_save_as;
  Gtk::MenuItem *_save_copy;
  Gtk::MenuItem *_revert;
#ifdef ENABLE_PRINT
  Gtk::MenuItem *_print;
  Gtk::MenuItem *_print_preview;
  Gtk::MenuItem *_page_setup;
#endif
  Gtk::MenuItem *_close;
  Gtk::MenuItem *_quit;
  /* Edit */
  Gtk::MenuItem *_undo;
  Gtk::MenuItem *_redo;
  Gtk::MenuItem *_cut;
  Gtk::MenuItem *_copy;
  Gtk::MenuItem *_paste;
  Gtk::MenuItem *_erase;
  Gtk::MenuItem *_select_all;
  Gtk::MenuItem *_preferences;
  Gtk::MenuItem *_insert_file;
  /* Search */
  Gtk::MenuItem *_find;
  Gtk::MenuItem *_find_next;
  Gtk::MenuItem *_replace;
  Gtk::MenuItem *_goto_line;
  /* View */
  Gtk::MenuItem *_statusbar;
  Gtk::MenuItem *_wrap_text;
  Gtk::MenuItem *_line_numbers;
  Gtk::MenuItem *_toolbar;
  Gtk::MenuItem *_extended_toolbar;
  Gtk::MenuItem *_text;
  Gtk::MenuItem *_icons;
  Gtk::MenuItem *_both;
  Gtk::MenuItem *_beside;

  /* Tools */
  Gtk::MenuItem *_execute;
#ifdef ENABLE_SPELL
  Gtk::MenuItem *_spell;
  Gtk::MenuItem *_auto_spell;
#endif
  /* Documents */
  Gtk::MenuItem *_save_all;
  Gtk::MenuItem *_close_all;
  /* Help */
  Gtk::MenuItem *_about;

  bool _ignore_encoding_changed_signal_hack;
  bool _ignore_document_clicked_signal_hack;
  bool _ignore_highlighting_changed_signal_hack;
  std::vector<Gtk::MenuItem *> encoding_menu_items;

  Conf &_conf;
  std::vector<DocItem> _documents;
  std::vector<ClosedDocItem> _closed_documents;
  int _active;
};

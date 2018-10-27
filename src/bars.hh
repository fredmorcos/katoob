/*
 * bars.hh
 * This file is part of katoob
 *
 * Copyright (C) 2008 Mohammed Sameer
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

#ifndef __BARS_HH__
#define __BARS_HH__

#include <sigc++/signal.h>
#include <glibmm/refptr.h>
#include <gtkmm/uimanager.h>
#include <gtkmm/actiongroup.h>
#include <gtkmm/stock.h>
//#include <sigc++/functor.h>

// forward declaration
class Conf;
class Encodings;
class SpellMenu;
class Document;
class Import;
class Export;
namespace Gtk { class Widget; class Toolbar; class Entry; class Label; }

class Bars : public sigc::trackable {
public:
  Bars(Conf *) throw();
  ~Bars();

  Gtk::Widget *get_menubar();
  Gtk::Widget *get_toolbar();
  Gtk::Widget *get_toolbar_extended();

  void set_encodings(Encodings *);

  void set_import(std::vector<Import *>&);
  void set_export(std::vector<Export *>&);

  void enable_spell(const std::string&, bool);
  void set_encoding(unsigned);
  void set_rom(int, bool, bool);
  void set_label(int, std::string&);
  void reset_widgets(bool);
  void enable_redo(bool);
  void enable_undo(bool);

  void enable_text_wrap(bool);
  void enable_line_numbers(bool);
  void reset_ui();
  void set_spell_dict(const std::string&);

  void set_active_document(int);

#ifdef ENABLE_HIGHLIGHT
  void set_highlight(std::string);
#endif

  Glib::RefPtr<Gtk::AccelGroup> get_accel_group();

  sigc::signal<void> signal_new;
  sigc::signal<void> signal_open;
  sigc::signal<void> signal_open_location;
  sigc::signal<void> signal_save;
  sigc::signal<void> signal_print;
  sigc::signal<void> signal_close;
  sigc::signal<void> signal_undo;
  sigc::signal<void> signal_redo;
  sigc::signal<void> signal_cut;
  sigc::signal<void> signal_copy;
  sigc::signal<void> signal_paste;
  sigc::signal<void> signal_delete;
  sigc::signal<void> signal_save_as;
  sigc::signal<void> signal_revert;
  sigc::signal<void> signal_quit;
  sigc::signal<void> signal_spell;
  sigc::signal<void> signal_find;
  sigc::signal<void> signal_find_next;
  sigc::signal<void> signal_find_replace;
  sigc::signal<void> signal_about;
  sigc::signal<void> signal_print_preview;
  sigc::signal<void> signal_page_setup;
  sigc::signal<void> signal_save_copy;
  sigc::signal<void> signal_select_all;
  sigc::signal<void> signal_save_all;
  sigc::signal<void> signal_close_all;
  sigc::signal<void> signal_goto;
  sigc::signal<void> signal_preferences;
  sigc::signal<void> signal_insert_file;
  sigc::signal<void> signal_execute;
  sigc::signal<void> signal_main_toolbar;
  sigc::signal<void> signal_extended_toolbar;
#ifdef ENABLE_MAEMO
  sigc::signal<void> signal_full_screen;
#endif

  sigc::signal<void, unsigned> signal_encoding;
  sigc::signal<void, unsigned> signal_open_document;
  sigc::signal<void, unsigned> signal_closed_document;

  sigc::signal<void, bool> signal_auto_spell;
  sigc::signal<void, bool> signal_statusbar;
  sigc::signal<void, bool> signal_wrap_text;
  sigc::signal<void, bool> signal_line_numbers;

  sigc::signal<void, int> signal_goto_line;
#ifdef ENABLE_HIGHLIGHT
  sigc::signal<void, std::string> signal_highlight;
#endif
  sigc::signal<void, std::string> signal_spell_dict_changed;
  sigc::signal<void, std::string> signal_extra_button;
  sigc::signal<void, std::string> signal_search_string;

  sigc::signal<void, std::string> signal_recent;

  sigc::signal<Document *> signal_request_active;

  sigc::signal<void, Import *> signal_import;
  sigc::signal<void, Export *> signal_export;

private:
  typedef struct _MenuItem {
    const char *id;
    const char *name;
  } MenuItem;

  typedef struct _StockItem {
    const char *name;
    const Gtk::BuiltinStockID *stock;
    sigc::slot<void> signal;
    bool generic;
    const char *accelerator;
  } StockItem;

  typedef struct _Item {
    bool generic;
    const char *id;
    const char *name;
    sigc::slot<void> signal;
    const char *accelerator;
  } Item;

  void add_actions();

  MenuItem *get_menubar_items();
  StockItem *get_stock_items();
  Item *get_items();
  Item *get_check_items();

  std::map<std::string, std::string> get_tooltips();

  void create_item(StockItem *);
  void create_item(Item *);
  void create_check_item(Item *);
  void add_to_action_group(Glib::RefPtr<Gtk::ActionGroup>&, Glib::RefPtr<Gtk::Action>, sigc::slot<void>&, const Gtk::AccelKey& = Gtk::AccelKey());

#ifdef ENABLE_HIGHLIGHT
  void build_highlight();
  void highlight_activate(std::string, std::string);
#endif

  void build_extended();
  void build_extra_buttons(Gtk::Toolbar *);
  void create_extra_button(Gtk::Toolbar *, std::string&);

  void create_recent();
  void create_encodings();

  void create_open_documents(Document *);
  void create_closed_documents();

  //  void create_documents(int&, const std::string&, Glib::RefPtr<Gtk::ActionGroup>&, sigc::signal<void, unsigned>&, std::vector<DocItem>&, bool);

  void toolbar_text();
  void toolbar_icons();
  void toolbar_both();
  void toolbar_both_horiz();

  void search_activate(Gtk::Entry *);
  void goto_activate(Gtk::Entry *);
  void spell_dict_changed(SpellMenu *);

  void encoding_clicked(unsigned);

  void statusbar_toggled();
  void wrap_text_toggled();
  void line_numbers_toggled();
  void auto_spell_toggled();

  void toolbar_main();
  void toolbar_extended();

  void recent_item_activate(std::string);
  void closed_item_activate(int);
  void color_label(Gtk::Label *, bool, bool);

  void set_active(const Glib::RefPtr<Gtk::Action>&, bool = true);
  void set_toolbar_style(const std::string&, const std::string&, const Gtk::ToolbarStyle&);
  void emit_signal(sigc::signal<void, bool>&, const Glib::RefPtr<Gtk::Action>&);
  void toolbar_toggle(const std::string&, const std::string&, const std::string&);

  sigc::connection recent_conn;

  Conf *_conf;
  Glib::RefPtr<Gtk::UIManager> ui;

  Glib::RefPtr<Gtk::ActionGroup> generic, document, menubar, recent, encodings, open, closed, import, exprt;
#ifdef ENABLE_HIGHLIGHT
  Glib::RefPtr<Gtk::ActionGroup> highlight;
#endif

  Encodings *_encodings;

  SpellMenu *sm;

  Gtk::Toolbar *extra_buttons;

  int recent_merge_id, open_merge_id, closed_merge_id;
};

#endif /* __BAR_HH__ */

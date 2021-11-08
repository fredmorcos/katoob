/*
 * menubar.cc
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

#include "macros.h"
#include "menubar.hh"
#include "utils.hh"
#include <cassert>
#include <gdk/gdkkeysyms.h>
#include <gdkmm.h>
#include <gtkmm.h>
#include <libgen.h>

#ifdef ENABLE_HIGHLIGHT
#include "sourcemanager.hh"
#endif

// TODO: Integrate with the gtk recent files thing.
MenuBar::MenuBar(Conf &config,
                 Encodings &encodings
#ifdef ENABLE_EMULATOR
                 ,
                 std::vector<std::string> &em
#endif
#ifdef ENABLE_MULTIPRESS
                 ,
                 std::vector<std::string> &mp
#endif
                 ):
 _conf(config)
{
  file();
  edit();
  search();
  view(encodings);
  tools(
#ifdef ENABLE_EMULATOR

      em
#endif
#ifdef ENABLE_MULTIPRESS
      ,
      mp
#endif
  );
  documents();
  help();
#ifdef ENABLE_HIGHLIGHT
  create_highlighters();
#endif
  show_all();
  recent_menu_item->hide();
}

MenuBar::~MenuBar()
{
}

Gtk::Menu *MenuBar::menu(char *str, Gtk::Menu *menu)
{
  Gtk::Menu *m = Gtk::manage(new Gtk::Menu());
  if (menu) {
    menu->items().push_back(Gtk::Menu_Helpers::MenuElem(str, *m));
  } else {
    items().push_back(Gtk::Menu_Helpers::MenuElem(str, *m));
  }
  return m;
}

Gtk::MenuItem *
MenuBar::radio_item(Gtk::Menu *menu, Gtk::RadioButtonGroup &group, const std::string &str)
{
  // I'm doing this by hand to disable the mnemonic

  Gtk::RadioMenuItem *item = manage(new Gtk::RadioMenuItem(group, str));

  menu->items().push_back(*item);
  // Gtk::Menu_Helpers::RadioMenuElem(group, str));

  //  return &menu->items().back();
  //  delete item;

  return &menu->items().back();
  //  return item;
}

Gtk::MenuItem *MenuBar::check_item(Gtk::Menu *menu, const std::string &str)
{
  menu->items().push_back(Gtk::Menu_Helpers::CheckMenuElem(str));
  return &menu->items().back();
}

Gtk::MenuItem *MenuBar::item(Gtk::Menu *menu, const Gtk::StockID &stock_id)
{
  menu->items().push_back(Gtk::Menu_Helpers::StockMenuElem(stock_id));
  return &menu->items().back();
}

Gtk::MenuItem *MenuBar::item(Gtk::Menu *menu, const std::string &str)
{
  menu->items().push_back(Gtk::Menu_Helpers::MenuElem(str));
  return &menu->items().back();
}

Gtk::MenuItem *
MenuBar::item(Gtk::Menu *menu, const Gtk::StockID &stock_id, guint key, Gdk::ModifierType mod)
{
  Gtk::AccelKey _key(key, mod);
  menu->items().push_back(Gtk::Menu_Helpers::StockMenuElem(stock_id, _key));
  return &menu->items().back();
}

Gtk::MenuItem *
MenuBar::item(Gtk::Menu *menu, const std::string &str, guint key, Gdk::ModifierType mod)
{
  Gtk::AccelKey _key(key, mod);
  menu->items().push_back(Gtk::Menu_Helpers::MenuElem(str, _key));
  return &menu->items().back();
}

void MenuBar::separator(Gtk::Menu *menu)
{
  menu->items().push_back(Gtk::Menu_Helpers::SeparatorElem());
}

void MenuBar::file()
{
  file_menu = menu(_("_File"));

  _create = item(file_menu, Gtk::Stock::NEW);
  _create->signal_activate().connect(
      sigc::mem_fun(signal_create_activate, &sigc::signal<void>::emit));

  _open = item(file_menu, Gtk::Stock::OPEN);
  _open->signal_activate().connect(sigc::mem_fun(signal_open_activate, &sigc::signal<void>::emit));

  _open_location = item(file_menu, _("Open _Location..."));
  _open_location->signal_activate().connect(
      sigc::mem_fun(signal_open_location_activate, &sigc::signal<void>::emit));

  /* Recent */
  recent_menu = menu(_("Recent"), file_menu);
  recent_menu_item = &file_menu->items().back();

  separator(file_menu);
  _save = item(file_menu, Gtk::Stock::SAVE);
  _save->signal_activate().connect(sigc::mem_fun(signal_save_activate, &sigc::signal<void>::emit));

  _save_as = item(file_menu,
                  Gtk::Stock::SAVE_AS,
                  GDK_s,
                  Gdk::ModifierType(GDK_CONTROL_MASK | GDK_SHIFT_MASK));
  _save_as->signal_activate().connect(
      sigc::mem_fun(signal_save_as_activate, &sigc::signal<void>::emit));

  _save_copy = item(file_menu, _("Sa_ve Copy..."));
  _save_copy->signal_activate().connect(
      sigc::mem_fun(signal_save_copy_activate, &sigc::signal<void>::emit));

  _revert = item(file_menu, Gtk::Stock::REVERT_TO_SAVED);
  _revert->signal_activate().connect(
      sigc::mem_fun(signal_revert_activate, &sigc::signal<void>::emit));

  separator(file_menu);
  _import_menu = menu(_("_Import"), file_menu);

  std::vector<Import> imports;
  import_init(imports);
  for (unsigned x = 0; x < imports.size(); x++) {
    item(_import_menu, imports[x].name)
        ->signal_activate()
        .connect(sigc::bind<Import>(sigc::mem_fun(this, &MenuBar::signal_import_activate_cb),
                                    imports[x]));
  }

  _export_menu = menu(_("_Export"), file_menu);
  std::vector<Export> exports;
  export_init(exports);
  for (unsigned x = 0; x < exports.size(); x++) {
    item(_export_menu, exports[x].name)
        ->signal_activate()
        .connect(sigc::bind<Export>(sigc::mem_fun(this, &MenuBar::signal_export_activate_cb),
                                    exports[x]));
  }

#ifdef ENABLE_PRINT
  separator(file_menu);
  _page_setup = item(file_menu, _("Page Set_up..."));
  _page_setup->signal_activate().connect(
      sigc::mem_fun(signal_page_setup_activate, &sigc::signal<void>::emit));

  _print_preview = item(file_menu,
                        _("Print Previe_w"),
                        GDK_p,
                        Gdk::ModifierType(GDK_CONTROL_MASK | GDK_SHIFT_MASK));
  _print_preview->signal_activate().connect(
      sigc::mem_fun(signal_print_preview_activate, &sigc::signal<void>::emit));

  _print = item(file_menu, Gtk::Stock::PRINT, GDK_p, Gdk::ModifierType(GDK_CONTROL_MASK));
  _print->signal_activate().connect(
      sigc::mem_fun(signal_print_activate, &sigc::signal<void>::emit));
#endif
  separator(file_menu);
  _close = item(file_menu, Gtk::Stock::CLOSE);
  _close->signal_activate().connect(
      sigc::mem_fun(signal_close_activate, &sigc::signal<void>::emit));

  _quit = item(file_menu, Gtk::Stock::QUIT);
  _quit->signal_activate().connect(sigc::mem_fun(signal_quit_activate, &sigc::signal<void>::emit));

  // We are not calling this because our reset_gui() will be called
  // And it'll call this.
  //  create_recent();
}

void MenuBar::create_recent()
{
  if (!_conf.get("recent", true)) {
    return;
  }

  recent_menu->items().clear();
  unsigned recentno = _conf.get("recentno", 10);
  recentno = recentno > _conf.get_recent().size() ? _conf.get_recent().size() : recentno;
  std::string base;

  for (unsigned x = 0; x < recentno; x++) {
#ifdef GLIBMM_EXCEPTIONS_ENABLED
    try {
      base = Glib::filename_to_utf8(Glib::path_get_basename(_conf.get_recent()[x]));
    } catch (...) {
      base = Glib::filename_display_basename(_conf.get_recent()[x]);
    }
#else
    // Note: We will skip non valid files for now.
    std::auto_ptr<Glib::Error> error;
    base = Glib::filename_to_utf8(Glib::path_get_basename(_conf.get_recent()[x]), error);
    if (error.get()) {
      base = Glib::filename_display_basename(_conf.get_recent()[x]);
    }
#endif
    Gtk::MenuItem *_item = item(recent_menu, base);
    _item->signal_activate().connect(
        sigc::bind<std::string>(sigc::mem_fun(*this, &MenuBar::signal_recent_activate_cb),
                                _conf.get_recent()[x]));
  }
}

void MenuBar::edit()
{
  edit_menu = menu(_("_Edit"));
  _undo = item(edit_menu, Gtk::Stock::UNDO, GDK_z, Gdk::ModifierType(GDK_CONTROL_MASK));
  _undo->signal_activate().connect(sigc::mem_fun(signal_undo_activate, &sigc::signal<void>::emit));

  _redo = item(edit_menu,
               Gtk::Stock::REDO,
               GDK_z,
               Gdk::ModifierType(GDK_CONTROL_MASK | GDK_SHIFT_MASK));
  _redo->signal_activate().connect(sigc::mem_fun(signal_redo_activate, &sigc::signal<void>::emit));

  separator(edit_menu);
  _cut = item(edit_menu, Gtk::Stock::CUT);
  _cut->signal_activate().connect(sigc::mem_fun(signal_cut_activate, &sigc::signal<void>::emit));

  _copy = item(edit_menu, Gtk::Stock::COPY);
  _copy->signal_activate().connect(sigc::mem_fun(signal_copy_activate, &sigc::signal<void>::emit));

  _paste = item(edit_menu, Gtk::Stock::PASTE);
  _paste->signal_activate().connect(
      sigc::mem_fun(signal_paste_activate, &sigc::signal<void>::emit));

  _erase = item(edit_menu, Gtk::Stock::DELETE, GDK_d, Gdk::ModifierType(GDK_CONTROL_MASK));
  _erase->signal_activate().connect(
      sigc::mem_fun(signal_erase_activate, &sigc::signal<void>::emit));

  separator(edit_menu);
  _select_all = item(edit_menu, _("_Select All"), GDK_a, Gdk::ModifierType(GDK_CONTROL_MASK));
  _select_all->signal_activate().connect(
      sigc::mem_fun(signal_select_all_activate, &sigc::signal<void>::emit));

  separator(edit_menu);
  _insert_file = item(edit_menu, _("_Insert File"));
  _insert_file->signal_activate().connect(
      sigc::mem_fun(signal_insert_file_activate, &sigc::signal<void>::emit));

  separator(edit_menu);
  _preferences = item(edit_menu, Gtk::Stock::PREFERENCES);
  _preferences->signal_activate().connect(
      sigc::mem_fun(signal_preferences_activate, &sigc::signal<void>::emit));
}

void MenuBar::search()
{
  search_menu = menu(_("_Search"));

  _find = item(search_menu, Gtk::Stock::FIND);
  _find->signal_activate().connect(sigc::mem_fun(signal_find_activate, &sigc::signal<void>::emit));

  _find_next = item(search_menu, _("Find Ne_xt"), GDK_g, Gdk::ModifierType(GDK_CONTROL_MASK));
  _find_next->signal_activate().connect(
      sigc::mem_fun(signal_find_next_activate, &sigc::signal<void>::emit));

  _replace = item(search_menu, Gtk::Stock::FIND_AND_REPLACE);
  _replace->signal_activate().connect(
      sigc::mem_fun(signal_replace_activate, &sigc::signal<void>::emit));

  separator(search_menu);
  _goto_line = item(search_menu, Gtk::Stock::JUMP_TO);
  _goto_line->signal_activate().connect(
      sigc::mem_fun(signal_goto_line_activate, &sigc::signal<void>::emit));
}

void MenuBar::view(Encodings &encodings)
{
  Gtk::RadioButtonGroup toolbars, encoding;
  view_menu = menu(_("_View"));

  _statusbar = check_item(view_menu, _("_Statusbar"));
  _statusbar->signal_activate().connect(
      sigc::mem_fun(*this, &MenuBar::signal_statusbar_activate_cb));

  _line_numbers = check_item(view_menu, _("_Line Numbers"));
  _line_numbers->signal_activate().connect(
      sigc::mem_fun(*this, &MenuBar::signal_line_numbers_activate_cb));

  _wrap_text = check_item(view_menu, _("_Wrap Text"));
  _wrap_text->signal_activate().connect(
      sigc::mem_fun(*this, &MenuBar::signal_wrap_text_activate_cb));

  separator(view_menu);

  toolbars_menu = menu(_("Toolbars"), view_menu);

  _toolbar = check_item(toolbars_menu, _("_Main Toolbar"));
  _toolbar->signal_activate().connect(sigc::mem_fun(*this, &MenuBar::signal_toolbar_activate_cb));

  _extended_toolbar = check_item(toolbars_menu, _("_Extended Toolbar"));
  _extended_toolbar->signal_activate().connect(
      sigc::mem_fun(*this, &MenuBar::signal_extended_toolbar_activate_cb));

  separator(toolbars_menu);
  // TODO: Disable these when the main toolbar is not shown.
  _icons = radio_item(toolbars_menu, toolbars, _("Icons only"));
  _icons->signal_activate().connect(sigc::mem_fun(*this, &MenuBar::signal_icons_activate_cb));

  _text = radio_item(toolbars_menu, toolbars, _("Text only"));
  _text->signal_activate().connect(sigc::mem_fun(*this, &MenuBar::signal_text_activate_cb));

  _both = radio_item(toolbars_menu, toolbars, _("Both"));
  _both->signal_activate().connect(sigc::mem_fun(*this, &MenuBar::signal_both_activate_cb));

  _beside = radio_item(toolbars_menu, toolbars, _("Text beside icons"));
  _beside->signal_activate().connect(sigc::mem_fun(*this, &MenuBar::signal_beside_activate_cb));

  _encoding_menu = menu(_("_Encoding"), view_menu);
  _encoding_menu->items().push_back(Gtk::Menu_Helpers::TearoffMenuElem());

  Gtk::Menu *item;
  int e = 0;
  for (int x = 0; x < encodings.languages(); x++) {
    item = menu(const_cast<char *>(encodings.language(x).c_str()), _encoding_menu);
    for (int y = 0; y < encodings.languages(x); y++) {
      Gtk::MenuItem *_item = radio_item(item, encoding, encodings.name(x, y));
      _item->signal_activate().connect(
          sigc::bind<int>(sigc::mem_fun(*this, &MenuBar::signal_encoding_activate_cb), e++));
      encoding_menu_items.push_back(_item);
    }
  }
}

void MenuBar::tools(
#ifdef ENABLE_EMULATOR
    std::vector<std::string> &em
#endif
#ifdef ENABLE_MULTIPRESS
    ,
    std::vector<std::string> &mp
#endif
)
{
  tools_menu = menu(_("_Tools"));
  _execute = item(tools_menu,
                  _("_Execute Command On Buffer..."),
                  GDK_e,
                  Gdk::ModifierType(GDK_CONTROL_MASK));
  _execute->signal_activate().connect(
      sigc::mem_fun(signal_execute_activate, &sigc::signal<void>::emit));
#if defined(ENABLE_EMULATOR) || defined(ENABLE_MULTIPRESS) || defined(ENABLE_SPELL)
  separator(tools_menu);
#endif

#ifdef ENABLE_SPELL
  _spell = item(tools_menu, Gtk::Stock::SPELL_CHECK, GDK_F7, Gdk::ModifierType(GDK_CONTROL_MASK));
  _spell->signal_activate().connect(
      sigc::mem_fun(signal_spell_activate, &sigc::signal<void>::emit));

  _auto_spell = check_item(tools_menu, _("_Autocheck Spelling"));
  _auto_spell->signal_activate().connect(
      sigc::mem_fun(*this, &MenuBar::signal_auto_spell_activate_cb));
#endif

#if defined(ENABLE_EMULATOR) || defined(ENABLE_MULTIPRESS)
  Gtk::RadioButtonGroup input_group;
  _input_menu = menu(_("Input"), tools_menu);
  Gtk::MenuItem *item = radio_item(_input_menu, input_group, _("Default"));
  dynamic_cast<Gtk::RadioMenuItem *>(item)->set_active(true);
  item->signal_activate().connect(
      sigc::bind<int, int>(sigc::mem_fun(*this, &MenuBar::signal_layout_activate_cb), -1, -1));
#endif

#ifdef ENABLE_EMULATOR
  _emulator_menu = menu(_("Keyboard emulator"), _input_menu);
  build_submenu(_emulator_menu, em, input_group, 0);
#endif

#ifdef ENABLE_MULTIPRESS
  _multipress_menu = menu(_("Multipress"), _input_menu);
  build_submenu(_multipress_menu, mp, input_group, 1);
#endif
}

#if defined(ENABLE_EMULATOR) || defined(ENABLE_MULTIPRESS)
void MenuBar::build_submenu(Gtk::Menu *menu,
                            std::vector<std::string> &items,
                            Gtk::RadioButtonGroup &group,
                            int num)
{
  for (std::size_t x = 0; x < items.size(); x++) {
    Gtk::MenuItem *_item = radio_item(menu, group, items[x]);
    auto callback = sigc::mem_fun(*this, &MenuBar::signal_layout_activate_cb);
    auto binding = sigc::bind<int, std::size_t>(callback, num, x);
    _item->signal_activate().connect(binding);
  }
}
#endif

void MenuBar::documents()
{
  documents_menu = menu(_("_Documents"));
  _save_all = item(documents_menu, _("Save _All"));
  _save_all->signal_activate().connect(
      mem_fun(signal_save_all_activate, &sigc::signal<void>::emit));

  _close_all = item(documents_menu,
                    _("Clos_e All"),
                    GDK_w,
                    Gdk::ModifierType(GDK_CONTROL_MASK | GDK_SHIFT_MASK));
  _close_all->signal_activate().connect(
      mem_fun(signal_close_all_activate, &sigc::signal<void>::emit));

  separator(documents_menu);

  opened_menu = menu(_("_Opened Documents"), documents_menu);
  closed_menu = menu(_("_Closed Documents"), documents_menu);
  documents_menu->show_all();
}

void MenuBar::help()
{
  help_menu = menu(_("_Help"));
  _about = item(help_menu, Gtk::Stock::ABOUT, GDK_t, Gdk::ModifierType(GDK_CONTROL_MASK));
  _about->signal_activate().connect(mem_fun(signal_about_activate, &sigc::signal<void>::emit));
}

#if defined(ENABLE_EMULATOR) || defined(ENABLE_MULTIPRESS)
void MenuBar::signal_layout_activate_cb(int x, std::size_t y)
{
  signal_layout_activate.emit(x, y);
}
#endif

void MenuBar::set_encoding(unsigned e)
{
  assert(e <= encoding_menu_items.size());

  _ignore_encoding_changed_signal_hack = true;

  Gtk::MenuItem *item = encoding_menu_items[e];
  dynamic_cast<Gtk::CheckMenuItem *>(item)->set_active(true);
  _ignore_encoding_changed_signal_hack = false;
}

void MenuBar::signal_encoding_activate_cb(unsigned e)
{
  assert(e <= encoding_menu_items.size());

  if (_ignore_encoding_changed_signal_hack) {
    return;
  }

  Gtk::MenuItem *item = encoding_menu_items[e];
  if (dynamic_cast<Gtk::CheckMenuItem *>(item)->get_active()) {
    signal_encoding_activate.emit(e);
  }
}

void MenuBar::signal_document_activate_cb(int x)
{
  if (_ignore_document_clicked_signal_hack) {
    return;
  }

  if (static_cast<Gtk::RadioMenuItem &>(opened_menu->items()[x]).get_active()) {
    signal_document_activate.emit(x);
  }
}

void MenuBar::document_add(std::string &label, bool ro, bool m)
{
  DocItem item(label, ro, m);
  _documents.push_back(item);

  documents_menu_clear();
  documents_menu_build();

  MenuList &ml = opened_menu->items();
  MenuList::iterator iter = ml.end();
  iter--;

  _ignore_document_clicked_signal_hack = true;

  Gtk::MenuItem &itm = *iter;
  static_cast<Gtk::RadioMenuItem &>(itm).set_active(true);

  _ignore_document_clicked_signal_hack = false;
}

void MenuBar::document_set_active(std::size_t x)
{
  _active = x;
  if (_active >= opened_menu->items().size()) {
    return;
  }

  _ignore_document_clicked_signal_hack = true;

  Gtk::MenuItem &item = opened_menu->items()[x];
  static_cast<Gtk::RadioMenuItem &>(item).set_active(true);
  // For some strange reason, We lose the colors when we switch tabs.
  // I'm setting them here.
  document_set_modified(x, _documents[x].get_modified());
  document_set_readonly(x, _documents[x].get_readonly());

  _ignore_document_clicked_signal_hack = false;
}

void MenuBar::document_remove(std::size_t x)
{
  documents_menu_clear();

  auto iter = _documents.begin() + x;
  _documents.erase(iter);

  documents_menu_build();
}

void MenuBar::document_set_label(int x, std::string &str)
{
  _documents[x].set_label(str);
  dynamic_cast<Gtk::Label *>(opened_menu->items()[x].get_child())->set_text(str);
}

void MenuBar::document_set_modified(std::size_t x, bool m)
{
  assert(x < _documents.size());

  // A read only document can't be modified.
  if (_documents[x].get_readonly()) {
    return;
  }

  _documents[x].set_modified(m);
  document_set_modified(opened_menu->items()[x], m);
}

void MenuBar::document_set_readonly(std::size_t x, bool ro)
{
  assert(x < _documents.size());

  // A modified document can't be read only.
  if (_documents[x].get_modified()) {
    return;
  }

  _documents[x].set_readonly(ro);

  document_set_readonly(opened_menu->items()[x], ro);
}

void MenuBar::document_set_readonly(Gtk::MenuItem &item, bool ro)
{
  if (ro) {
    katoob_set_color(_conf,
                     dynamic_cast<Gtk::Label *>(item.get_child()),
                     Utils::KATOOB_COLOR_READONLY);
  } else {
    document_set_normal(item);
  }
}

void MenuBar::document_set_modified(Gtk::MenuItem &item, bool m)
{
  if (m)
    katoob_set_color(_conf,
                     dynamic_cast<Gtk::Label *>(item.get_child()),
                     Utils::KATOOB_COLOR_MODIFIED);
  else
    document_set_normal(item);
}

void MenuBar::document_set_normal(Gtk::MenuItem &item)
{
  katoob_set_color(_conf, dynamic_cast<Gtk::Label *>(item.get_child()), Utils::KATOOB_COLOR_NORMAL);
}

void MenuBar::documents_menu_clear()
{
  opened_menu->items().clear();
}

void MenuBar::documents_menu_build()
{
  Gtk::RadioButtonGroup documents_radio;

  for (unsigned i = 0; i < _documents.size(); i++) {
    Gtk::MenuItem *_item = radio_item(opened_menu, documents_radio, _documents[i].get_label());
    _item->signal_activate().connect(
        sigc::bind<int>(sigc::mem_fun(*this, &MenuBar::signal_document_activate_cb), i));

    Gtk::MenuItem &itm = *_item;

    bool r = _documents[i].get_readonly();
    bool m = _documents[i].get_modified();
    if (r)
      document_set_readonly(itm, true);
    if (m)
      document_set_modified(itm, true);
    if ((!r) && (!m))
      document_set_normal(itm);
    _item->show();
  }
}

void MenuBar::signal_closed_document_erased_cb()
{
  // This is called when we remove an item from the beginning.
  _closed_documents.erase(_closed_documents.begin());
  closed_documents_rebuild();
}

void MenuBar::signal_closed_document_added(std::string title)
{
  // We will add an item to the end.
  ClosedDocItem doc;
  doc.title = title;
  doc.n = _closed_documents.size() + 1;
  _closed_documents.push_back(doc);
  closed_documents_rebuild();
}

void MenuBar::closed_documents_rebuild()
{
  closed_menu->items().clear();

  Gtk::MenuItem *_item;
  for (unsigned i = 0; i < _closed_documents.size(); i++) {
    _item = item(closed_menu, _closed_documents[i].title);
    _item->signal_activate().connect(
        sigc::bind<int>(sigc::mem_fun(*this, &MenuBar::signal_closed_document_activate_cb), i));
  }
}

void MenuBar::signal_closed_document_activate_cb(int n)
{
  std::vector<ClosedDocItem>::iterator iter = _closed_documents.begin();
  iter += n;
  _closed_documents.erase(iter);
  closed_documents_rebuild();
  signal_closed_document_activate.emit(n);
}

void MenuBar::signal_import_activate_cb(Import imp)
{
  signal_import_activate.emit(imp);
}

void MenuBar::signal_export_activate_cb(Export exp)
{
  signal_export_activate.emit(exp);
}

void MenuBar::reset_gui()
{
  dynamic_cast<Gtk::CheckMenuItem *>(_line_numbers)->set_active(_conf.get("linenumbers", true));

  dynamic_cast<Gtk::CheckMenuItem *>(_statusbar)->set_active(_conf.get("statusbar", true));
  dynamic_cast<Gtk::CheckMenuItem *>(_wrap_text)->set_active(_conf.get("textwrap", true));

  dynamic_cast<Gtk::CheckMenuItem *>(_toolbar)->set_active(_conf.get("toolbar", true));
  dynamic_cast<Gtk::CheckMenuItem *>(_extended_toolbar)
      ->set_active(_conf.get("extended_toolbar", true));
#ifdef ENABLE_SPELL
  dynamic_cast<Gtk::CheckMenuItem *>(_auto_spell)->set_active(_conf.get("spell_check", true));
#endif

  const std::string &toolbartype = _conf.get("toolbartype", "both");
  if (toolbartype == "text") {
    dynamic_cast<Gtk::RadioMenuItem *>(_text)->set_active();
  } else if (toolbartype == "icons") {
    dynamic_cast<Gtk::RadioMenuItem *>(_icons)->set_active();
  } else if (toolbartype == "both_horiz") {
    dynamic_cast<Gtk::RadioMenuItem *>(_beside)->set_active();
  } else {
    dynamic_cast<Gtk::RadioMenuItem *>(_both)->set_active();
  }

  if (_conf.get("recent", true)) {
    recent_menu_item->show();
  } else {
    recent_menu_item->hide();
  }

  create_recent();

  documents_menu_clear();
  documents_menu_build();
  document_set_active(_active);
}

#ifdef ENABLE_SPELL
void MenuBar::signal_auto_spell_activate_cb()
{
  signal_auto_spell_activate.emit(dynamic_cast<Gtk::CheckMenuItem *>(_auto_spell)->get_active());
}
#endif
void MenuBar::signal_toolbar_activate_cb()
{
  signal_toolbar_activate.emit(dynamic_cast<Gtk::CheckMenuItem *>(_toolbar)->get_active());
}

void MenuBar::signal_extended_toolbar_activate_cb()
{
  signal_extended_toolbar_activate.emit(
      dynamic_cast<Gtk::CheckMenuItem *>(_extended_toolbar)->get_active());
}

void MenuBar::signal_statusbar_activate_cb()
{
  signal_statusbar_activate.emit(dynamic_cast<Gtk::CheckMenuItem *>(_statusbar)->get_active());
}

void MenuBar::signal_line_numbers_activate_cb()
{
  signal_line_numbers_activate.emit(
      dynamic_cast<Gtk::CheckMenuItem *>(_line_numbers)->get_active());
}

void MenuBar::signal_wrap_text_activate_cb()
{
  signal_wrap_text_activate.emit(dynamic_cast<Gtk::CheckMenuItem *>(_wrap_text)->get_active());
}

void MenuBar::signal_icons_activate_cb()
{
  if (dynamic_cast<Gtk::CheckMenuItem *>(_icons)->get_active()) {
    signal_icons_activate.emit();
  }
}

void MenuBar::signal_text_activate_cb()
{
  if (dynamic_cast<Gtk::CheckMenuItem *>(_text)->get_active()) {
    signal_text_activate.emit();
  }
}

void MenuBar::signal_beside_activate_cb()
{
  if (dynamic_cast<Gtk::CheckMenuItem *>(_beside)->get_active()) {
    signal_beside_activate.emit();
  }
}

void MenuBar::signal_both_activate_cb()
{
  if (dynamic_cast<Gtk::CheckMenuItem *>(_both)->get_active()) {
    signal_both_activate.emit();
  }
}

void MenuBar::enable_undo(bool s)
{
  _undo->set_sensitive(s);
}

void MenuBar::enable_redo(bool s)
{
  _redo->set_sensitive(s);
}

void MenuBar::enable_wrap_text(bool s)
{
  dynamic_cast<Gtk::CheckMenuItem *>(_wrap_text)->set_active(s);
}

void MenuBar::enable_line_numbers(bool s)
{
  dynamic_cast<Gtk::CheckMenuItem *>(_line_numbers)->set_active(s);
}

#ifdef ENABLE_SPELL
void MenuBar::enable_auto_spell(bool s)
{
  dynamic_cast<Gtk::CheckMenuItem *>(_auto_spell)->set_active(s);
}
#endif

void MenuBar::reset_gui(bool enable)
{
  _save->set_sensitive(enable);
  _save_as->set_sensitive(enable);
  _save_copy->set_sensitive(enable);
  _revert->set_sensitive(enable);
  _export_menu->set_sensitive(enable);
  _import_menu->set_sensitive(enable);

#ifdef ENABLE_PRINT
  _print_preview->set_sensitive(enable);
  _print->set_sensitive(enable);
#endif
  _close->set_sensitive(enable);
  _undo->set_sensitive(enable);
  _redo->set_sensitive(enable);
  _cut->set_sensitive(enable);
  _copy->set_sensitive(enable);
  _paste->set_sensitive(enable);
  _erase->set_sensitive(enable);
  _select_all->set_sensitive(enable);
  _find->set_sensitive(enable);
  _find_next->set_sensitive(enable);
  _replace->set_sensitive(enable);
  _goto_line->set_sensitive(enable);
  _line_numbers->set_sensitive(enable);
  _wrap_text->set_sensitive(enable);
  _encoding_menu->set_sensitive(enable);
  _execute->set_sensitive(enable);
#ifdef ENABLE_SPELL
  _spell->set_sensitive(enable);
  _auto_spell->set_sensitive(enable);
#endif
#ifdef ENABLE_EMULATOR
  _emulator_menu->set_sensitive(enable);
#endif
  _save_all->set_sensitive(enable);
  _close_all->set_sensitive(enable);
}

void MenuBar::set_read_only(int x, bool r)
{
  _save->set_sensitive(!r);
  _cut->set_sensitive(!r);
  _paste->set_sensitive(!r);
  _erase->set_sensitive(!r);
  _replace->set_sensitive(!r);
  document_set_readonly(x, r);
}

void MenuBar::signal_recent_activate_cb(std::string &str)
{
  signal_recent_activate.emit(str);
}

#ifdef ENABLE_HIGHLIGHT
void MenuBar::create_highlighters()
{
  Gtk::Menu *item;
  Gtk::MenuItem *_item;
  Gtk::RadioButtonGroup highlighters_radio;

  highlight = menu(_("_Highlight"), view_menu);

  _item = radio_item(highlight, highlighters_radio, _("None"));
  _item->signal_activate().connect(
      sigc::bind<std::string>(sigc::mem_fun(*this, &MenuBar::signal_highlighter_activate_cb), ""));

  SourceCategory &cats = SourceManager::get_categories();
  SourceCategoryIter iter;

  for (iter = cats.begin(); iter != cats.end(); iter++) {
    item = menu(const_cast<char *>(iter->first.c_str()), highlight);
    for (unsigned x = 0; x < iter->second.size(); x++) {
      _item = radio_item(item, highlighters_radio, SourceManager::get_name(iter->second[x]));
      _item->property_user_data().set_value(const_cast<char *>(iter->second[x].c_str()));
      _item->signal_activate().connect(
          sigc::bind<std::string>(sigc::mem_fun(*this, &MenuBar::signal_highlighter_activate_cb),
                                  iter->second[x]));
    }
  }
}

void MenuBar::signal_highlighter_activate_cb(std::string id)
{
  if (_ignore_highlighting_changed_signal_hack == true) {
    return;
  }

  Gtk::Menu_Helpers::MenuList &items = highlight->items();
  // Iterate over the items.
  for (Gtk::Menu_Helpers::MenuList::iterator iter = items.begin(); iter != items.end(); iter++) {
    Gtk::Menu *m = iter->get_submenu();
    if (m) {
      Gtk::Menu_Helpers::MenuList &items2 = m->items();
      // Iterate over the items.
      for (Gtk::Menu_Helpers::MenuList::iterator iter2 = items2.begin(); iter != items2.end();
           iter++) {
        if (id == static_cast<char *>(iter2->property_user_data().get_value())) {
          Gtk::CheckMenuItem *it = dynamic_cast<Gtk::CheckMenuItem *>(&(*iter2));
          if (it && it->get_active()) {
            signal_highlighter_activate.emit(id);
            return;
          }
        }
      }
    } else {
      // Probably this is the None item.
      Gtk::CheckMenuItem *it = dynamic_cast<Gtk::CheckMenuItem *>(&(*iter));
      if (it && it->get_active()) {
        signal_highlighter_activate.emit("");
        return;
      }
    }
  }
}

void MenuBar::set_highlight(std::string id)
{
  _ignore_highlighting_changed_signal_hack = true;

  Gtk::Menu_Helpers::MenuList &items = highlight->items();
  if (id == "") {
    Gtk::CheckMenuItem *item = dynamic_cast<Gtk::CheckMenuItem *>(&(*items.begin()));
    if (item) {
      item->set_active(true);
      _ignore_highlighting_changed_signal_hack = false;
      return;
    }
  } else {
    // Iterate over the items.
    for (Gtk::Menu_Helpers::MenuList::iterator iter = items.begin(); iter != items.end(); iter++) {
      // We know they are all Gtk::Menu objects
      Gtk::Menu *m = iter->get_submenu();
      if (!m) {
        continue;
      }
      Gtk::Menu_Helpers::MenuList &items2 = m->items();

      for (Gtk::Menu_Helpers::MenuList::iterator iter2 = items2.begin(); iter2 != items2.end();
           iter2++) {
        if (id == static_cast<char *>(iter2->property_user_data().get_value())) {
          Gtk::CheckMenuItem *it = dynamic_cast<Gtk::CheckMenuItem *>(&(*iter2));
          if (it) {
            it->set_active(true);
            _ignore_highlighting_changed_signal_hack = false;
            return;
          }
        }
      }
    }
  }
  // Just in case.
  _ignore_highlighting_changed_signal_hack = false;
}
#endif

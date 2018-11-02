/*
 * bars.cc
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include "bars.hh"
#include "utils.hh"
#include "macros.h"
#include <gtkmm/toggleaction.h>
#include <gtkmm/radioaction.h>
#include <iostream>
#include <gtkmm/toolbar.h>
#include <gtkmm/entry.h>
#include "spellmenu.hh"
#include <fstream>
#include <sigc++/bind.h>
#include "recent.hh"
#include <cassert>
#include "encodings.hh"
#include <gtk/gtkaction.h>
#include "docfactory.hh"
#include "import.hh"
#include "export.hh"
#ifdef ENABLE_HIGHLIGHT
#include "sourcemanager.hh"
#endif

Bars::Bars(Conf *conf) throw() : _conf(conf), _encodings(NULL),  sm(NULL), extra_buttons(NULL) {
  recent_merge_id = 0;
  open_merge_id = 0;
  closed_merge_id = 0;

  std::string ui_path = Utils::get_data_path("ui", "bars.xml");
  ui = Gtk::UIManager::create();
#ifdef GLIBMM_EXCEPTIONS_ENABLED
  try {
    ui->add_ui_from_file(ui_path);
  }
  catch (Glib::FileError& er) {
    std::cerr << "Can't read the UI definition file: " << er.what() << std::endl;
  }
  catch (Glib::MarkupError& er) {
    std::cerr << "Can't read the UI definition file '" << ui_path << "': " << er.what() << std::endl;
  }
#else
  std::auto_ptr<Glib::Error> error;
  ui->add_ui_from_file(ui_path, error);
  if (error.get()) {
    std::cerr << "Can't read the UI definition file: " << error->what() << std::endl;
  }
#endif

  document = Gtk::ActionGroup::create();
  generic = Gtk::ActionGroup::create();
  menubar = Gtk::ActionGroup::create();
  recent = Gtk::ActionGroup::create();
  encodings = Gtk::ActionGroup::create();
  open = Gtk::ActionGroup::create();
  closed = Gtk::ActionGroup::create();
  import = Gtk::ActionGroup::create();
  exprt = Gtk::ActionGroup::create();
#ifdef ENABLE_HIGHLIGHT
  highlight = Gtk::ActionGroup::create();
#endif

  add_actions();

  ui->insert_action_group(recent);
  ui->insert_action_group(encodings);
  ui->insert_action_group(open);
  ui->insert_action_group(closed);
  ui->insert_action_group(import);
  ui->insert_action_group(exprt);

  // We don't call this because reset_ui() does it for us.
  //  create_recent();

  DocFactory *factory = DocFactory::get();

  factory->signal_can_undo.connect(sigc::mem_fun(this, &Bars::enable_undo));
  factory->signal_can_redo.connect(sigc::mem_fun(this, &Bars::enable_redo));
  factory->signal_wrap_text_set.connect(sigc::mem_fun(this, &Bars::enable_text_wrap));
  factory->signal_line_numbers_set.connect(sigc::mem_fun(this, &Bars::enable_line_numbers));
#ifdef ENABLE_SPELL
  factory->signal_dictionary_changed.connect(sigc::mem_fun(this, &Bars::set_spell_dict));
#endif
  factory->signal_created.connect(sigc::mem_fun(this, &Bars::create_open_documents));
  factory->signal_closed.connect(sigc::mem_fun(this, &Bars::create_closed_documents));
  factory->signal_closed.connect(sigc::bind<Document *>(sigc::mem_fun(this, &Bars::create_open_documents), NULL));
#ifdef ENABLE_HIGHLIGHT
  factory->signal_highlight.connect(sigc::mem_fun(this, &Bars::set_highlight));
#endif
#ifdef ENABLE_MAEMO
  generic->add(Gtk::Action::create("FullScreenB", Gtk::Stock::ZOOM_FIT), signal_full_screen);
  ui->add_ui(ui->new_merge_id(), "/ToolBar/FullScreen", "FullScreenB", "FullScreenB", Gtk::UI_MANAGER_TOOLITEM, false);
#endif
}

Bars::~Bars() {

}

Gtk::Widget *Bars::get_menubar() {
  static Gtk::Widget *widget = ui->get_widget("/MenuBar");
  return widget;
}

Gtk::Widget *Bars::get_toolbar() {
  static Gtk::Widget *widget = ui->get_widget("/ToolBar");
  return widget;
}

Gtk::Widget *Bars::get_toolbar_extended() {
  static Gtk::Widget *widget = ui->get_widget("/ToolBarExtended");
  return widget;
}

Glib::RefPtr<Gtk::AccelGroup> Bars::get_accel_group() {
  return ui->get_accel_group();
}

Bars::MenuItem *Bars::get_menubar_items() {
  static MenuItem MenuItems[] = {
    {"MenuFile", N_("_File")},
    {"MenuEdit", N_("_Edit")},
    {"MenuSearch", N_("_Search")},
    {"MenuView", N_("_View")},
    {"MenuTools", N_("_Tools")},
    {"MenuDocuments", N_("_Documents")},
    {"MenuHelp", N_("_Help")},
    {"Recent", N_("Recent")},
    {"Import", N_("_Import")},
    {"Export", N_("_Export")},
    {"Toolbars", N_("Toolbars")},
    {"Encoding", N_("_Encoding")},
    {"OpenDocuments", N_("_Open Documents")},
    {"ClosedDocuments", N_("_Closed Documents")},
    {"IconsOnly", N_("Icons only")},
    {"TextOnly", N_("Text only")},
    {"Both", N_("Both")},
    {"BothHoriz", N_("Text beside icons")},
#ifdef ENABLE_HIGHLIGHT
    {"Highlight", N_("_Highlight")},
#endif
    {NULL, NULL}
  };
  return MenuItems;
}

Bars::StockItem *Bars::get_stock_items() {
  static StockItem StockItems[] = {
    {"New", &Gtk::Stock::NEW, signal_new, true, NULL},
    {"Open", &Gtk::Stock::OPEN, signal_open, true, NULL},
    {"Save", &Gtk::Stock::SAVE, signal_save, false, NULL},
    {"Print", &Gtk::Stock::PRINT, signal_print, false, "<control>P"},
    {"Close", &Gtk::Stock::CLOSE, signal_close, false, NULL},
    {"Undo", &Gtk::Stock::UNDO, signal_undo, false, "<control>Z"},
    {"Redo", &Gtk::Stock::REDO, signal_redo, false, "<control><shift>Z"},
    {"Cut", &Gtk::Stock::CUT, signal_cut, false, NULL},
    {"Copy", &Gtk::Stock::COPY, signal_copy, false, NULL},
    {"Paste", &Gtk::Stock::PASTE, signal_paste, false, NULL},
    {"Delete", &Gtk::Stock::DELETE, signal_delete, false, "<control>D"},
    {"SaveAs", &Gtk::Stock::SAVE_AS, signal_save_as, false, "<control><shift>S"},
    {"Revert", &Gtk::Stock::REVERT_TO_SAVED, signal_revert, false, NULL},
    {"Quit", &Gtk::Stock::QUIT, signal_quit, true},
    {"Preferences", &Gtk::Stock::PREFERENCES, signal_preferences, true, NULL},
    {"Find", &Gtk::Stock::FIND, signal_find, false, NULL},
    {"FindReplace", &Gtk::Stock::FIND_AND_REPLACE, signal_find_replace, false, NULL},
    {"JumpTo", &Gtk::Stock::JUMP_TO, signal_goto, false, NULL},
    {"SpellCheck", &Gtk::Stock::SPELL_CHECK, signal_spell, true, "<control>F7"},
    {"About", &Gtk::Stock::ABOUT, signal_about, true, NULL},
    {NULL}
  };

  return StockItems;
}

Bars::Item *Bars::get_items() {
  static Item Items[] = {
    {true, "OpenLocation", N_("Open _Location..."), signal_open_location, NULL},
    {false, "SaveCopy", N_("Sa_ve Copy..."), signal_save_copy, NULL},
    {false, "PageSetup", N_("Page Set_up..."), signal_page_setup, NULL},
    {false, "PrintPreview", N_("Print Previe_w"), signal_print_preview, "<control><shift>P"},
    {false, "SelectAll", N_("_Select All"), signal_select_all, "<control>A"},
    {false, "InsertFile", N_("_Insert File..."), signal_insert_file, NULL},
    {false, "FindNext", N_("Find Ne_xt"), signal_find_next, "<control>G"},
    {false, "ExecuteCommand", N_("_Execute Command On Buffer..."), signal_execute, "<control>E"},
    {true, "SaveAll", N_("Save _All"), signal_save_all, "<control><shift>S"},
    {true, "CloseAll", N_("Clos_e All"), signal_close_all, "<control><shift>W"},
    {true, NULL, NULL}
  };

  return Items;
}

Bars::Item *Bars::get_check_items() {
  static Item items[] = {
    {true, "Statusbar", N_("_Statusbar"), sigc::mem_fun(this, &Bars::statusbar_toggled), NULL},
    {false, "LineNumbers", N_("_Line Numbers"), sigc::mem_fun(this, &Bars::line_numbers_toggled), NULL},
    {false, "WrapText", N_("_Wrap Text"), sigc::mem_fun(this, &Bars::wrap_text_toggled), NULL},
    {false, "AutocheckSpelling", N_("_Autocheck Spelling"), sigc::mem_fun(this, &Bars::auto_spell_toggled), NULL},
    {true, "ExtendedToolbar", N_("_Extended Toolbar"), sigc::mem_fun(this, &Bars::toolbar_extended), NULL},
    {true, "MainToolbar", N_("_Main Toolbar"), sigc::mem_fun(this, &Bars::toolbar_main), NULL},
    {true, NULL, NULL}
  };
  return items;
}

std::map<std::string, std::string> Bars::get_tooltips() {
  std::map<std::string, std::string> tips;

  tips["gtk-new"] = _("Create a new file");
  tips["gtk-open"] = _("Open a file for editing");
  tips["gtk-save"] = _("Save the existing file");
#ifdef ENABLE_PRINT
  tips["gtk-print"] = _("Print this document");
#endif
  tips["gtk-close"] = _("Close the active file");
  tips["gtk-undo"] = _("Undo");
  tips["gtk-redo"] = _("Redo");
  tips["gtk-cut"] = _("Cut");
  tips["gtk-copy"] = _("Copy");
  tips["gtk-paste"] = _("Paste");
  tips["gtk-delete"] = _("Delete current selection");

#ifdef ENABLE_MAEMO
  tips["gtk-zoom-fit"] = _("Toggle full screen mode");
#endif

  return tips;
}

void Bars::add_actions() {
  MenuItem *mb_item = get_menubar_items();
  while (mb_item->id != NULL) {
    menubar->add(Gtk::Action::create(mb_item->id, _(mb_item->name)));
    ++mb_item;
  }
  ui->insert_action_group(menubar);

  StockItem *s_item = get_stock_items();
  while (s_item->name != NULL) {
    create_item(s_item++);
  }

  Item *item = get_items();
  while (item->id != NULL) {
    create_item(item++);
  }

  item = get_check_items();
  while (item->id != NULL) {
    create_check_item(item++);
  }

  // ToolBarstyle
  Gtk::RadioAction::Group gp;
  generic->add(Gtk::RadioAction::create(gp, "TextOnly", _("Text only")), sigc::mem_fun(this, &Bars::toolbar_text));
  generic->add(Gtk::RadioAction::create(gp, "IconsOnly", _("Icons only")), sigc::mem_fun(this, &Bars::toolbar_icons));
  generic->add(Gtk::RadioAction::create(gp, "Both", _("Both")), sigc::mem_fun(this, &Bars::toolbar_both));
  generic->add(Gtk::RadioAction::create(gp, "BothHoriz", _("Text beside icons")), sigc::mem_fun(this, &Bars::toolbar_both_horiz));

  //  create_recent();
  ui->insert_action_group(generic);
  ui->insert_action_group(document);


  build_extended();

#ifdef ENABLE_HIGHLIGHT
  build_highlight();
#endif

  // HACK: This is for both_horiz style
  // NOTE: http://mail.gnome.org/archives/gtkmm-list/2004-June/msg00112.html

  Gtk::Widget *widget = ui->get_widget("/ToolBar");
  if (widget) {
    Gtk::Toolbar *toolbar = dynamic_cast<Gtk::Toolbar *>(widget);
    if (toolbar) {
      std::vector<Gtk::Widget *> ch = toolbar->get_children();
      Gtk::ToolButton *tb_item = NULL;
      std::map<std::string, std::string> tips_map = get_tooltips();

      // Our tooltips.
      // NOTE:
      // http://mail.gnome.org/archives/gtkmm-list/2004-October/msg00010.html
      // http://mail.gnome.org/archives/gtkmm-list/2004-October/msg00011.html

      toolbar->set_tooltips(true);
      Gtk::Tooltips *tips = toolbar->get_tooltips_object();

      for (unsigned x = 0; x < ch.size(); x++) {
        tb_item = dynamic_cast<Gtk::ToolButton *>(ch[x]);
        if (tb_item) {
          tb_item->set_is_important();
          // tb_item->set_tooltip(*tips, tips_map[tb_item->get_stock_id()]);
        }
      }
    }
  }
}

void Bars::create_item(Bars::StockItem *item) {
  if (item->accelerator) {
    add_to_action_group(item->generic ? generic : document, Gtk::Action::create(item->name, *(item->stock)), item->signal, Gtk::AccelKey(item->accelerator));
  }
  else {
    add_to_action_group(item->generic ? generic : document, Gtk::Action::create(item->name, *(item->stock)), item->signal);
  }
}

void Bars::create_item(Bars::Item *item) {
  if (item->accelerator) {
    add_to_action_group(item->generic ? generic : document, Gtk::Action::create(item->id, _(item->name)), item->signal, Gtk::AccelKey(item->accelerator));
  }
  else {
    add_to_action_group(item->generic ? generic : document, Gtk::Action::create(item->id, _(item->name)), item->signal);
  }
}

void Bars::create_check_item(Bars::Item *item) {
  if (item->generic) {
    generic->add(Gtk::ToggleAction::create(item->id, _(item->name)), item->signal);
  }
  else {
    document->add(Gtk::ToggleAction::create(item->id, _(item->name)), item->signal);
  }
}

void Bars::add_to_action_group(Glib::RefPtr<Gtk::ActionGroup>& group, Glib::RefPtr<Gtk::Action> action, sigc::slot<void>& signal, const Gtk::AccelKey& key) {
  if (key.is_null()) {
    group->add(action, signal);
  }
  else {
    group->add(action, key, signal);
  }
}

#ifdef ENABLE_HIGHLIGHT
void Bars::set_highlight(std::string lang) {
  if (lang == "") {
    Glib::RefPtr<Gtk::Action> action = highlight->get_action("Highlight0");
      Utils::block<void, std::string>(signal_highlight);
      set_active(action, true);
      Utils::unblock<void, std::string>(signal_highlight);
  }

  std::vector<Glib::RefPtr<Gtk::Action> > actions = highlight->get_actions();
  for (unsigned x = 0; x < actions.size(); x++) {
    if (lang == actions[x]->property_label().get_value()) {
      Utils::block<void, std::string>(signal_highlight);
      set_active(actions[x], true);
      Utils::unblock<void, std::string>(signal_highlight);
    }
  }
}

void Bars::highlight_activate(std::string id, std::string lang) {
  Glib::RefPtr<Gtk::Action> act = highlight->get_action(id);
  if (!act) {
    return;
  }

  Glib::RefPtr<Gtk::RadioAction> action = Glib::RefPtr<Gtk::RadioAction>::cast_dynamic(act);
  if (!act) {
    return;
  }

  if (action->get_active()) {
    signal_highlight.emit(lang);
  }
}

void Bars::build_highlight() {
  ui->insert_action_group(highlight);

  int merge_id = ui->new_merge_id();
  Gtk::RadioAction::Group r_gp;

  std::string parent = "/MenuBar/MenuView/Highlight";
  std::string id = "Highlight0";

  highlight->add(Gtk::RadioAction::create(r_gp, id, _("None")), sigc::bind<std::string, std::string>(sigc::mem_fun(this, &Bars::highlight_activate), id, ""));
  ui->add_ui(merge_id, parent, id, id, Gtk::UI_MANAGER_AUTO, false);

  SourceCategory& cats = SourceManager::get_categories();
  SourceCategoryIter iter;
  int x = 0;
  int h = 1;

  for (iter = cats.begin(); iter != cats.end(); iter++) {
    id = Utils::substitute("HighlightCat%i", x++);
    highlight->add(Gtk::Action::create(id, iter->first));
    ui->add_ui(merge_id, parent, id, id, Gtk::UI_MANAGER_MENU, false);

    for (unsigned i = 0; i < iter->second.size(); i++) {
      std::string path = Utils::substitute("%s/%s", parent, id);
      std::string name = Utils::substitute("Highlight%i", h++);
      highlight->add(Gtk::RadioAction::create(r_gp, name, iter->second[i]), sigc::bind<std::string, std::string>(sigc::mem_fun(this, &Bars::highlight_activate), name, iter->second[i]));
      ui->add_ui(merge_id, path, name, name, Gtk::UI_MANAGER_MENUITEM, false);
    }
  }
}
#endif

void Bars::build_extended() {
  Gtk::Widget *widget = ui->get_widget("/ToolBarExtended");
  if (!widget) {
    std::cerr << "Can't get the extended toolbar" << std::endl;
    return;
  }
  Gtk::Container *toolbar = dynamic_cast<Gtk::Container *>(widget);
  //  toolbar->set_border_width(10);

  // Search
  Gtk::Label *label = Gtk::manage(new Gtk::Label(_("Search")));
  label->set_padding(5, 0);
  Gtk::Entry *entry = Gtk::manage(new Gtk::Entry());
  entry->signal_activate().connect(sigc::bind<Gtk::Entry *>(sigc::mem_fun(*this, &Bars::search_activate), entry));
  toolbar->add(*label);
  toolbar->add(*entry);

  // Goto
  label = Gtk::manage(new Gtk::Label(_("Goto Line")));
  label->set_padding(5, 0);
  entry = Gtk::manage(new Gtk::Entry());
  entry->set_width_chars(5);
  entry->signal_activate().connect(sigc::bind<Gtk::Entry *>(sigc::mem_fun(*this, &Bars::goto_activate), entry));
  toolbar->add(*label);
  toolbar->add(*entry);

  // Dictionary drop down.
#ifdef ENABLE_SPELL
  label = Gtk::manage(new Gtk::Label(_("Spelling Dictionary")));
  label->set_padding(5, 0);
  sm = Gtk::manage(new SpellMenu());
  toolbar->add(*label);
  toolbar->add(*sm);
  sm->signal_changed().connect(sigc::bind<SpellMenu *>(sigc::mem_fun(*this, &Bars::spell_dict_changed), sm));

  // spell button
  Gtk::Button *button = Gtk::manage(new Gtk::Button());
  button->set_image(*(Gtk::manage(new Gtk::Image(Gtk::Stock::SPELL_CHECK, Gtk::ICON_SIZE_BUTTON))));
  button->set_focus_on_click(false);
  button->set_relief(Gtk::RELIEF_NONE);
  button->signal_clicked().connect(sigc::mem_fun(signal_spell, &sigc::signal<void>::emit));
  toolbar->add(*button);
#endif
  // extra buttons.
  Gtk::Toolbar *extended = Gtk::manage(new Gtk::Toolbar());
  build_extra_buttons(extended);
  toolbar->add(*extended);
  toolbar->show_all();
}

void Bars::build_extra_buttons(Gtk::Toolbar *toolbar) {
  extra_buttons = Gtk::manage(new Gtk::Toolbar);
  toolbar->add(*extra_buttons);

  std::string path = Utils::get_data_path("ui", "toolbar");
  std::ifstream ifs;
  ifs.open(path.c_str());
  if (ifs.is_open()) {
    std::string buff;
    while (getline(ifs, buff)) {
      create_extra_button(extra_buttons, buff);
    }
    ifs.close();
  }
}

void Bars::create_extra_button(Gtk::Toolbar *toolbar, std::string& label) {
  Gtk::ToolButton *button = Gtk::manage(new Gtk::ToolButton(label));
  button->signal_clicked().connect(sigc::bind<std::string>(sigc::mem_fun(signal_extra_button, &sigc::signal<void, std::string>::emit), label));
  toolbar->append(*button);
}

void Bars::toolbar_text() {
  set_toolbar_style("TextOnly", "text", Gtk::TOOLBAR_TEXT);
}

void Bars::toolbar_icons() {
  set_toolbar_style("IconsOnly", "icons", Gtk::TOOLBAR_ICONS);
}

void Bars::toolbar_both() {
  set_toolbar_style("Both", "both", Gtk::TOOLBAR_BOTH);
}

void Bars::toolbar_both_horiz() {
  set_toolbar_style("BothHoriz", "both_horiz", Gtk::TOOLBAR_BOTH_HORIZ);
}

void Bars::toolbar_main() {
  toolbar_toggle("/ToolBar", "MainToolbar", "toolbar");
}

void Bars::toolbar_extended() {
  toolbar_toggle("/ToolBarExtended", "ExtendedToolbar", "extended_toolbar");
}

void Bars::search_activate(Gtk::Entry *entry) {
  std::string text = entry->get_text();
  if (text != "") {
    signal_search_string.emit(text);
  }
}

void Bars::goto_activate(Gtk::Entry *entry) {
  std::string text = entry->get_text();
  if (text != "") {
    int x = atoi(text.c_str());
    if (x > 0) {
      signal_goto_line.emit(x);
    }
  }
}

#ifdef ENABLE_SPELL
void Bars::spell_dict_changed(SpellMenu *sm) {
  signal_spell_dict_changed.emit(sm->get_active_text());
}
#endif

void Bars::statusbar_toggled() {
  emit_signal(signal_statusbar, generic->get_action("Statusbar"));
}

void Bars::wrap_text_toggled() {
  emit_signal(signal_wrap_text, document->get_action("WrapText"));
}

void Bars::line_numbers_toggled() {
  emit_signal(signal_line_numbers, document->get_action("LineNumbers"));
}

void Bars::auto_spell_toggled() {
  emit_signal(signal_auto_spell, document->get_action("AutocheckSpelling"));
}

void Bars::create_recent() {
  Recent *_recent = Recent::get();

  if (recent_merge_id != 0) {
    ui->remove_ui(recent_merge_id);
    std::vector<Glib::RefPtr<Gtk::Action> > actions = recent->get_actions();
    for (unsigned x = 0; x < actions.size(); x++) {
      recent->remove(actions[x]);
    }
  }
  else {
    // first time
    recent_conn = _recent->signal_changed.connect(sigc::mem_fun(*this, &Bars::create_recent));
  }

  // This is here after we erase the menu contents.
  if (!_conf->get("recent", true)) {
    recent_conn.disconnect();
    return;
  }

  recent_merge_id = ui->new_merge_id();

  for (unsigned x = 0; x < _recent->size(); x++) {
    std::string base = Utils::filename_display_basename((*_recent)[x]);
    std::string name = Utils::substitute("Recent%i", x);
    recent->add(Gtk::Action::create(name, base), sigc::bind<std::string>(sigc::mem_fun(*this, &Bars::recent_item_activate), (*_recent)[x]));
    std::string path = Utils::substitute("/MenuBar/MenuFile/Recent/%s", name);
    ui->add_ui(recent_merge_id, "/MenuBar/MenuFile/Recent", name, name, Gtk::UI_MANAGER_AUTO, false);
  }
}

void Bars::recent_item_activate(std::string file) {
  signal_recent(file);
}

void Bars::create_open_documents(Document *doc) {
  if (open_merge_id != 0) {
    ui->remove_ui(open_merge_id);
    std::vector<Glib::RefPtr<Gtk::Action> > actions = open->get_actions();
    for (unsigned x = 0; x < actions.size(); x++) {
      open->remove(actions[x]);
    }
  }

  open_merge_id = ui->new_merge_id();
  Gtk::RadioAction::Group r_gp;

  std::string parent = "/MenuBar/MenuDocuments/OpenDocuments";

  int x = 0;
  std::string title;
  bool ro, m;

  DocFactory *factory = DocFactory::get();

  while (factory->get_info(x, title, ro, m)) {
    std::string id = Utils::substitute("OpenDocuments%i", x);
    open->add(Gtk::RadioAction::create(r_gp, id, title), sigc::bind<unsigned>(sigc::mem_fun(signal_open_document, &sigc::signal<void, unsigned>::emit), x));
    ui->add_ui(open_merge_id, parent, id, id, Gtk::UI_MANAGER_AUTO, false);

    std::string path = Utils::substitute("%s/%s", parent, id);
    ui->ensure_update();

    Gtk::MenuItem *item = dynamic_cast<Gtk::MenuItem *>(ui->get_widget(path));
    if (item) {
      Gtk::Label *label = dynamic_cast<Gtk::Label *>(dynamic_cast<Gtk::Bin *>(item)->get_child());

      color_label(label, ro, m);
    }
    ++x;
  }

  if (!doc) {
    // TODO: Better way ?
    doc = signal_request_active.emit();
  }

  if (doc) {
    int x = factory->get_index(doc);
    if (x != -1) {
      set_active_document(x);
    }
  }
}

void Bars::create_closed_documents() {
  if (closed_merge_id != 0) {
    ui->remove_ui(closed_merge_id);
    std::vector<Glib::RefPtr<Gtk::Action> > actions = closed->get_actions();
    for (unsigned x = 0; x < actions.size(); x++) {
      closed->remove(actions[x]);
    }
  }

  closed_merge_id = ui->new_merge_id();

  std::string parent = "/MenuBar/MenuDocuments/ClosedDocuments";

  int x = 0;
  std::string title;
  bool ro, m;

  DocFactory *factory = DocFactory::get();
  while (factory->get_closed_info(x, title, ro, m)) {
    std::string id = Utils::substitute("ClosedDocuments%i", x);
    closed->add(Gtk::Action::create(id, title), sigc::bind<unsigned>(sigc::mem_fun(this, &Bars::closed_item_activate), x));
    ui->add_ui(closed_merge_id, parent, id, id, Gtk::UI_MANAGER_AUTO, false);

    std::string path = Utils::substitute("%s/%s", parent, id);

    Gtk::MenuItem *item = dynamic_cast<Gtk::MenuItem *>(ui->get_widget(path));
    if (item) {
      Gtk::Label *label = dynamic_cast<Gtk::Label *>(dynamic_cast<Gtk::Bin *>(item)->get_child());

      color_label(label, ro, m);
    }
    ++x;
  }
}

void Bars::color_label(Gtk::Label *label, bool ro, bool m) {
  if (ro) {
    Utils::katoob_set_color(*_conf, *label, Utils::KATOOB_COLOR_READONLY);
  }
  else if (m) {
    Utils::katoob_set_color(*_conf, *label, Utils::KATOOB_COLOR_MODIFIED);
  }
  else {
    Utils::katoob_set_color(*_conf, *label, Utils::KATOOB_COLOR_NORMAL);
  }
}

void Bars::closed_item_activate(int idx) {
  create_closed_documents();
  signal_closed_document.emit(idx);
}

void Bars::create_encodings() {
  assert(_encodings != NULL);

  int merge_id = ui->new_merge_id();
  unsigned e = 0;
  Gtk::RadioAction::Group gp;
  for (int x = 0; x < _encodings->languages(); x++) {
    std::string name = Utils::substitute("Language%i", x);
    encodings->add(Gtk::Action::create(name, _encodings->language(x)));

    std::string path = Utils::substitute("/MenuBar/MenuView/Encoding/%s", name);
    ui->add_ui(merge_id, "/MenuBar/MenuView/Encoding", name, name, Gtk::UI_MANAGER_MENU, false);

    for (int y = 0; y <_encodings->languages(x); y++) {
      name = Utils::substitute("Encoding%i", e);
      encodings->add(Gtk::RadioAction::create(gp, name, _encodings->name(x, y)), sigc::bind<unsigned>(sigc::mem_fun(this, &Bars::encoding_clicked), e));
      ui->add_ui(merge_id, path, name, name, Gtk::UI_MANAGER_AUTO, false);
      ++e;
    }
  }
}

void Bars::set_encodings(Encodings *enc) {
  _encodings = enc;
  create_encodings();
}

void Bars::set_import(std::vector<Import *>& imprt) {
  int merge_id = ui->new_merge_id();

  std::string name, path;

  for (unsigned x = 0; x < imprt.size(); x++) {
    name = Utils::substitute("Import%i", x);
    path = Utils::substitute("/MenuBar/MenuFile/Import/%s", name);
    import->add(Gtk::Action::create(name, imprt[x]->name), sigc::bind<Import *>(sigc::mem_fun(signal_import, &sigc::signal<void, Import *>::emit), imprt[x]));
    ui->add_ui(merge_id, "/MenuBar/MenuFile/Import", name, name, Gtk::UI_MANAGER_MENUITEM, false);
  }
}

void Bars::set_export(std::vector<Export *>& exp) {
  int merge_id = ui->new_merge_id();

  std::string name, path;

  for (unsigned x = 0; x < exp.size(); x++) {
    name = Utils::substitute("Export%i", x);
    path = Utils::substitute("/MenuBar/MenuFile/Export/%s", name);
    exprt->add(Gtk::Action::create(name, exp[x]->name), sigc::bind<Export *>(sigc::mem_fun(signal_export, &sigc::signal<void, Export *>::emit), exp[x]));
    ui->add_ui(merge_id, "/MenuBar/MenuFile/Export", name, name, Gtk::UI_MANAGER_MENUITEM, false);
  }
}

#ifdef ENABLE_SPELL
void Bars::enable_spell(const std::string& dict, bool state) {
  Glib::RefPtr<Gtk::Action> action = document->get_action("AutocheckSpelling");
  if (!action) {
    return;
  }

  Glib::RefPtr<Gtk::ToggleAction> toggle = Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(action);
  if (!toggle) {
    return;
  }

  if (!sm) {
    return;
  }

  Utils::block<void, bool>(signal_auto_spell);
  Utils::block<void, std::string>(signal_spell_dict_changed);
  toggle->set_active(state);
  sm->set_sensitive(state);
  sm->set_active_text(dict);
  Utils::unblock<void, bool>(signal_auto_spell);
  Utils::unblock<void, std::string>(signal_spell_dict_changed);
}
#endif

void Bars::set_encoding(unsigned e) {
  std::string name = Utils::substitute("Encoding%i", e);
  Glib::RefPtr<Gtk::ToggleAction> action = Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(encodings->get_action(name));

  if (action->get_active()) {
    return;
  }

  Utils::block<void, unsigned>(signal_encoding);
  action->set_active(true);
  Utils::unblock<void, unsigned>(signal_encoding);
}

void Bars::set_rom(int idx, bool ro, bool m) {
  // TODO: We need to disable revert if the document has no file.
  // TODO: Better way than regenerating the whole menu.
  create_open_documents(NULL);
  create_closed_documents();

  if (ro) {
    document->get_action("Save")->set_sensitive(!ro);
    document->get_action("Cut")->set_sensitive(!ro);
    document->get_action("Paste")->set_sensitive(!ro);
    document->get_action("Delete")->set_sensitive(!ro);
    document->get_action("InsertFile")->set_sensitive(!ro);
    document->get_action("FindReplace")->set_sensitive(!ro);
    document->get_action("Revert")->set_sensitive(!ro);
  }
  else if (m) {
    document->get_action("Save")->set_sensitive(m);
    document->get_action("Cut")->set_sensitive(m);
    document->get_action("Paste")->set_sensitive(m);
    document->get_action("Delete")->set_sensitive(m);
    document->get_action("InsertFile")->set_sensitive(m);
    document->get_action("FindReplace")->set_sensitive(m);
    document->get_action("Revert")->set_sensitive(m);
  }
  else {
    document->get_action("Save")->set_sensitive(false);
  }
}

void Bars::set_label(int idx, std::string& label) {
  // TODO: We do nothing here.
  // saving means emitting the "rom" signal so we will end up rebuilding the menu anyway.
  /*
  // TODO: Better way

  std::string path = Utils::substitute("/MenuBar/MenuDocuments/OpenDocuments/OpenDocuments%i", idx);
  Gtk::Widget *widget = ui->get_widget(path);
  if (widget) {
    Gtk::MenuItem *item = dynamic_cast<Gtk::MenuItem *>(widget);
    if (item) {
      dynamic_cast<Gtk::Label *>(dynamic_cast<Gtk::Bin *>(item)->get_child())->set_text(label);
    }
  }
  */
}

void Bars::reset_widgets(bool enable) {
  document->set_sensitive(enable);
  exprt->set_sensitive(enable);
  encodings->set_sensitive(enable);
  generic->get_action("SpellCheck")->set_sensitive(enable);
  generic->get_action("SaveAll")->set_sensitive(enable);
  generic->get_action("CloseAll")->set_sensitive(enable);

#ifdef ENABLE_HIGHLIGHT
  if (!enable) {
    set_highlight("");
  }

  highlight->set_sensitive(enable);
#endif

  Gtk::Widget *widget = ui->get_widget("/ToolBarExtended");
  if (widget) {
    widget->set_sensitive(enable);
  }
}

void Bars::enable_redo(bool enabled) {
  document->get_action("Redo")->set_sensitive(enabled);
}

void Bars::enable_undo(bool enabled) {
  document->get_action("Undo")->set_sensitive(enabled);
}

void Bars::set_active_document(int idx) {
  std::string name = Utils::substitute("OpenDocuments%i", idx);
  Glib::RefPtr<Gtk::ToggleAction> action = Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(open->get_action(name));

  if ((!action) || (action->get_active())) {
    return;
  }

  Utils::block<void, unsigned>(signal_open_document);
  action->set_active(true);
  Utils::unblock<void, unsigned>(signal_open_document);
}

void Bars::enable_text_wrap(bool enabled) {
  Utils::block<void, bool>(signal_wrap_text);
  Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(document->get_action("WrapText"))->set_active(enabled);
  Utils::unblock<void, bool>(signal_wrap_text);
}

void Bars::enable_line_numbers(bool enabled) {
  Utils::block<void, bool>(signal_line_numbers);
  Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(document->get_action("LineNumbers"))->set_active(enabled);
  Utils::unblock<void, bool>(signal_line_numbers);
}

void Bars::reset_ui() {
  // We don't set the toolbar style manually cause we set the radio button and it will manage it.

  Gtk::Widget *widget = ui->get_widget("/ToolBar");
  if (widget) {
    _conf->get("toolbar", true) == true ? widget->show() :  widget->hide();
  }

  widget = ui->get_widget("/ToolBarExtended");
  if (widget) {
    _conf->get("extended_toolbar", true) == true ? widget->show() : widget->hide();
  }

  if (extra_buttons) {
    _conf->get("extra_buttons", true) == true ? extra_buttons->show_all() : extra_buttons->hide();
  }

#ifdef ENABLE_MAEMO
  const std::string& toolbartype = _conf->get("toolbartype", "icons");
#else
  const std::string& toolbartype = _conf->get("toolbartype", "both");
#endif

  if (toolbartype == "text") {
    set_active(generic->get_action("TextOnly"));
  }
  else if (toolbartype == "icons") {
    set_active(generic->get_action("IconsOnly"));
  }
  else if (toolbartype == "both_horiz") {
    set_active(generic->get_action("BothHoriz"));
  }
  else {
    set_active(generic->get_action("Both"));
  }

  bool linenumbers;
#ifdef ENABLE_MAEMO
  linenumbers = false;
#else
  linenumbers = true;
#endif
  set_active(document->get_action("LineNumbers"), linenumbers);

  set_active(generic->get_action("Statusbar"), _conf->get("statusbar", true));

  set_active(document->get_action("WrapText"), _conf->get("textwrap", true));

  set_active(generic->get_action("MainToolbar"), _conf->get("toolbar", true));
  set_active(generic->get_action("ExtendedToolbar"), _conf->get("extended_toolbar", true));

#ifdef ENABLE_SPELL
  set_active(document->get_action("AutocheckSpelling"), _conf->get("spell_check", true));
#endif

  create_recent();

#ifdef ENABLE_SPELL
  Utils::block<void, std::string>(signal_spell_dict_changed);
  if (sm) {
    sm->set_active_text(_conf->get("default_dict", "en"));
  }
  Utils::unblock<void, std::string>(signal_spell_dict_changed);
#endif

  // We do this to reset the colors.
  // TODO: Better way ?
  create_open_documents(NULL);
  create_closed_documents();
}

#ifdef ENABLE_SPELL
void Bars::set_spell_dict(const std::string& dict) {
  if (!sm) {
    return;
  }

  Utils::block<void, std::string>(signal_spell_dict_changed);
  sm->set_active_text(dict);
  Utils::unblock<void, std::string>(signal_spell_dict_changed);
}
#endif

void Bars::encoding_clicked(unsigned e) {
  std::string name = Utils::substitute("Encoding%i", e);
  Glib::RefPtr<Gtk::ToggleAction> action = Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(encodings->get_action(name));

  if (action->get_active()) {
    signal_encoding.emit(e);
  }
}

void Bars::set_active(const Glib::RefPtr<Gtk::Action>& action, bool active) {
  if (!action) {
    return;
  }

  Glib::RefPtr<Gtk::ToggleAction> ta = Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(action);
  ta->set_active(active);
}

void Bars::set_toolbar_style(const std::string& name, const std::string& conf, const Gtk::ToolbarStyle& style) {
  Glib::RefPtr<Gtk::Action> action = generic->get_action(name);
  if (!action) {
    return;
  }

  Gtk::Widget *widget = ui->get_widget("/ToolBar");

  if (!widget) {
    return;
  }

  Gtk::Toolbar *toolbar = dynamic_cast<Gtk::Toolbar *>(widget);

  if (toolbar) {
    toolbar->set_toolbar_style(style);
    _conf->set("toolbartype", conf.c_str());
  }
}

void Bars::emit_signal(sigc::signal<void, bool>& signal, const Glib::RefPtr<Gtk::Action>& action) {
  if (!action) {
    return;
  }

  Glib::RefPtr<Gtk::ToggleAction> toggle = Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(action);
  if (!toggle) {
    return;
  }

  signal.emit(toggle->get_active());
}

void Bars::toolbar_toggle(const std::string& path, const std::string& name, const std::string& conf) {
  Glib::RefPtr<Gtk::Action> action = generic->get_action(name);
  if (!action) {
    return;
  }
  Glib::RefPtr<Gtk::ToggleAction> toggle = Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(action);
  if (!toggle) {
    return;
  }

  Gtk::Widget *widget = ui->get_widget(path);
  if (!widget) {
    return;
  }

  bool st = toggle->get_active();
  _conf->set(conf.c_str(), st);

  if (st) {
    widget->show();
  }
  else {
    widget->hide();
  }
}

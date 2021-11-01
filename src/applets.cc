/*
 * applets.cc
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

#include "applets.hh"
#include "dict.hh"
#include "macros.h"
#include "network.hh"
#include "utils.hh"
#include <gtkmm.h>

Applet::Applet(Conf &conf) : _conf(conf) {
  box.set_spacing(5);
  box.set_border_width(10);
  box.set_homogeneous(false);
}

Applet::~Applet() {
  // Nothing.
}

Gtk::Widget &Applet::get_box() { return box; }

/* GeneralApplet */
GeneralApplet::GeneralApplet(Conf &_conf)
    : Applet::Applet(_conf), undono_adj(Gtk::Adjustment::create(0, 0, 100)),
      exec_adj(Gtk::Adjustment::create(0, 0, 100)),
      undo_closed_adj(Gtk::Adjustment::create(0, 0, 100)) {

  undo.set_use_underline();
  undo.set_label(_("_Enable undo, redo\t"));
  undo_closed.set_use_underline();
  undo_closed.set_label(_("Keep a history of recently closed documents"));

  undo_label.set_text(_("Undo history size\n(0 for unlimited)"));
  exec_label.set_text(_("Executed commands history size\n(0 for unlimited)"));
  undo_closed_label.set_text(
      _("Closed documents history size\n(0 for unlimited)"));

  undono.set_adjustment(undono_adj);
  exec_cmd_size.set_adjustment(exec_adj);
  undo_closedno.set_adjustment(undo_closed_adj);

  box.pack_start(undo, false, false);
  box.pack_start(general_table1, false, false);
  box.pack_start(undo_closed, false, false);
  box.pack_start(general_table2, false, false);

  general_table1.set_col_spacing(0, 5);
  general_table1.attach(undo_label, 0, 1, 0, 1, Gtk::AttachOptions(Gtk::SHRINK),
                        Gtk::AttachOptions(Gtk::SHRINK));
  general_table1.attach(undono, 1, 2, 0, 1,
                        Gtk::AttachOptions(Gtk::EXPAND | Gtk::FILL),
                        Gtk::AttachOptions(Gtk::SHRINK));
  general_table1.attach(exec_label, 0, 1, 1, 2, Gtk::AttachOptions(Gtk::SHRINK),
                        Gtk::AttachOptions(Gtk::SHRINK));
  general_table1.attach(exec_cmd_size, 1, 2, 1, 2,
                        Gtk::AttachOptions(Gtk::EXPAND | Gtk::FILL),
                        Gtk::AttachOptions(Gtk::SHRINK));

  general_table2.set_col_spacing(0, 5);
  general_table2.attach(undo_closed_label, 0, 1, 2, 3,
                        Gtk::AttachOptions(Gtk::SHRINK),
                        Gtk::AttachOptions(Gtk::SHRINK));
  general_table2.attach(undo_closedno, 1, 2, 2, 3,
                        Gtk::AttachOptions(Gtk::EXPAND | Gtk::FILL),
                        Gtk::AttachOptions(Gtk::SHRINK));

  undono_adj->set_value(_conf.get("undono", 0));
  exec_adj->set_value(_conf.get("exec_cmd_size", 10));
  undo_closed_adj->set_value(_conf.get("undo_closedno", 5));
  undo.set_active(_conf.get("undo", true));
  undo_closed.set_active(_conf.get("undo_closed", true));

  undo.signal_toggled().connect(
      sigc::mem_fun(*this, &GeneralApplet::undo_toggled_cb));
  undo_closed.signal_toggled().connect(
      sigc::mem_fun(*this, &GeneralApplet::undo_closed_toggled_cb));
  undo.toggled();
  undo_closed.toggled();
}

void GeneralApplet::apply() {
  _conf.set("undo", undo.get_active());
  _conf.set("undo_closed", undo_closed.get_active());
  _conf.set("undono", undono.get_value_as_int());
  _conf.set("exec_cmd_size", exec_cmd_size.get_value_as_int());
  _conf.set("undo_closedno", undo_closedno.get_value_as_int());
}

void GeneralApplet::undo_toggled_cb() {
  undono.set_sensitive(undo.get_active());
}

void GeneralApplet::undo_closed_toggled_cb() {
  undo_closedno.set_sensitive(undo_closed.get_active());
}

/* InterfaceApplet */
InterfaceApplet::InterfaceApplet(Conf &_conf)
    : Applet::Applet(_conf), recentno_adj(Gtk::Adjustment::create(0, 0, 100)) {

  toolbar.set_use_underline();
  extended_toolbar.set_use_underline();
  statusbar.set_use_underline();
  recent.set_use_underline();
  showclose.set_use_underline();
  extra_buttons.set_use_underline();

  extra_buttons.set_label(_("Show the extra _buttons"));
  toolbar.set_label(_("Show the _Main Toolbar"));
  extended_toolbar.set_label(_("Show the _Extended Toolbar"));
  statusbar.set_label(_("Show the _Statusbar"));
  recent.set_label(_("Enable re_cent menu"));
  showclose.set_label(_("Show the _close button on each tab label"));
  recentno_label.set_text(_("No. of recent entries\n(0 for unlimited)"));
  toolbartype_label.set_text(_("Show the toolbar as"));
  toolbartype.append(_("Text only"));
  toolbartype.append(_("Icons only"));
  toolbartype.append(_("Both"));
  toolbartype.append(_("Text beside icons"));

  box.pack_start(toolbar, false, false);
  box.pack_start(extended_toolbar, false, false);
  box.pack_start(statusbar, false, false);
  box.pack_start(recent, false, false);
  box.pack_start(interface_table, false, false);

  interface_table.set_col_spacing(0, 5);
  interface_table.attach(recentno_label, 0, 1, 0, 1,
                         Gtk::AttachOptions(Gtk::SHRINK),
                         Gtk::AttachOptions(Gtk::SHRINK));
  interface_table.attach(recentno, 1, 2, 0, 1,
                         Gtk::AttachOptions(Gtk::FILL | Gtk::EXPAND),
                         Gtk::AttachOptions(Gtk::SHRINK));
  interface_table.attach(toolbartype_label, 0, 1, 1, 2,
                         Gtk::AttachOptions(Gtk::SHRINK),
                         Gtk::AttachOptions(Gtk::SHRINK));
  interface_table.attach(toolbartype, 1, 2, 1, 2,
                         Gtk::AttachOptions(Gtk::FILL | Gtk::EXPAND),
                         Gtk::AttachOptions(Gtk::SHRINK));
  box.pack_start(showclose, false, false);
  box.pack_start(extra_buttons, false, false);

  extra_buttons.set_active(_conf.get("extra_buttons", true));
  recentno_adj->set_value(_conf.get("recentno", 10));
  recentno.set_adjustment(recentno_adj);
  toolbar.set_active(_conf.get("toolbar", true));
  extended_toolbar.set_active(_conf.get("extended_toolbar", true));
  statusbar.set_active(_conf.get("statusbar", true));
  recent.set_active(_conf.get("recent", true));
  showclose.set_active(_conf.get("showclose", true));
  std::string type = _conf.get("toolbartype", "both");
  if (type == "text") {
    toolbartype.set_active_text(_("Text only"));
  } else if (type == "icons") {
    toolbartype.set_active_text(_("Icons only"));
  } else if (type == "both_horiz") {
    toolbartype.set_active_text(_("Text beside icons"));
  } else {
    toolbartype.set_active_text(_("Both"));
  }

  toolbar.signal_toggled().connect(
      sigc::mem_fun(*this, &InterfaceApplet::toolbar_toggled_cb));
  recent.signal_toggled().connect(
      sigc::mem_fun(*this, &InterfaceApplet::recent_toggled_cb));
  toolbar.toggled();
  recent.toggled();
}

void InterfaceApplet::apply() {
  _conf.set("toolbar", toolbar.get_active());
  _conf.set("extended_toolbar", extended_toolbar.get_active());
  _conf.set("statusbar", statusbar.get_active());
  _conf.set("recent", recent.get_active());
  _conf.set("showclose", showclose.get_active());
  _conf.set("recentno", recentno.get_value_as_int());
  _conf.set("extra_buttons", extra_buttons.get_active());

  std::string val;
  switch (toolbartype.get_active_row_number()) {
  case 0:
    val = "text";
    break;
  case 1:
    val = "icons";
    break;
  case 3:
    val = "both_horiz";
    break;
  default:
    val = "both";
    break;
  }
  _conf.set("toolbartype", val.c_str());
}

void InterfaceApplet::recent_toggled_cb() {
  recentno.set_sensitive(recent.get_active());
}

void InterfaceApplet::toolbar_toggled_cb() {
  toolbartype.set_sensitive(toolbar.get_active());
}

/* TabsApplet */
TabsApplet::TabsApplet(Conf &_conf) : Applet::Applet(_conf) {
  showtabs.set_use_underline();
  tabsmenu.set_use_underline();
  scrolltabs.set_use_underline();
  showtabs.set_label(_("_Show tabs"));
  tabsmenu.set_label(_("_Enable tabs menu"));
  scrolltabs.set_label(_("Scro_ll tabs"));
  tabspos_label.set_text(_("Show the tabs at the"));
  tabspos.append(_("Top"));
  tabspos.append(_("Bottom"));
  tabspos.append(_("Right"));
  tabspos.append(_("Left"));

  readonly_label.set_text(_("Color for read only tabs"));
  modified_label.set_text(_("Color for modified tabs"));
  normal_label.set_text(_("Color for normal tabs"));

  Gdk::Color readonly_color, modified_color, normal_color;

  readonly_color.set_red(_conf.get("readonly_red", 0));
  readonly_color.set_green(_conf.get("readonly_green", 0));
  readonly_color.set_blue(_conf.get("readonly_blue", 65535));

  modified_color.set_red(_conf.get("modified_red", 65535));
  modified_color.set_green(_conf.get("modified_green", 0));
  modified_color.set_blue(_conf.get("modified_blue", 0));

  normal_color.set_red(_conf.get("normal_red", 0));
  normal_color.set_green(_conf.get("normal_green", 0));
  normal_color.set_blue(_conf.get("normal_blue", 0));

  readonly.set_color(readonly_color);
  modified.set_color(modified_color);
  normal.set_color(normal_color);

  box.pack_start(showtabs, false, false);
  box.pack_start(tabsmenu, false, false);
  box.pack_start(scrolltabs, false, false);
  box.pack_start(tabs_table, true, true);

  tabs_table.attach(tabspos_label, 0, 1, 0, 1,
                    Gtk::AttachOptions(Gtk::FILL | Gtk::EXPAND),
                    Gtk::AttachOptions(Gtk::SHRINK));
  tabs_table.attach(tabspos, 1, 2, 0, 1,
                    Gtk::AttachOptions(Gtk::FILL | Gtk::EXPAND),
                    Gtk::AttachOptions(Gtk::SHRINK));
  tabs_table.attach(readonly_label, 0, 1, 1, 2,
                    Gtk::AttachOptions(Gtk::FILL | Gtk::EXPAND),
                    Gtk::AttachOptions(Gtk::SHRINK));
  tabs_table.attach(readonly, 1, 2, 1, 2,
                    Gtk::AttachOptions(Gtk::FILL | Gtk::EXPAND),
                    Gtk::AttachOptions(Gtk::SHRINK));
  tabs_table.attach(modified_label, 0, 1, 3, 4,
                    Gtk::AttachOptions(Gtk::FILL | Gtk::EXPAND),
                    Gtk::AttachOptions(Gtk::SHRINK));
  tabs_table.attach(modified, 1, 2, 3, 4,
                    Gtk::AttachOptions(Gtk::FILL | Gtk::EXPAND),
                    Gtk::AttachOptions(Gtk::SHRINK));
  tabs_table.attach(normal_label, 0, 1, 5, 6,
                    Gtk::AttachOptions(Gtk::FILL | Gtk::EXPAND),
                    Gtk::AttachOptions(Gtk::SHRINK));
  tabs_table.attach(normal, 1, 2, 5, 6,
                    Gtk::AttachOptions(Gtk::FILL | Gtk::EXPAND),
                    Gtk::AttachOptions(Gtk::SHRINK));

  showtabs.set_active(_conf.get("showtabs", true));
  tabsmenu.set_active(_conf.get("tabsmenu", true));
  scrolltabs.set_active(_conf.get("scrolltabs", true));
  switch (_conf.get("tabspos", TABS_POS_TOP)) {
  case TABS_POS_BOTTOM:
    tabspos.set_active_text(_("Bottom"));
    break;
  case TABS_POS_RIGHT:
    tabspos.set_active_text(_("Right"));
    break;
  case TABS_POS_LEFT:
    tabspos.set_active_text(_("Left"));
    break;
  default:
    tabspos.set_active_text(_("Top"));
    break;
  }

  showtabs.signal_toggled().connect(
      sigc::mem_fun(*this, &TabsApplet::showtabs_toggled_cb));
  showtabs.toggled();
}

void TabsApplet::apply() {
  _conf.set("showtabs", showtabs.get_active());
  _conf.set("tabsmenu", tabsmenu.get_active());
  _conf.set("scrolltabs", scrolltabs.get_active());

  int i;
  switch (tabspos.get_active_row_number()) {
  case 1:
    i = TABS_POS_BOTTOM;
    break;
  case 2:
    i = TABS_POS_RIGHT;
    break;
  case 3:
    i = TABS_POS_LEFT;
    break;
  default:
    i = TABS_POS_TOP;
    break;
  }
  _conf.set("tabspos", i);
  Gdk::Color color = readonly.get_color();
  _conf.set("readonly_red", color.get_red());
  _conf.set("readonly_green", color.get_green());
  _conf.set("readonly_blue", color.get_blue());
  color = modified.get_color();
  _conf.set("modified_red", color.get_red());
  _conf.set("modified_green", color.get_green());
  _conf.set("modified_blue", color.get_blue());
  color = normal.get_color();
  _conf.set("normal_red", color.get_red());
  _conf.set("normal_green", color.get_green());
  _conf.set("normal_blue", color.get_blue());
}

void TabsApplet::showtabs_toggled_cb() {
  tabspos.set_sensitive(showtabs.get_active());
}

/* EditorApplet */
EditorApplet::EditorApplet(Conf &_conf)
    : Applet::Applet(_conf), tab_width_adj(Gtk::Adjustment::create(0, 0, 10)) {

  textwrap.set_use_underline();
  linenumbers.set_use_underline();
  default_font.set_use_underline();

  highlight_auto.set_use_underline();

  textwrap.set_label(_("Text _Wrapping is on by default"));
  linenumbers.set_label(_("Show _Line numbers by default"));
  default_font.set_label(_("Use the default _font"));

  highlight_auto.set_label(
      _("_Automatic syntax highlighting for known file types"));

  linenumbers_label.set_text(_("Line numbers position"));
  linenumbers_pos.append(_("Right"));
  linenumbers_pos.append(_("Left"));
  linenumbers_pos.append(_("Both"));
  font_frame.set_label(_("Font"));

  tab_width_label.set_text(_("Tab width"));
  tab_width.set_adjustment(tab_width_adj);

  box.pack_start(textwrap, false, false);
  box.pack_start(linenumbers, false, false);
  box.pack_start(ed_box1, false, false);
  box.pack_start(font_frame, false, false);

  box.pack_start(highlight_auto, false, false);

  box.pack_start(ed_box3, false, false);

  font_frame.add(ed_box2);
  ed_box2.pack_start(default_font);
  ed_box2.pack_start(font);
  ed_box1.pack_start(linenumbers_label);
  ed_box1.pack_start(linenumbers_pos);
  ed_box3.pack_start(tab_width_label);
  ed_box3.pack_start(tab_width);

  textwrap.set_active(_conf.get("textwrap", true));
  linenumbers.set_active(_conf.get("linenumbers", false));
  default_font.set_active(_conf.get("default_font", true));

  highlight_auto.set_active(_conf.get("highlight_auto", true));

  bool r = _conf.get("numbers_right", true);
  bool l = _conf.get("numbers_left", true);
  if ((l) && (r)) {
    linenumbers_pos.set_active_text(_("Both"));
  } else if (l) {
    linenumbers_pos.set_active_text(_("Left"));
  } else if (r) {
    linenumbers_pos.set_active_text(_("Right"));
  } else {
    linenumbers_pos.set_active_text(_("Both"));
  }

  std::string _df = Utils::katoob_get_default_font();
  std::string def_font = _conf.get("font", _df);
  font.set_font_name(def_font);

  tab_width_adj->set_value(_conf.get("tab_width", 8));

  linenumbers.signal_toggled().connect(
      sigc::mem_fun(*this, &EditorApplet::linenumbers_toggled_cb));
  default_font.signal_toggled().connect(
      sigc::mem_fun(*this, &EditorApplet::default_font_toggled_cb));
  linenumbers.toggled();
  default_font.toggled();
}

void EditorApplet::apply() {
  _conf.set("tab_width", tab_width.get_value_as_int());
  _conf.set("textwrap", textwrap.get_active());
  _conf.set("linenumbers", linenumbers.get_active());
  _conf.set("default_font", default_font.get_active());
  _conf.set("font", font.get_font_name().c_str());

  _conf.set("highlight_auto", highlight_auto.get_active());

  bool r, l;
  switch (linenumbers_pos.get_active_row_number()) {
  case 0:
    l = false;
    r = true;
    break;
  case 1:
    l = true;
    r = false;
    break;
  default:
    l = true;
    r = true;
    break;
  }
  _conf.set("numbers_right", r);
  _conf.set("numbers_left", l);
}

void EditorApplet::linenumbers_toggled_cb() {
  linenumbers_pos.set_sensitive(linenumbers.get_active());
}

void EditorApplet::default_font_toggled_cb() {
  font.set_sensitive(!default_font.get_active());
}

/* EncodingsApplet */
EncodingsApplet::EncodingsApplet(Conf &_conf, Encodings &enc)
    : Applet::Applet(_conf), _enc(enc) {

  locale_enc.set_use_underline();
  locale_enc.set_label(_("_Use the locale encoding"));
  saved_enc_label.set_text(_("Specify an encoding"));
  save_enc_label.set_text(_("Specify an encoding"));

  locale_enc.set_active(_conf.get("locale_enc", false));

  saved_frame.set_label(_("File open encoding"));
  save_frame.set_label(_("File save encoding"));

  box.pack_start(saved_frame, false, false);
  box.pack_start(save_frame, false, false);

  saved_frame.add(enc_box3);
  enc_box3.pack_start(locale_enc, false, false);
  enc_box3.pack_start(enc_box1, false, false);
  save_frame.add(enc_box2);
  enc_box1.pack_start(saved_enc_label, false, false);
  enc_box1.pack_start(saved_enc, false, false);

  enc_box2.pack_start(save_enc_label, false, false);
  enc_box2.pack_start(save_enc, false, false);

  for (int i = 0; i < _enc.size(); i++) {
    if (i != _enc.utf8()) {
      saved_enc.append(_enc.at(i));
    }
  }
  for (int i = 0; i < _enc.size(); i++) {
    save_enc.append(_enc.at(i));
  }

  saved_enc.set_active(_enc.default_open());
  save_enc.set_active(_enc.default_save());

  locale_enc.signal_toggled().connect(
      sigc::mem_fun(*this, &EncodingsApplet::locale_enc_toggled_cb));
  locale_enc.toggled();
}

void EncodingsApplet::apply() {
  _conf.set("save_enc",
            _enc.get_charset(save_enc.get_active_row_number()).c_str());
  _conf.set("saved_enc",
            _enc.get_charset(saved_enc.get_active_row_number()).c_str());
  _conf.set("locale_enc", locale_enc.get_active());
  _conf.defaults(_enc);
}

void EncodingsApplet::locale_enc_toggled_cb() {
  saved_enc.set_sensitive(!locale_enc.get_active());
}

/* FileSaveApplet */
FileSaveApplet::FileSaveApplet(Conf &_conf) : Applet::Applet(_conf) {
  backup.set_use_underline();
  backup.set_label(_("_Backup files before saving."));
  backup_label.set_text(_("Backup extension"));

  box.pack_start(backup, false, false);
  box.pack_start(fs_box1, false, false);
  fs_box1.pack_start(backup_label);
  fs_box1.pack_start(backup_ext);

  backup.set_active(_conf.get("backup", true));
  backup_ext.set_text(_conf.get("backup_ext", "~"));

  backup.signal_toggled().connect(
      sigc::mem_fun(*this, &FileSaveApplet::backup_toggled_cb));
  backup.toggled();
}

void FileSaveApplet::apply() {
  _conf.set("backup", backup.get_active());
  if (backup_ext.get_text().size() == 0) {
    backup_ext.set_text("~");
  }
  _conf.set("backup_ext", backup_ext.get_text().c_str());
}

void FileSaveApplet::backup_toggled_cb() {
  backup_ext.set_sensitive(backup.get_active());
}

/* SpellCheckerApplet */
SpellCheckerApplet::SpellCheckerApplet(Conf &_conf) : Applet::Applet(_conf) {
  spell_check.set_use_underline();

  spell_check.set_label(_("_Autocheck Spelling"));
  //  misspelled_label.set_text(_("Misspelled words color"));
  default_dict_label.set_text(_("Choose the default dictionary"));

  box.pack_start(spell_check, false, false);
  //  box.pack_start(sc_box1, false, false);
  box.pack_start(sc_box2, false, false);
  //  sc_box1.pack_start(misspelled_label);
  //  sc_box1.pack_start(misspelled);
  sc_box2.pack_start(default_dict_label);
  sc_box2.pack_start(default_dict);

  spell_check.set_active(_conf.get("spell_check", true));
  default_dict.set_active_text(_conf.get("default_dict", "en"));
  /*
    Gdk::Color color;
    color.set_red(_conf.get("misspelled_red", 65535));
    color.set_green(_conf.get("misspelled_green", 0));
    color.set_blue(_conf.get("misspelled_blue", 0));

    misspelled.set_color(color);
    misspelled.set_use_alpha(false);
  */
  //  spell_check.signal_toggled().connect(sigc::mem_fun(*this,
  //  &PreferencesDialog::spell_check_toggled_cb));
  spell_check.toggled();
}

void SpellCheckerApplet::apply() {
  _conf.set("spell_check", spell_check.get_active());
  _conf.set("default_dict", default_dict.get_active_text().c_str());

  /*
    color = misspelled.get_color();
    _conf.set("misspelled_red", color.get_red());
    _conf.set("misspelled_green", color.get_green());
    _conf.set("misspelled_blue", color.get_blue());
  */
}

/*
void SpellCheckerApplet::spell_check_toggled_cb() {
  misspelled.set_sensitive(spell_check.get_active());
}
*/

/* PrintApplet */
PrintApplet::PrintApplet(Conf &_conf)
    : Applet::Applet(_conf)
//  dpi_x_adj(0,0,0),
//  dpi_y_adj(0,0,0),
{
  print_label.set_text(_("Printing font"));

#if 0
  gui_show_advanced.set_use_underline();
  gui_show_advanced.set_label(_("_Enable advanced options"));

  dpi_x_label.set_use_underline();
  dpi_y_label.set_use_underline();
  dpi_x_label.set_text(_("X Dot Per Inch"));
  dpi_y_label.set_text(_("Y Dot Per Inch"));

  dpi_x_adj.set_lower(150);
  dpi_x_adj.set_upper(1200);
  dpi_x_adj.set_value(_conf.print_get("dpi_x", 150));
  dpi_y_adj.set_lower(150);
  dpi_y_adj.set_upper(1200);
  dpi_y_adj.set_value(_conf.print_get("dpi_y", 150));

  dpi_x.set_adjustment(dpi_x_adj);
  dpi_y.set_adjustment(dpi_y_adj);

  gui_show_advanced.set_active(_conf.print_get("gui_show_advanced", false));
#endif

  print_font.set_font_name(_conf.print_get("print_font", "Sans Regular 12"));

  box.pack_start(p_box2, false, false);
  p_box2.pack_start(print_label, true, true);
  p_box2.pack_start(print_font, true, true);
#if 0
  box.pack_start(gui_show_advanced, false, false);

  box.pack_start(print_frame, false, false);

  print_frame.add(print_table);
  print_table.attach(dpi_x_label, 0, 1, 0, 1);
  print_table.attach(dpi_x, 1, 2, 0, 1);
  print_table.attach(dpi_y_label, 0, 1, 1, 2);
  print_table.attach(dpi_y, 1, 2, 1, 2);

  gui_show_advanced.signal_toggled().connect(sigc::mem_fun(*this, &PreferencesDialog::gui_show_advanced_toggled_cb));
  gui_show_advanced.toggled();
#endif
}

void PrintApplet::apply() {
  //  _conf.print_set("gui_show_advanced", gui_show_advanced.get_active());
  //  _conf.print_set("dpi_x", dpi_x.get_value_as_int());
  //  _conf.print_set("dpi_y", dpi_y.get_value_as_int());
  _conf.print_set("print_font", print_font.get_font_name().c_str());
}

/*
void PrintApplet::gui_show_advanced_toggled_cb() {
print_frame.set_sensitive(gui_show_advanced.get_active());
}
*/

/* DictionaryApplet */
DictionaryApplet::DictionaryApplet(Conf &_conf)
    : Applet::Applet(_conf), handle(NULL),
      dict_port_adj(Gtk::Adjustment::create(0, 0, 0)) {

  dict.set_use_underline();
  dict.set_label(_("_Enable the dictionary."));
  dict_host_label.set_text(_("Host"));
  list_dicts.set_label(_("_List dictionaries"));
  list_dicts.set_use_underline();
  dict_port_label.set_label(_("Port"));

  dict_port_adj->set_lower(0);
  dict_port_adj->set_upper(65535);
  dict_port_adj->set_value(_conf.get("dict_port", 2628));

  dict_port.set_adjustment(dict_port_adj);

  dict.set_active(_conf.get("dict", true));
  dict_host.set_text(_conf.get("dict_host", "dict.arabeyes.org"));

  dict_name_label.set_text(_("Dictionary"));
  dict_sw.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  dict_sw.add(dicts);

  dict_record.add(dict_name);
  dict_record.add(dict_description);
  dict_store = Gtk::ListStore::create(dict_record);
  dicts.set_model(dict_store);
  dicts.append_column(_("Dictionary"), dict_name);
  dicts.append_column(_("Description"), dict_description);
  dict_selection = dicts.get_selection();

  Gtk::TreeModel::Row row = *(dict_store->append());
  row[dict_name] = _conf.get("dict_db", "arabic");
  row[dict_description] = _("Not Available.");
  dict_selection->select(row);

  box.pack_start(dict, false, false);
  box.pack_start(dictionary_table);
  dictionary_table.set_col_spacing(0, 5);

  dictionary_table.attach(dict_host_label, 0, 1, 0, 1,
                          Gtk::AttachOptions(Gtk::SHRINK));
  dictionary_table.attach(dict_host, 1, 2, 0, 1);
  dictionary_table.attach(list_dicts, 2, 3, 0, 1,
                          Gtk::AttachOptions(Gtk::SHRINK),
                          Gtk::AttachOptions(Gtk::SHRINK));
  dictionary_table.attach(dict_port_label, 0, 1, 1, 2,
                          Gtk::AttachOptions(Gtk::SHRINK));
  dictionary_table.attach(dict_port, 1, 2, 1, 2);
  dictionary_table.attach(dict_name_label, 0, 1, 2, 3,
                          Gtk::AttachOptions(Gtk::SHRINK));
  dictionary_table.attach(dict_sw, 1, 2, 2, 3);

  dict.signal_toggled().connect(
      sigc::mem_fun(*this, &DictionaryApplet::dict_toggled_cb));
  dict.toggled();
  list_dicts.signal_clicked().connect(
      sigc::mem_fun(*this, &DictionaryApplet::list_dicts_clicked_cb));
}

void DictionaryApplet::apply() {
  _conf.set("dict", dict.get_active());
  _conf.set("dict_host", dict_host.get_text().c_str());
  _conf.set("dict_port", dict_port.get_value_as_int());

  Gtk::TreeModel::iterator iter = dict_selection->get_selected();
  if (iter) {
    Gtk::TreeModel::Row row = *iter;
    Glib::ustring word = row[dict_name];
    _conf.set("dict_db", word.c_str());
  }
}

void DictionaryApplet::dict_toggled_cb() {
  dictionary_table.set_sensitive(dict.get_active());
}

void DictionaryApplet::signal_cancel_clicked_cb() {
  Network::del_transfer(handle);
  handle = NULL;
  conn.disconnect();
}

void DictionaryApplet::signal_dict_transfer_complete_cb(
    bool st, const std::string &str) {
  // stop the progress indicator.
  meter->quit();

  conn.disconnect();

  if (!st) {
    katoob_error(str);
    return;
  }

  std::map<std::string, std::string> res;
  if (!Dict::parse_dbs(str, res)) {
    if (res.size() == 0) {
      katoob_error(_("Failed to get the available dictionaries."));
    } else if (res.begin()->first.size() == 0) {
      katoob_error(_("Failed to get the available dictionaries."));
    } else {
      katoob_error(res.begin()->first);
    }
  } else {
    dict_store->clear();
    Gtk::TreeModel::Row _row;
    std::map<std::string, std::string>::iterator iter;
    for (iter = res.begin(); iter != res.end(); iter++) {
      Gtk::TreeModel::Row row = *(dict_store->append());
      if (iter == res.begin()) {
        _row = row;
      }
      row[dict_name] = iter->first;
      row[dict_description] = iter->second;
    }
    dict_selection->select(_row);
  }
}

void DictionaryApplet::list_dicts_clicked_cb() {
  std::string host(dict_host.get_text());
  int port = dict_port.get_value_as_int();

  if (host.size() == 0) {
    katoob_error(_("Please provide a dict server"));
    return;
  }

  sigc::slot<void, bool, const std::string &> slot =
      sigc::mem_fun(this, &DictionaryApplet::signal_dict_transfer_complete_cb);

  std::string uri = Dict::construct_lsdb_uri(_conf, host, port);

  std::string error;
  if (!Network::add_transfer(uri, error, slot, handle)) {
    katoob_error(error);
  }

  meter = katoob_activity();

  conn = meter->signal_cancel_clicked.connect(
      sigc::mem_fun(this, &DictionaryApplet::signal_cancel_clicked_cb));

  meter->run();

  return;
}

/* MultipressApplet */
MultipressApplet::MultipressApplet(Conf &_conf)
    : Applet::Applet(_conf),
      multipress_timeout_adj(Gtk::Adjustment::create(0, 0, 0)) {

  multipress_timeout_l.set_use_underline();
  multipress_timeout_l.set_label(
      _("_Milliseconds before multipress will accept the current value."));
  multipress_timeout.set_adjustment(multipress_timeout_adj);
  multipress_timeout_adj->set_upper(2000);
  multipress_timeout_adj->set_lower(200);
  multipress_timeout_adj->set_value(_conf.get("multipress_timeout", 1000));
  m_box1.pack_start(multipress_timeout_l);
  m_box1.pack_start(multipress_timeout);
  box.pack_start(m_box1, false, false);
}

void MultipressApplet::apply() {
  _conf.set("multipress_timeout", multipress_timeout.get_value_as_int());
}

/* RemoteDocumentsApplet */
RemoteDocumentsApplet::RemoteDocumentsApplet(Conf &_conf)
    : Applet::Applet(_conf),
      locations_size_adj(Gtk::Adjustment::create(0, 0, 0)) {

  box.pack_start(open_location_to_active, false, false);
  box.pack_start(r_box1, false, false);
  box.pack_start(r_box2, false, false);
  r_box1.pack_start(locations_size_label);
  r_box1.pack_start(locations_size);

  locations_size.set_adjustment(locations_size_adj);
  locations_size_adj->set_upper(1000);
  locations_size_adj->set_lower(0);
  locations_size_adj->set_value(_conf.get("locations_size", 10));
  locations_size_label.set_text(
      _("Number of remote addresses to save\n(0 for unlimited)"));
  open_location_to_active.set_use_underline();
  open_location_to_active.set_active(
      _conf.get("open_location_to_active", false));
  open_location_to_active.set_label(
      _("_Insert into the active document enabled by default ?"));
}

void RemoteDocumentsApplet::apply() {
  _conf.set("open_location_to_active", open_location_to_active.get_active());
  _conf.set("locations_size", locations_size.get_value_as_int());
}

/* AdvancedApplet */
AdvancedApplet::AdvancedApplet(Conf &_conf)
    : Applet::Applet(_conf), x_adj(Gtk::Adjustment::create(0, 0, 0)),
      y_adj(Gtk::Adjustment::create(0, 0, 0)),
      w_adj(Gtk::Adjustment::create(0, 0, 0)),
      h_adj(Gtk::Adjustment::create(0, 0, 0)) {
  x_label.set_text(_("Left"));
  y_label.set_text(_("Top"));
  w_label.set_text(_("Width"));
  h_label.set_text(_("Height"));

  saveonexit.set_label(_("Save all the program _settings on exit"));
  savewinpos.set_label(_("Save the main window _attributes on exit"));

  saveonexit.set_use_underline();
  savewinpos.set_use_underline();

  pos.set_label(_("Window position"));
  size.set_label(_("Window size"));
  x.set_adjustment(x_adj);
  y.set_adjustment(y_adj);
  w.set_adjustment(w_adj);
  h.set_adjustment(h_adj);

  box.pack_start(pos, false, false);
  box.pack_start(size, false, false);
  box.pack_start(savewinpos, false, false);
  box.pack_start(saveonexit, false, false);

  pos.add(pos_table);
  size.add(size_table);

  pos_table.attach(x_label, 0, 1, 0, 1);
  pos_table.attach(x, 1, 2, 0, 1);
  pos_table.attach(y_label, 0, 1, 1, 2);
  pos_table.attach(y, 1, 2, 1, 2);

  size_table.attach(w_label, 0, 1, 0, 1);
  size_table.attach(w, 1, 2, 0, 1);
  size_table.attach(h_label, 0, 1, 1, 2);
  size_table.attach(h, 1, 2, 1, 2);

  Glib::RefPtr<Gdk::Screen> screen = Gdk::Screen::get_default();
  x_adj->set_upper(screen->get_width() - 10);
  y_adj->set_upper(screen->get_height() - 10);
  h_adj->set_upper(screen->get_height());
  w_adj->set_upper(screen->get_width());
  w_adj->set_lower(400);
  h_adj->set_lower(400);

  x_adj->set_value(_conf.get("x", 50));
  y_adj->set_value(_conf.get("y", 50));
  h_adj->set_value(_conf.get("h", 400));
  w_adj->set_value(_conf.get("w", 500));

  saveonexit.set_active(_conf.get("saveonexit", true));
  savewinpos.set_active(_conf.get("savewinpos", true));

  savewinpos.signal_toggled().connect(
      sigc::mem_fun(*this, &AdvancedApplet::savewinpos_toggled_cb));
  savewinpos.toggled();
}

void AdvancedApplet::apply() {
  _conf.set("saveonexit", saveonexit.get_active());
  _conf.set("savewinpos", savewinpos.get_active());
  _conf.set("x", x.get_value_as_int());
  _conf.set("y", y.get_value_as_int());
  _conf.set("w", w.get_value_as_int());
  _conf.set("h", h.get_value_as_int());
}

void AdvancedApplet::savewinpos_toggled_cb() {
  bool active = savewinpos.get_active();
  size.set_sensitive(active);
  pos.set_sensitive(active);
}

/* NetworkApplet */
NetworkApplet::NetworkApplet(Conf &_conf)
    : Applet(_conf), timeout_adj(Gtk::Adjustment::create(0, 0, 0)),
      proxyport_adj(Gtk::Adjustment::create(0, 0, 0)) {

  timeout.set_adjustment(timeout_adj);
  proxyport.set_adjustment(proxyport_adj);

  timeout_l.set_text(_("Network timeout (In seconds):"));
  proxyhost_l.set_text(_("Proxy host:"));
  proxyport_l.set_text(_("Proxy port:"));
  proxytype_l.set_text(_("Proxy type:"));

  proxytype.append(_("None"));
  proxytype.append(_("Get from the environment"));
  proxytype.append(_("HTTP"));
  proxytype.append(_("SOCKS4"));
  proxytype.append(_("SOCKS5"));

  table.attach(timeout_l, 0, 1, 0, 1);
  table.attach(timeout, 1, 2, 0, 1);

  table.attach(proxytype_l, 0, 1, 1, 2);
  table.attach(proxytype, 1, 2, 1, 2);

  table.attach(proxyhost_l, 0, 1, 2, 3);
  table.attach(proxyhost, 1, 2, 2, 3);

  table.attach(proxyport_l, 0, 1, 3, 4);
  table.attach(proxyport, 1, 2, 3, 4);

  proxytunnel.set_label(_("Tunnel traffic through the proxy"));

  proxyauth.set_label(_("Use proxy authentication"));
  proxyuser_l.set_text(_("Proxy username:"));
  proxypass_l.set_text(_("Proxy password:"));
  proxypass.set_visibility(false);

  table2.attach(proxyuser_l, 0, 1, 0, 1);
  table2.attach(proxyuser, 1, 2, 0, 1);

  table2.attach(proxypass_l, 0, 1, 1, 2);
  table2.attach(proxypass, 1, 2, 1, 2);

  box.pack_start(table, false, false);
  box.pack_start(proxyauth, false, false);
  box.pack_start(table2, false, false);

  box.pack_start(proxytunnel, false, false);

  timeout_adj->set_upper(600);
  timeout_adj->set_lower(0);
  timeout_adj->set_value(_conf.get("timeout", 180));

  proxyport_adj->set_upper(65535);
  proxyport_adj->set_lower(1);
  proxyport_adj->set_value(_conf.get("proxyport", 8080));

  proxyhost.set_text(_conf.get("proxyhost", ""));

  proxytype.set_active(_conf.get("proxytype", 0));

  proxytunnel.set_active(_conf.get("proxytunnel", false));

  proxyauth.set_active(_conf.get("proxyauth", false));
  proxyuser.set_text(_conf.get("proxyuser", ""));
  proxypass.set_text(_conf.get("proxypass", ""));
}

void NetworkApplet::apply() {
  // TODO: Password is saved as plain text.
  _conf.set("timeout", timeout.get_value_as_int());
  _conf.set("proxyport", proxyport.get_value_as_int());
  _conf.set("proxytype", proxytype.get_active_row_number());
  _conf.set("proxyhost", proxyhost.get_text().c_str());
  _conf.set("proxytunnel", proxytunnel.get_active());
  _conf.set("proxyuser", proxyuser.get_text().c_str());
  _conf.set("proxypass", proxypass.get_text().c_str());
  _conf.set("proxyauth", proxyauth.get_active());
}

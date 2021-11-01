/*
 * applets.hh
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

#pragma once

#include "conf.hh"
#include "dialogs.hh"
#include "glibmm/refptr.h"
#include "spellmenu.hh"
#include <gtkmm.h>

class Applet {
public:
  Applet(Conf &);
  virtual ~Applet();
  virtual Gtk::Widget &get_box();
  virtual void apply() = 0;

protected:
  Gtk::VBox box;
  Conf &_conf;
};

class GeneralApplet : public Applet {
public:
  GeneralApplet(Conf &);
  virtual void apply();

private:
  void undo_toggled_cb();
  void undo_closed_toggled_cb();

  Gtk::CheckButton undo, undo_closed;
  Gtk::Table general_table1, general_table2;
  Gtk::Label undo_label, exec_label, undo_closed_label;
  Glib::RefPtr<Gtk::Adjustment> undono_adj, exec_adj, undo_closed_adj;
  Gtk::SpinButton undono, exec_cmd_size, undo_closedno;
};

class InterfaceApplet : public Applet {
public:
  InterfaceApplet(Conf &);
  virtual void apply();

private:
  void recent_toggled_cb();
  void toolbar_toggled_cb();

  Gtk::CheckButton toolbar, extended_toolbar, statusbar, recent, showclose,
      extra_buttons;
  Glib::RefPtr<Gtk::Adjustment> recentno_adj;
  Gtk::SpinButton recentno;
  Gtk::Label recentno_label, toolbartype_label;
  Gtk::Table interface_table;
  Gtk::ComboBoxText toolbartype;
};

class TabsApplet : public Applet {
public:
  TabsApplet(Conf &);
  virtual void apply();

private:
  void showtabs_toggled_cb();

  Gtk::CheckButton showtabs, tabsmenu, scrolltabs;
  Gtk::Label tabspos_label, readonly_label, modified_label, normal_label;
  Gtk::ComboBoxText tabspos;
  Gtk::Table tabs_table;
  Gtk::ColorButton readonly, modified, normal;
};

class EditorApplet : public Applet {
public:
  EditorApplet(Conf &);
  virtual void apply();

private:
  void linenumbers_toggled_cb();
  void default_font_toggled_cb();

  Gtk::CheckButton textwrap, linenumbers, default_font;
  Gtk::FontButton font;
  Gtk::ComboBoxText linenumbers_pos;
  Gtk::Frame font_frame;
  Gtk::Label linenumbers_label;
  Gtk::HBox ed_box1;
  Gtk::VBox ed_box2;
  Gtk::HBox ed_box3;
  Gtk::SpinButton tab_width;
  Glib::RefPtr<Gtk::Adjustment> tab_width_adj;
  Gtk::Label tab_width_label;
  Gtk::CheckButton highlight_auto;
};

class EncodingsApplet : public Applet {
public:
  EncodingsApplet(Conf &, Encodings &);
  virtual void apply();

private:
  void locale_enc_toggled_cb();

  Encodings &_enc;

  Gtk::ComboBoxText save_enc, saved_enc;
  Gtk::CheckButton locale_enc;
  Gtk::Label saved_enc_label, save_enc_label;
  Gtk::HBox enc_box1, enc_box2;
  Gtk::VBox enc_box3;
  Gtk::Frame save_frame, saved_frame;
};

class FileSaveApplet : public Applet {
public:
  FileSaveApplet(Conf &);
  virtual void apply();

private:
  void backup_toggled_cb();

  Gtk::CheckButton backup;
  Gtk::Entry backup_ext;
  Gtk::Label backup_label;
  Gtk::HBox fs_box1;
};

class SpellCheckerApplet : public Applet {
public:
  SpellCheckerApplet(Conf &);
  virtual void apply();

private:
  Gtk::CheckButton spell_check;
  SpellMenu default_dict;
  Gtk::Label default_dict_label; //, misspelled_label;
  Gtk::HBox /*sc_box1,*/ sc_box2;
  //  Gtk::ColorButton misspelled;
};

class PrintApplet : public Applet {
public:
  PrintApplet(Conf &);
  virtual void apply();

private:
  Gtk::Label print_label;
  Gtk::FontButton print_font;
  Gtk::HBox p_box2;
};

class DictionaryApplet : public Applet {
public:
  DictionaryApplet(Conf &);
  virtual void apply();

private:
  void dict_toggled_cb();
  void list_dicts_clicked_cb();
  void signal_dict_transfer_complete_cb(bool, const std::string &);
  void signal_cancel_clicked_cb();

  sigc::connection conn;

  Glib::RefPtr<ActivityMeter> meter;

  // our network handle
  void *handle;

  Gtk::CheckButton dict;
  Gtk::Label dict_host_label, dict_port_label, dict_name_label;
  Gtk::Entry dict_host;
  Gtk::SpinButton dict_port;
  Glib::RefPtr<Gtk::Adjustment> dict_port_adj;
  Gtk::Button list_dicts;
  Gtk::Table dictionary_table;
  // The tree view.
  Glib::RefPtr<Gtk::TreeSelection> dict_selection;
  Gtk::TreeView dicts;
  Gtk::TreeModelColumn<Glib::ustring> dict_name;
  Gtk::TreeModelColumn<Glib::ustring> dict_description;
  Gtk::TreeModelColumnRecord dict_record;
  Glib::RefPtr<Gtk::ListStore> dict_store;
  Gtk::ScrolledWindow dict_sw;
};

class MultipressApplet : public Applet {
public:
  MultipressApplet(Conf &);
  virtual void apply();

private:
  Glib::RefPtr<Gtk::Adjustment> multipress_timeout_adj;
  Gtk::SpinButton multipress_timeout;
  Gtk::HBox m_box1;
  Gtk::Label multipress_timeout_l;
};

class RemoteDocumentsApplet : public Applet {
public:
  RemoteDocumentsApplet(Conf &);
  virtual void apply();

private:
  Gtk::CheckButton open_location_to_active;
  Gtk::SpinButton locations_size;
  Glib::RefPtr<Gtk::Adjustment> locations_size_adj;
  Gtk::Label locations_size_label;
  Gtk::HBox r_box1, r_box2;
};

class AdvancedApplet : public Applet {
public:
  AdvancedApplet(Conf &);
  virtual void apply();

private:
  void savewinpos_toggled_cb();

  Gtk::CheckButton saveonexit, savewinpos;
  Gtk::Frame pos, size;
  Gtk::Label x_label, y_label, w_label, h_label;
  Glib::RefPtr<Gtk::Adjustment> x_adj, y_adj, w_adj, h_adj;
  Gtk::Table pos_table, size_table;
  Gtk::SpinButton x, y, w, h;
};

class NetworkApplet : public Applet {
public:
  NetworkApplet(Conf &);
  virtual void apply();

private:
  Glib::RefPtr<Gtk::Adjustment> timeout_adj, proxyport_adj;
  Gtk::SpinButton timeout, proxyport;
  Gtk::ComboBoxText proxytype;
  Gtk::Entry proxyhost, proxyuser, proxypass;
  Gtk::Label proxyhost_l, proxyport_l, timeout_l, proxytype_l, proxyuser_l,
      proxypass_l;
  Gtk::Table table;
  Gtk::Table table2;
  Gtk::CheckButton proxytunnel, proxyauth;
};

/*
 * preferencesdialog.cc
 * This file is part of katoob
 *
 * Copyright (C) 2006, 2007 Mohammed Sameer
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

#include <gtkmm/stock.h>
#include <gtkmm/enums.h>
#include "preferencesdialog.hh"
#include "macros.h"
#include "mdi.hh"
#include "utils.hh"
#include "dict.hh"
#include "dialogs.hh"

PreferencesDialog::PreferencesDialog(Conf& conf, Encodings& enc) :
  _conf(conf),
  _enc(enc),
  apply(Gtk::Stock::APPLY)
{
  dialog.set_title(_("Preferences"));
  dialog.set_modal(true);

  Gtk::VBox *vbox = dialog.get_vbox();
  vbox->pack_start(paned);

  sw.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  paned.pack1(sw, true, true);

  // Now for the tree view.
  record.add(section);
  //  record.add(number);
  store = Gtk::ListStore::create(record);
  treeview.set_model(store);
  treeview.append_column(_("Options for"), section);
  treeview.set_headers_visible(false);

  sw.add(treeview);
  sw.set_size_request(150, 300);
  selection = treeview.get_selection();
  selection->signal_changed().connect(sigc::mem_fun(*this, &PreferencesDialog::selection_signal_changed_cb));

  add_applet(_("General"), new GeneralApplet(_conf));
  add_applet(_("Interface"), new InterfaceApplet(_conf));
  add_applet(_("Tabs"), new TabsApplet(_conf));
  add_applet(_("Editor"), new EditorApplet(_conf));
  add_applet(_("Encodings"), new EncodingsApplet(_conf, _enc));
  add_applet(_("File Save"), new FileSaveApplet(_conf));
#ifdef ENABLE_SPELL
  add_applet(_("Spell Checker"), new SpellCheckerApplet(_conf));
#endif
#ifdef ENABLE_PRINT
  add_applet(_("Print"), new PrintApplet(_conf));
#endif
  add_applet(_("Dictionary"), new DictionaryApplet(_conf));
#ifdef ENABLE_MULTIPRESS
  add_applet(_("Multipress"), new MultipressApplet(_conf));
#endif
  add_applet(_("Remote Documents"),new RemoteDocumentsApplet(_conf));
  add_applet(_("Advanced"), new AdvancedApplet(_conf));
  add_applet(_("Network"), new NetworkApplet(_conf));

  paned.pack2(notebook, true, true);
  notebook.set_show_tabs(false);


  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  Gtk::ButtonBox* box = dialog.get_action_area();
  box->pack_start(apply);
  //  dialog.add_button(Gtk::Stock::APPLY, Gtk::RESPONSE_APPLY);
  dialog.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

  apply.signal_clicked().connect(sigc::mem_fun(*this, &PreferencesDialog::apply_clicked_cb));
}

void PreferencesDialog::add_applet(const std::string& name, Applet *applet) {
  applets[name] = applet;
  Gtk::TreeModel::Row row = *(store->append ());
  row[section] = name;
  notebook.append_page(applet->get_box());
}

PreferencesDialog::~PreferencesDialog() {
  std::map<std::string, Applet *>::iterator iter;
  for (iter = applets.begin(); iter != applets.end(); iter++) {
    delete iter->second;
  }
  applets.clear();
}

bool PreferencesDialog::run() {
  dialog.show_all();
  if (dialog.run() == Gtk::RESPONSE_OK) {
    repopulate_conf();
    return true;
  }
  return false;
}

void PreferencesDialog::selection_signal_changed_cb() {
  Gtk::TreeModel::iterator iter = selection->get_selected();
  if (iter) {
    Gtk::TreeModel::Row row = *iter;
    Glib::ustring name = row[section];
    int n = notebook.page_num(applets[name]->get_box());
    notebook.set_current_page(n);
  }
}

void PreferencesDialog::repopulate_conf() {
  std::map<std::string, Applet *>::iterator iter;
  for (iter = applets.begin(); iter != applets.end(); iter++) {
    iter->second->apply();
  }
  _conf.adjust_lists();
}

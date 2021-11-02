/*
 * spelldialog.cc
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

#include "spelldialog.hh"
#include "dialogs.hh"
#include "gdk/gdkkeysyms.h"
#include "macros.h"
#include <gtkmm.h>

// TODO: Add replace all
// Start checking from the insert mark not from the beginning of the document.

SpellDialog::SpellDialog(Document *doc)
    : close(Gtk::Stock::CLOSE), ignore(_("_Ignore"), true),
      ignore_all(_("Ignore _All"), true), change(_("C_hange"), true),
      //  change_all(_("Change A_ll"), true),
      check(Gtk::Stock::SPELL_CHECK), add(_("_Add to user dictionary"), true),
      misspelled(_("Misspelled:")), change_to(_("Change to:")),
      yes(Gtk::StockID(Gtk::Stock::YES), Gtk::IconSize(Gtk::ICON_SIZE_BUTTON)),
      no(Gtk::StockID(Gtk::Stock::NO), Gtk::IconSize(Gtk::ICON_SIZE_BUTTON)),
      _doc(doc) {
  set_title(_("Check Spelling"));
  set_modal(true);
  set_position(Gtk::WIN_POS_CENTER);
  Gtk::Window::add(vbox1);

  vbox1.pack_start(table, false, false, 10);
  vbox1.pack_start(hbox1);

  hbox1.pack_start(sw, Gtk::PACK_EXPAND_WIDGET, 5);
  hbox1.pack_start(vbox2);

  vbox1.pack_start(separator, false, false, 5);
  vbox1.pack_end(hbox3);
  vbox1.set_border_width(5);

  hbox3.set_border_width(5);
  hbox3.pack_end(close, false, false, 5);

  sw.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

  Gtk::Image *image = Gtk::manage(new Gtk::Image(
      Gtk::StockID(Gtk::Stock::ADD), Gtk::IconSize(Gtk::ICON_SIZE_BUTTON)));
  add.set_image(*image);
  vbox2.pack_start(change, false, false, 5);
  //  vbox2.pack_start(change_all, false, false, 5);
  vbox2.pack_start(ignore, false, false, 5);
  vbox2.pack_start(ignore_all, false, false, 5);
  vbox2.pack_start(add, false, false, 5);

  table.attach(misspelled, 0, 1, 0, 1, Gtk::FILL | Gtk::EXPAND,
               Gtk::FILL | Gtk::EXPAND, 5);
  table.attach(change_to, 0, 1, 1, 2, Gtk::FILL | Gtk::EXPAND,
               Gtk::FILL | Gtk::EXPAND, 5);
  table.attach(hbox2, 2, 3, 0, 1);
  table.attach(misspelled_word, 1, 2, 0, 1, Gtk::FILL | Gtk::EXPAND,
               Gtk::FILL | Gtk::EXPAND, 5);
  table.attach(entry, 1, 2, 1, 2, Gtk::FILL | Gtk::EXPAND,
               Gtk::FILL | Gtk::EXPAND, 5);
  table.attach(check, 2, 3, 1, 2);

  table.set_spacings(5);

  hbox2.pack_start(yes, false, false);
  hbox2.pack_start(no, false, false);

  record.add(suggestions_col);
  store = Gtk::ListStore::create(record);

  suggestions.set_model(store);
  suggestions.append_column(_("Suggestions"), suggestions_col);

  sw.add(suggestions);

  selection = suggestions.get_selection();
  selection->signal_changed().connect(
      sigc::mem_fun(*this, &SpellDialog::selection_signal_changed_cb));

  no.set_no_show_all();

  // our signals.
  close.signal_clicked().connect(
      sigc::mem_fun(*this, &SpellDialog::close_clicked_cb));
  ignore.signal_clicked().connect(
      sigc::mem_fun(*this, &SpellDialog::ignore_clicked_cb));
  ignore_all.signal_clicked().connect(
      sigc::mem_fun(*this, &SpellDialog::ignore_all_clicked_cb));
  change.signal_clicked().connect(
      sigc::mem_fun(*this, &SpellDialog::change_clicked_cb));
  //  change_all.signal_clicked().connect(sigc::mem_fun(*this,
  //  &SpellDialog::change_all_clicked_cb));
  check.signal_clicked().connect(
      sigc::mem_fun(*this, &SpellDialog::check_clicked_cb));
  add.signal_clicked().connect(
      sigc::mem_fun(*this, &SpellDialog::add_clicked_cb));
  signal_key_press_event().connect(
      sigc::mem_fun(this, &SpellDialog::signal_key_press_event_cb));
}

SpellDialog::~SpellDialog() {
  // NOTE: Recheck the whole document.
  // The user might've added new words to the dictionary.
  _doc->spell_dialog_helper_recheck();
}

void SpellDialog::close_clicked_cb() { loop->quit(); }

void SpellDialog::run() {
  std::string word;

  _doc->spell_dialog_mode();

  if (!_doc->spell_dialog_helper_has_misspelled(word)) {
    std::string message = _("No misspelled words.");
    katoob_info(message);
    return;
  }

  std::vector<std::string> suggestions;
  _doc->spell_dialog_helper_get_suggestions(word, suggestions);

  got_misspelled(word, suggestions);

  show_all();

  // NOTE: This is bad but it's needed otherwise the first word won't highlight.
  _doc->spell_dialog_mode();
  _doc->spell_dialog_helper_has_misspelled(word);

  loop = Glib::MainLoop::create();
  loop->run();
}

void SpellDialog::got_misspelled(std::string &word,
                                 std::vector<std::string> &suggestions) {
  misspelled_word.set_text(word);

  populate_suggestions(suggestions);
}

void SpellDialog::populate_suggestions(std::vector<std::string> &suggestions) {
  store->clear();
  Gtk::TreeModel::Row _row;
  for (unsigned x = 0; x < suggestions.size(); x++) {
    Gtk::TreeModel::Row row = *(store->append());
    if (x == 0)
      _row = row;
    row[suggestions_col] = suggestions[x];
  }
  if (suggestions.size() > 0) {
    selection->select(_row);
  }
}

void SpellDialog::selection_signal_changed_cb() {
  Gtk::TreeModel::iterator iter = selection->get_selected();
  if (iter) {
    Gtk::TreeModel::Row row = *iter;
    Glib::ustring word = row[suggestions_col];
    entry.set_text(word);
  }
}

void SpellDialog::next() {
  std::string word;

  if (!_doc->spell_dialog_helper_has_misspelled(word)) {
    loop->quit();
  }

  std::vector<std::string> suggestions;
  _doc->spell_dialog_helper_get_suggestions(word, suggestions);

  got_misspelled(word, suggestions);
}

void SpellDialog::ignore_clicked_cb() { next(); }

void SpellDialog::ignore_all_clicked_cb() {
  std::string old_word = misspelled_word.get_text();
  _doc->spell_dialog_helper_add_to_session(old_word);
  next();
}

void SpellDialog::add_clicked_cb() {
  std::string old_word = misspelled_word.get_text();
  _doc->spell_dialog_helper_add_to_personal(old_word);
  next();
}

void SpellDialog::change_clicked_cb() {
  std::string old_word = misspelled_word.get_text();
  std::string new_word = entry.get_text();
  if (new_word.length() > 0) {
    _doc->spell_dialog_helper_replace(old_word, new_word);
    next();
  }
}

// TODO: Add check_and_suggest() method to our speller class.
void SpellDialog::check_clicked_cb() {
  std::string word = entry.get_text();
  if (word.length() > 0) {
    if (_doc->spell_dialog_helper_check(word)) {
      yes.show();
      no.hide();
      store->clear();
    } else {
      no.show();
      yes.hide();
      std::vector<std::string> suggestions;
      _doc->spell_dialog_helper_get_suggestions(word, suggestions);
      populate_suggestions(suggestions);
    }
  }
}

bool SpellDialog::signal_key_press_event_cb(GdkEventKey *key) {
  if ((key) && (key->type == GDK_KEY_PRESS) && (key->state == 0) &&
      (key->keyval == GDK_KEY_Escape)) {
    close_clicked_cb();
    return true;
  }
  return false;
}

#if 0
void SpellDialog::change_all_clicked_cb() {
  std::string old_word = misspelled_word.get_text();
  std::string new_word = entry.get_text();
  if (new_word.length() > 0) {
    //      _doc->spell_dialog_helper_replace(old_word, new_word);
    next();
  }
}
#endif

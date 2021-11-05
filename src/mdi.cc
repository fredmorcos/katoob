/*
 * mdi.cc
 *
 * This file is part of Katoob.
 *
 * Copyright (C) 2008-2021 Fred Morcos <fm+Katoob@fredmorcos.com>
 * Copyright (C) 2002-2007 Mohammed Sameer <msameer@foolab.org>
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

#include "dialogs.hh"
#include "dict.hh"
#include "execdialog.hh"
#include "filedialog.hh"
#include "macros.h"
#include "mdi.hh"
#include "network.hh"
#include "openlocationdialog.hh"
#include "pipe.hh"
#include "searchdialog.hh"
#include "tempfile.hh"
#include "utils.hh"
#include <cassert>
#include <cerrno>
#include <cstring>
#include <iostream>
//#include "replacedialog.hh"

#ifdef ENABLE_SPELL
#include "spelldialog.hh"
#endif

#ifdef ENABLE_PRINT
#include "print.hh"
#endif

MDI::MDI(Conf &conf, Encodings &enc):
 _conf(conf),
 _encodings(enc)
#ifdef ENABLE_PRINT
 ,
 page_setup(PageSetup::create(_conf)),
 settings(PrintSettings::create(_conf))
#endif
{
  signal_switch_page().connect(sigc::mem_fun(*this, &MDI::signal_switch_page_cb));
  // 1 minute.
  Glib::signal_timeout().connect(sigc::mem_fun(this, &MDI::autosave), 1 * 60 * 1000);
}

MDI::~MDI()
{
  // destroy our documents.
  for (unsigned x = 0; x < children.size(); x++) {
    delete children[x];
  }

  for (unsigned x = 0; x < closed_children.size(); x++) {
    delete closed_children[x];
  }

  children.clear();
  closed_children.clear();
  page_setup->save();
  settings->save();
}

void MDI::scan_temp()
{
  std::map<std::string, std::string> temps;
  std::string error;
  if (!Utils::get_recovery_files(temps, error)) {
    katoob_error(Utils::substitute(_("Failed to scan for any autorecovery files: %s"), error));
    return;
  }

  if (temps.size() > 0) {
    if (katoob_simple_question(_("Some unrecovered files were found. Try to recover them ?"))) {
      for (std::map<std::string, std::string>::iterator iter = temps.begin(); iter != temps.end();
           iter++) {
        Document *doc = create_document();
        if (doc) {
          doc->set_text(iter->second);
          if (unlink(iter->first.c_str()) == -1) {
            std::cerr << "Failed to unlink " << iter->first << " " << std::strerror(errno)
                      << std::endl;
          }
        }
      }
    }
  }
}

bool MDI::autosave()
{
  Document *doc;
  for (unsigned x = 0; x < children.size(); x++) {
    doc = children[x];
    if (doc->get_readonly()) {
      continue;
    }
    if (!doc->get_modified()) {
      continue;
    }
    doc->autosave();
  }

  return true;
}

Document *MDI::create_document(std::string &file, int enc)
{
  if (enc == -1) {
    enc = _encodings.default_open();
  }

  int x = children.size();
  Document *doc;
  if (file == "-") {
    doc = new Document(_conf, _encodings, ++x, enc);
  } else {
    doc = new Document(_conf, _encodings, enc, file);
  }

  if (doc->ok()) {
    add_document(doc);
    _conf.open_dir(Glib::path_get_basename(file));

    // Ensure absolute path for the file.
    if (Glib::path_is_absolute(file)) {
      _conf.recent_prepend(file);
    } else {
      std::vector<std::string> elems;
      elems.push_back(Glib::get_current_dir());
      elems.push_back(file);
      file = Glib::build_path(Utils::get_dir_separator(), elems);
      elems.clear();
      _conf.recent_prepend(file);
    }

    signal_recent_regenerate.emit();
    return doc;
  } else {
    delete doc;
    return NULL;
  }
}

Document *MDI::create_document()
{
  int x = children.size();
  Document *doc = new Document(_conf, _encodings, ++x);
  if (doc->ok()) {
    add_document(doc);
    return doc;
  } else {
    delete doc;
    return NULL;
  }
}

void MDI::add_document(Document *doc, bool signals)
{
  // This should be first so when we get the switch_page signal we'll find something.
  children.push_back(doc);

  if (children.size() == 1) {
    signal_reset_gui.emit(0);
  }

  if (signals) {
    connect_signals(doc);
  }

  // This should be first otherwise the documents menu will try to manipulate
  // a menu that has not been added.
  signal_document_added.emit(doc->get_readonly(), doc->get_modified(), doc->get_title());

  append_page(*doc, doc->get_label());
  set_menu_label_text(*doc, doc->get_title());

  set_current_page(-1);

  // This is here because we can't put it in connect_signals() because the document
  // is appended after connect_signals()
  // We will connect to the label directly bypassing the Document object.
  dynamic_cast<Label *>(get_tab_label(*children[get_current_page()]))
      ->signal_close_clicked.connect(
          sigc::bind<Document *>(sigc::mem_fun(this, &MDI::signal_document_label_close_clicked_cb),
                                 doc));
}

Document *MDI::get_active()
{
  int x = get_current_page();
  return (x == -1 ? NULL : children[x]);
}

void MDI::open_cb()
{
  FileDialog dlg(_("Please select a file for editing."), FILE_OPEN, -1, _conf, _encodings);
  if (dlg.run()) {
    int enc = dlg.encoding();
    std::vector<std::string> files = dlg.get();
    for (unsigned int x = 0; x < files.size(); x++) {
      create_document(files[x], enc);
    }
  }
}

void MDI::open_location_cb()
{
  OpenLocationDialog dialog(_conf, _encodings);

  Document *doc = get_active();
  // disable the check if we have no active.
  if (!doc) {
    dialog.disable_insert_to_active();
  }

  bool to_active = false;

  std::string error;
  if (dialog.run(error)) {
    if (doc) {
      to_active = dialog.insert_to_active();
    }
    // get encoding,
    int enc = dialog.get_encoding();
    std::string uri = dialog.get_uri();
    sigc::slot<void, bool, const std::string &> slot =
        sigc::bind(sigc::mem_fun(this, &MDI::signal_transfer_complete_cb), uri, enc, to_active);
    std::string error;
    if (!Network::add_transfer(uri, error, slot)) {
      katoob_error(error);
    }
  } else if (error.size() > 0) {
    katoob_error(error);
  }
}

void MDI::signal_transfer_complete_cb(bool st,
                                      const std::string &str,
                                      const std::string uri,
                                      int enc,
                                      bool to_active)
{
  if (!st) {
    katoob_error(str);
    return;
  }

  // We convert it to utf8.
  std::string res;
  if (_encodings.convert_to(str, res, enc) != -1) {
    Document *doc = get_active();
    if (!doc) {
      doc = create_document();
    } else if (!to_active) {
      doc = create_document();
    }
    // Insert.
    if (doc) {
      // TODO: Set a title for the document.
      // We also need the document to know that it's a remote document.
      doc->insert(res);
    }
  } else {
    katoob_error(res);
  }

  //  std::cout << uri << std::endl;
  // std::cout << str << std::endl;
}

void MDI::insert_file_cb()
{
  Document *doc = get_active();
  if (!doc) {
    return;
  }

  if (doc->get_readonly()) {
    katoob_error(_("This is a read only document."));
    return;
  }

  FileDialog dlg(_("Please select a file."), FILE_OPEN, -1, _conf, _encodings);
  if (dlg.run()) {
    int enc = dlg.encoding();
    std::vector<std::string> files = dlg.get();
    std::string contents, contents2;

    for (unsigned int x = 0; x < files.size(); x++) {
      if (Utils::katoob_read(files[x], contents)) {
        int tenc = _encodings.convert_to(contents, contents2, enc);
        if (tenc == -1) {
          katoob_error(Utils::substitute(_("Couldn't detect the encoding of %s"), files[x]));
        } else {
          doc->insert(tenc == _encodings.utf8() ? contents : contents2);
        }
      } else {
        katoob_error(contents);
      }
    }
  }
}

void MDI::save_cb()
{
  save();
}

bool MDI::save()
{
  Document *doc = get_active();
  if (!doc) {
    return true;
  }
  return save(doc);
}

bool MDI::save(Document *doc)
{
  assert(doc != NULL);

  if (doc->get_readonly()) {
    katoob_error(_("This document can not be saved. It's a read only file."));
    return true;
  }

  if (doc->has_file()) {
    return doc->save();
  }

  return save(true);
}

void MDI::save_as_cb()
{
  if (get_active()) {
    save(true);
  }
}

void MDI::save_copy_cb()
{
  if (get_active()) {
    save(false);
  }
}

bool MDI::save(bool replace)
{
  Document *doc = get_active();

  if (!doc) {
    return true;
  }

  FileDialog dlg(_("Please select a file for saving."),
                 FILE_SAVE,
                 doc->get_encoding(),
                 _conf,
                 _encodings);
  if (dlg.run()) {
    int enc = dlg.encoding();
    std::vector<std::string> files = dlg.get();
    std::string str = Glib::path_get_basename(files[0]);

    if (Glib::file_test(files[0], Glib::FILE_TEST_EXISTS)) {
      if (katoob_simple_question(
              (Utils::substitute(_("Are you sure you want to overwrite the file %s ?"),
                                 files[0])))) {
        if (doc->save(files[0], enc, replace)) {
          if (replace) {
            _conf.recent_prepend(files[0]);
            signal_recent_regenerate.emit();
          }
          _conf.save_dir(str);
          return true;
        } else {
          return false;
        }
      }
      return false;
    } else {
      if (doc->save(files[0], enc, replace)) {
        if (replace) {
          _conf.recent_prepend(files[0]);
          signal_recent_regenerate.emit();
        }
        _conf.save_dir(str);
        return true;
      } else {
        return false;
      }
    }
  }
  return false;
}

#ifdef ENABLE_PRINT
void MDI::print_cb()
{
  Document *doc = get_active();
  if (!doc) {
    return;
  }

  Glib::RefPtr<Print> prnt = Print::create(_conf, doc, page_setup, settings);
  std::string error;
  if (!prnt->run(error, Gtk::PRINT_OPERATION_ACTION_PRINT_DIALOG)) {
    if (error.size() > 0) {
      katoob_error(error);
    }
  } else {
    // TODO: Save PrintSettings
  }
}

void MDI::print_preview_cb()
{
  Document *doc = get_active();
  if (!doc) {
    return;
  }

  Glib::RefPtr<Print> prnt = Print::create(_conf, doc, page_setup, settings);
  std::string error;
  if (!prnt->run(error, Gtk::PRINT_OPERATION_ACTION_PREVIEW)) {
    katoob_error(error);
  }
}

void MDI::page_setup_cb()
{
  // NOTE: All this so we can get our damn parent!
  page_setup->reset(
      Gtk::run_page_setup_dialog(*(dynamic_cast<Gtk::Window *>(get_parent()->get_parent())),
                                 page_setup->get_page_setup(),
                                 settings));
}
#endif

void MDI::close_cb()
{
  Document *doc = get_active();
  if (doc) {
    close();
  }

  if (children.size() == 0) {
    signal_reset_gui.emit(-1);
  }
}

bool MDI::close(int n)
{
  Document *doc = (n == -1 ? get_active() : children[n]);

  if (!doc) {
    return true;
  }

  if (doc->get_modified()) {
    std::string message;
    if (doc->has_file()) {
      message = Utils::substitute(_("The file %s is not saved, Save first ?"), doc->get_file());
    } else {
      message = Utils::substitute(_("\"%s\" is not saved, Save first?"), doc->get_title());
    }
    switch (katoob_question(message)) {
      case Gtk::RESPONSE_YES:
        if (save()) {
          break;
        } else {
          return false;
        }
      case Gtk::RESPONSE_NO:
        break;
      case Gtk::RESPONSE_CANCEL:
      default:
        return false;
    }
  }

  std::vector<Document *>::iterator iter = children.begin();

  int x = (n == -1 ? get_current_page() : n);

  iter += x;
  children.erase(iter);
  remove_page(x);

  // Let's add it to our closed documents vector.
  // Is it enabled ?
  if (_conf.get("undo_closed", true)) {
    doc->autosave();
    closed_children.push_back(doc);
    // Now let's see how many items do we have.
    unsigned x = _conf.get("undo_closedno", 5);
    if (x != 0) {
      while (closed_children.size() > x) {
        Document *_doc = closed_children[0];
        closed_children.erase(closed_children.begin());
        signal_closed_document_erased.emit();
        delete _doc;
      }
    }
    // emit the signal so the menu can rebuild.
    signal_closed_document_added.emit(doc->get_title());
  } else {
    delete doc;
  }
  signal_document_removed.emit(x);
  return true;
}

void MDI::recent_cb(std::string &rfile)
{
  // fail if the file is not there.
  if (!Glib::file_test(rfile.c_str(), Glib::FILE_TEST_EXISTS)) {
    katoob_error(Utils::substitute(_("The file \"%s\" doesn't exist."), rfile.c_str()));
    return;
  }

  unsigned x = children.size();
  create_document(rfile, -1);
  if (children.size() != x) {
    _conf.recent_prepend(rfile);
    signal_recent_regenerate.emit();
  }
}

void MDI::undo_cb()
{
  Document *doc = get_active();
  if (doc) {
    return doc->undo();
  }
}

void MDI::redo_cb()
{
  Document *doc = get_active();
  if (doc) {
    return doc->redo();
  }
}

void MDI::cut_cb()
{
  Document *doc = get_active();
  if (doc) {
    doc->cut();
  }
}

void MDI::copy_cb()
{
  Document *doc = get_active();
  if (doc) {
    doc->copy();
  }
}

void MDI::paste_cb()
{
  Document *doc = get_active();
  if (doc) {
    doc->paste();
  }
}

void MDI::erase_cb()
{
  Document *doc = get_active();
  if (doc) {
    doc->erase();
  }
}

void MDI::revert_cb()
{
  Document *doc = get_active();
  if (!doc) {
    return;
  }

  if (!doc->has_file()) {
    return;
  }

  if (katoob_simple_question((Utils::substitute(
          _("This will close the current file \"%s\". Discard any changes done and reload the "
            "saved copy of the file. Are you sure you want to continue ?"),
          doc->get_file())))) {
    std::string message;
    if (!doc->revert(message)) {
      katoob_error(message);
    }
  }
}

void MDI::select_all_cb()
{
  Document *doc = get_active();
  if (doc) {
    doc->select_all();
  }
}

void MDI::close_all_cb()
{
  close_all();
}

void MDI::save_all_cb()
{
  for (unsigned x = 0; x < children.size(); x++) {
    save(children[x]);
  }
}

void MDI::set_line_numbers(bool _active)
{
  Document *doc = get_active();
  if (doc) {
    doc->line_numbers(_active);
  }
}

void MDI::set_wrap_text(bool _active)
{
  Document *doc = get_active();
  if (doc) {
    doc->set_wrap_text(_active);
  }
}

bool MDI::close_all()
{
  while (true) {
    if (!get_active()) {
      return true;
    }
    if (!close()) {
      return false;
    }
  }
}

void MDI::goto_line_cb()
{
  Document *doc = get_active();
  if (!doc) {
    return;
  }

  goto_line_cb2(katoob_goto_dialog());
}

void MDI::goto_line_cb2(int line)
{
  Document *doc = get_active();
  if (!doc) {
    return;
  }

  if (line >= 1) {
    doc->scroll_to(line);
  }
}

void MDI::find(std::string &str)
{
  Document *doc = get_active();
  if (!doc) {
    return;
  }

  doc->set_search_text(str);
  find_next_cb();
}

void MDI::find_cb()
{
  Document *doc = get_active();
  if (!doc) {
    return;
  }

  SearchDialog dialog;

  dialog.whole_word(doc->get_search_whole_word());
  dialog.match_case(doc->get_search_match_case());
  dialog.wrap(doc->get_search_wrap());
  dialog.backwards(doc->get_search_backwards());
  dialog.beginning(doc->get_search_from_beginning());
  dialog.set_text(doc->get_search_text());

  if (!dialog.run()) {
    return;
  }

  if (dialog.get_text().size() == 0) {
    katoob_error(_("You must inter a word to search for."));
    return;
  }

  doc->set_search_whole_word(dialog.whole_word());
  doc->set_search_match_case(dialog.match_case());
  doc->set_search_wrap(dialog.wrap());
  doc->set_search_backwards(dialog.backwards());
  doc->set_search_from_beginning(dialog.beginning());
  doc->set_search_text(dialog.get_text());

  if (!doc->search()) {
    katoob_error(_("No search results found."));
  }
}

void MDI::find_next_cb()
{
  Document *doc = get_active();
  if (!doc) {
    return;
  }

  if (doc->get_search_text().size() == 0) {
    find_cb();
    return;
  }

  if (!doc->search_next()) {
    katoob_error(_("No search results found."));
  }
}

void MDI::replace_cb()
{
  Document *doc = get_active();
  if (!doc) {
    return;
  }

  ReplaceDialog dialog;

  dialog.whole_word(doc->get_search_whole_word());
  dialog.match_case(doc->get_search_match_case());
  dialog.wrap(doc->get_search_wrap());
  dialog.backwards(doc->get_search_backwards());
  dialog.beginning(doc->get_search_from_beginning());
  dialog.set_text(doc->get_search_text());
  dialog.set_replace(doc->get_replace_text());

  // Connect the signals.
  dialog.signal_find.connect(
      sigc::bind<ReplaceDialog *>(sigc::mem_fun(*this, &MDI::replace_dialog_signal_find_cb),
                                  &dialog));
  dialog.signal_replace.connect(
      sigc::bind<ReplaceDialog *>(sigc::mem_fun(*this, &MDI::replace_dialog_signal_replace_cb),
                                  &dialog));
  dialog.signal_replace_all.connect(
      sigc::bind<ReplaceDialog *>(sigc::mem_fun(*this, &MDI::replace_dialog_signal_replace_all_cb),
                                  &dialog));
  dialog.run();

  doc->set_search_whole_word(dialog.whole_word());
  doc->set_search_match_case(dialog.match_case());
  doc->set_search_wrap(dialog.wrap());
  doc->set_search_backwards(dialog.backwards());
  doc->set_search_from_beginning(dialog.beginning());
  doc->set_search_text(dialog.get_text());
  doc->set_replace_text(dialog.get_replace());
}

bool MDI::replace_dialog_signal_find_cb(ReplaceDialog *dialog)
{
  Document *doc = get_active();
  assert(doc != NULL);

  doc->set_search_whole_word(dialog->whole_word());
  doc->set_search_match_case(dialog->match_case());
  doc->set_search_wrap(dialog->wrap());
  doc->set_search_backwards(dialog->backwards());
  doc->set_search_from_beginning(dialog->beginning());
  doc->set_search_text(dialog->get_text());

  if (!doc->search()) {
    katoob_error(_("No search results found."));
    return false;
  }
  dialog->beginning(doc->get_search_from_beginning());
  return true;
}

void MDI::replace_dialog_signal_replace_cb(ReplaceDialog *dialog)
{
  Document *doc = get_active();
  assert(doc != NULL);
  doc->set_replace_text(dialog->get_replace());
  doc->replace();
}

void MDI::replace_dialog_signal_replace_all_cb(ReplaceDialog *dialog)
{
  Document *doc = get_active();
  assert(doc != NULL);
  int x = 0;

  doc->set_search_whole_word(dialog->whole_word());
  doc->set_search_match_case(dialog->match_case());
  doc->set_search_backwards(dialog->backwards());
  doc->set_search_from_beginning(dialog->beginning());
  doc->set_search_text(dialog->get_text());
  doc->set_replace_text(dialog->get_replace());

  // We don't wrap otherwise we'll get an infinite loop.
  doc->set_search_wrap(false);

  while (doc->search()) {
    ++x;
    doc->replace();
  }
  katoob_info(
      Utils::substitute(ngettext("Replaced %d occurence.", "Replaced %d occurences.", x), x));
}

void MDI::execute_cb()
{
  Document *doc = get_active();
  if (!doc) {
    return;
  }

  ExecDialog dlg(_conf);

  if (dlg.run()) {
    bool new_buffer = dlg.get_new_buffer();
    std::string command = dlg.get_command();
    _conf.exec_cmd_prepend(command);

    unsigned found = 0;
    for (unsigned x = 0; x < command.size(); x++) {
      // TODO: Can this crash if we have a string that ends in % ?
      if ((command[x] == '%') && (command[x + 1] == 's')) {
        ++found;
      }
    }

    if (found > 1) {
      katoob_error(_("You can use only one '%s'"));
      return;
    }

    std::string error;
    std::string cmd;
    std::string text;
    TempFile tf;
    if (found == 1) {
      if (!tf.ok(error)) {
        katoob_error(error);
        return;
      }

      // Write the buffer contents.
      text = doc->get_text();
      if (!tf.write(text, error)) {
        katoob_error(error);
        return;
      }
      text.clear();
      cmd = Utils::substitute(command, tf.get_name());
    } else {
      cmd = command;
    }

    if (!Pipe::exec(command, cmd, text, error)) {
      katoob_error(error);
      return;
    }

    if (!new_buffer) {
      doc->set_text(text);
    } else {
      // We create a new document then we insert.
      Document *d = create_document();
      if (d) {
        d->set_text(text);
      }
    }
  }
}

#ifdef ENABLE_SPELL
void MDI::set_auto_spell(bool s)
{
  Document *doc = get_active();
  if (!doc) {
    return;
  }
  doc->set_auto_spell(s);
}

void MDI::do_spell()
{
  Document *doc = get_active();
  if ((!doc) || (doc->get_readonly()) || (doc->is_empty())) {
    return;
  }
  // TODO: Check that the document can really do the spell checking.

  SpellDialog dialog(doc);
  dialog.run();
}

bool MDI::set_dictionary(std::string &old, std::string &new_dict)
{
  std::string error;
  Document *doc = get_active();
  assert(doc != NULL);
  old = doc->get_dictionary();
  if (!doc->set_dictionary(new_dict, error)) {
    katoob_error(error);
    return false;
  }
  return true;
}
#endif

void MDI::signal_switch_page_cb(GtkNotebookPage *p, guint n)
{
  children[n]->emit_signals();
  signal_doc_activated.emit(n);
}

void MDI::activate(int x)
{
  set_current_page(x);
}

void MDI::connect_signals(Document *doc)
{
  // NOTE: Connect new signals whenever the Document class gets new!
  doc->signal_modified_set.connect(sigc::mem_fun(this, &MDI::signal_document_modified_cb));
  doc->signal_can_undo.connect(sigc::mem_fun(this, &MDI::signal_document_can_undo_cb));
  doc->signal_can_redo.connect(sigc::mem_fun(this, &MDI::signal_document_can_redo_cb));
  doc->signal_readonly_set.connect(sigc::mem_fun(this, &MDI::signal_document_readonly_cb));
  doc->signal_file_changed.connect(sigc::mem_fun(this, &MDI::signal_document_file_changed_cb));
  doc->signal_cursor_moved.connect(sigc::mem_fun(this, &MDI::signal_document_cursor_moved_cb));
  doc->signal_encoding_changed.connect(
      sigc::mem_fun(this, &MDI::signal_document_encoding_changed_cb));
  doc->signal_overwrite_toggled.connect(
      sigc::mem_fun(this, &MDI::signal_document_overwrite_toggled_cb));
  doc->signal_title_changed.connect(sigc::mem_fun(this, &MDI::signal_document_title_changed_cb));
#ifdef ENABLE_SPELL
  doc->signal_auto_spell_set.connect(sigc::mem_fun(this, &MDI::signal_document_spell_enabled_cb));
  doc->signal_dictionary_changed.connect(sigc::mem_fun(this, &MDI::signal_dictionary_changed_cb));
#endif
  doc->signal_wrap_text_set.connect(sigc::mem_fun(this, &MDI::signal_document_wrap_text_cb));
  doc->signal_line_numbers_set.connect(sigc::mem_fun(this, &MDI::signal_document_line_numbers_cb));
  doc->signal_dict_lookup_request.connect(
      sigc::mem_fun(this, &MDI::signal_document_dict_lookup_cb));
#ifdef ENABLE_HIGHLIGHT
  doc->signal_highlight_set.connect(sigc::mem_fun(this, &MDI::signal_document_highlight_cb));
#endif
  doc->signal_text_view_request_file_open.connect(
      sigc::mem_fun(*this, &MDI::signal_text_view_request_file_open_cb));
}

bool MDI::set_encoding(int n, int &o)
{
  std::string err;

  Document *doc = get_active();

  if (!doc) {
    return true;
  }

  o = doc->get_encoding();
  if (!doc->has_file()) {
    doc->set_encoding(n, false, err);
    return true;
  }

  if (!doc->set_encoding(n, true, err)) {
    katoob_error(err);
    return false;
  }
  return true;
}

void MDI::signal_document_dict_lookup_cb(std::string word)
{
  std::string uri = Dict::construct_uri(_conf, word);

  sigc::slot<void, bool, const std::string &> slot =
      sigc::bind(sigc::mem_fun(this, &MDI::signal_dict_transfer_complete_cb), word);
  std::string error;
  if (!Network::add_transfer(uri, error, slot)) {
    katoob_error(error);
  }
}

void MDI::signal_dict_transfer_complete_cb(bool st, const std::string &str, std::string word)
{
  if (!st) {
    katoob_error(str);
    return;
  }

  std::vector<std::string> defs;

  if (!Dict::parse_defs(str, defs)) {
    if (defs.size() == 0) {
      katoob_error(_("Failed to get any definitions."));
    } else if (defs[0].size() == 0) {
      katoob_error(_("Failed to get any definitions."));
    } else {
      katoob_error(defs[0]);
    }
  } else {
    // TODO: only one definition is being displayed.
    Document *doc = get_active();
    if (katoob_big_info(defs[0], doc != NULL)) {
      // not needed but just in case!
      if (doc) {
        doc->insert(defs[0]);
      }
    }
  }
}

void MDI::signal_document_title_changed_cb(std::string t)
{
  signal_document_title_changed.emit(t, get_current_page());
  set_menu_label_text(*get_active(), t);
}

void MDI::signal_document_readonly_cb(bool ro)
{
  signal_document_readonly.emit(get_current_page(), ro);
  // NOTE: No idea why won't this work when I set it from the document itself.
  dynamic_cast<Label *>(get_tab_label(*children[get_current_page()]))->set_readonly(ro);
}

void MDI::signal_document_modified_cb(bool m)
{
  signal_document_modified.emit(get_current_page(), m);
  // NOTE: No idea why won't this work when I set it from the document itself.
  dynamic_cast<Label *>(get_tab_label(*children[get_current_page()]))->set_modified(m);
}

void MDI::signal_document_label_close_clicked_cb(Document *doc)
{
  for (unsigned x = 0; x < children.size(); x++) {
    if (children[x] == doc) {
      close(x);
      if (children.size() == 0) {
        signal_reset_gui.emit(-1);
      }
      return;
    }
  }
}

void MDI::closed_document_activated_cb(int x)
{
  Document *doc = closed_children[x];
  std::vector<Document *>::iterator iter = closed_children.begin();
  iter += x;
  closed_children.erase(iter);

  add_document(doc, false);
}

void MDI::reset_gui()
{
  if (_conf.get("tabsmenu", true)) {
    popup_enable();
  } else {
    popup_disable();
  }

#ifdef ENABLE_MAEMO
  set_show_tabs(_conf.get("showtabs", false));
  set_scrollable(_conf.get("scrolltabs", false));
#else
  set_show_tabs(_conf.get("showtabs", true));
  set_scrollable(_conf.get("scrolltabs", true));
#endif

  switch (_conf.get("tabspos", TABS_POS_TOP)) {
    case TABS_POS_BOTTOM:
      set_tab_pos(Gtk::POS_BOTTOM);
      break;
    case TABS_POS_RIGHT:
      set_tab_pos(Gtk::POS_RIGHT);
      break;
    case TABS_POS_LEFT:
      set_tab_pos(Gtk::POS_LEFT);
      break;
    default:
      set_tab_pos(Gtk::POS_TOP);
      break;
  }

  for (unsigned x = 0; x < children.size(); x++) {
    children[x]->reset_gui();
  }

  for (unsigned x = 0; x < closed_children.size(); x++) {
    closed_children[x]->reset_gui();
  }

  int total = get_n_pages();
  for (int x = 0; x < total; x++) {
    bool m = children[x]->get_modified();
    bool r = children[x]->get_readonly();

    if (m) {
      dynamic_cast<Label *>(get_tab_label(*children[x]))
          ->set_modified(children[x]->get_modified(), true);
    }
    if (r) {
      dynamic_cast<Label *>(get_tab_label(*children[x]))
          ->set_readonly(children[x]->get_readonly(), true);
    }
    if ((!r) && (!m)) {
      dynamic_cast<Label *>(get_tab_label(*children[x]))->set_normal();
    }
  }

#ifdef ENABLE_PRINT
  settings->reset();
#endif
}

void MDI::import_cb(Import im)
{
  std::string file = SimpleFileDialog::get_file(im.name, FILE_OPEN, _conf);
  if (file.size() == 0) {
    return;
  }

  std::string out;
  if (!im.func(file, out)) {
    katoob_error(out);
    return;
  }
  // is it utf8 ?
  if (_encodings.utf8(out)) {
    // create.
    Document *doc = create_document();
    if (doc) {
      doc->set_text(out);
    }
  } else {
    std::string res;
    if (_encodings.convert_to(out, res, _encodings.default_open()) != -1) {
      // create.
      Document *doc = create_document();
      if (doc) {
        doc->set_text(res);
      }
    } else {
      katoob_error(Utils::substitute(_("Couldn't detect the encoding of %s"), file));
    }
  }
}

void MDI::export_cb(Export ex)
{
  Document *doc = get_active();
  if (!doc) {
    return;
  }

  std::string file = SimpleFileDialog::get_file(ex.name, FILE_SAVE, _conf);
  if (file.size() == 0) {
    return;
  }

  if (Glib::file_test(file, Glib::FILE_TEST_EXISTS)) {
    std::string message(
        Utils::substitute(_("Are you sure you want to overwrite the file %s ?"), file));
    if (!katoob_simple_question(message))
      return;
  }
  std::string error;
  std::string out;
  if (ex.lines) {
    std::vector<Glib::ustring> lines;
    doc->get_lines(lines);
    for (unsigned x = 0; x < lines.size(); x++) {
      std::string tout;
      if (!ex.func(lines[x], tout, error)) {
        katoob_error(error);
        return;
      }
      out += tout;
      // TODO: We need to check whether the document has a \n at the end or not.
      out += '\n';
    }
  } else {
    Glib::ustring text = doc->get_text();
    if (!ex.func(text, out, error)) {
      katoob_error(error);
      return;
    }
    text.clear();
  }
  // Write.
  if (!Utils::katoob_write(_conf, file, out, error)) {
    katoob_error(error);
  }
}

void MDI::signal_extra_button_clicked_cb(std::string str)
{
  Document *doc = get_active();
  if (!doc) {
    return;
  }
  doc->insert(str);
}

#ifdef ENABLE_HIGHLIGHT
void MDI::set_highlight(std::string x)
{
  Document *doc = get_active();
  if (doc) {
    doc->set_highlight(x);
  }
}
#endif

void MDI::signal_text_view_request_file_open_cb(std::string filename)
{
  create_document(filename);
}

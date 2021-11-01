/*
 * docfactory.cc
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

#include "docfactory.hh"
#include "document.hh"
#include <cassert>
#include <glibmm/main.h>

int get_unused_number() {
  static int foo = 0;
  foo++;
  return foo;
}

Document *DocFactory::create(std::string &str) {
  Document *doc = process(new Document(*_conf, *_enc, get_unused_number()));
  if (doc) {
    doc->set_text(str);
  }

  return doc;
}

Document *DocFactory::create(std::string &file, int encoding, bool is_stdin) {
  if (is_stdin) {
    return process(
        new Document(*_conf, *_enc, get_unused_number(),
                     encoding == -1 ? _enc->default_open() : encoding));
  } else {
    return process(new Document(*_conf, *_enc, encoding, file));
  }
}

Document *DocFactory::create() {
  int x = get_unused_number();
  return process(new Document(*_conf, *_enc, x));
}

Document *DocFactory::process(Document *doc) {
  if (!doc->ok()) {
    delete doc;
    return NULL;
  }

  children.push_back(doc);
  signal_created.emit(doc);

  doc->signal_can_undo.connect(
      sigc::mem_fun(signal_can_undo, &sigc::signal<void, bool>::emit));
  doc->signal_can_redo.connect(
      sigc::mem_fun(signal_can_redo, &sigc::signal<void, bool>::emit));
  // TODO FIXME XXX For some reason that I cannot figure out yet this
  // completely breaks with libsigc++ and doesn't even build.
  // doc->signal_readonly_set.connect
  //   (sigc::mem_fun(signal_rom_set, &sigc::signal<void, bool, bool>::emit));
  doc->signal_file_changed.connect(sigc::mem_fun(
      signal_file_changed, &sigc::signal<void, std::string>::emit));
  doc->signal_cursor_moved.connect(
      sigc::mem_fun(signal_cursor_moved, &sigc::signal<void, int, int>::emit));
  doc->signal_encoding_changed.connect(
      sigc::mem_fun(signal_encoding_changed, &sigc::signal<void, int>::emit));
  doc->signal_overwrite_toggled.connect(
      sigc::mem_fun(signal_overwrite_toggled, &sigc::signal<void, bool>::emit));
  doc->signal_title_changed.connect(sigc::mem_fun(
      signal_title_changed, &sigc::signal<void, std::string>::emit));

  doc->signal_auto_spell_set.connect(
      sigc::mem_fun(signal_auto_spell_set, &sigc::signal<void, bool>::emit));
  doc->signal_dictionary_changed.connect(sigc::mem_fun(
      signal_dictionary_changed, &sigc::signal<void, std::string>::emit));

  doc->signal_wrap_text_set.connect(
      sigc::mem_fun(signal_wrap_text_set, &sigc::signal<void, bool>::emit));
  doc->signal_line_numbers_set.connect(
      sigc::mem_fun(signal_line_numbers_set, &sigc::signal<void, bool>::emit));
  doc->signal_dict_lookup_request.connect(sigc::mem_fun(
      signal_dict_lookup_request, &sigc::signal<void, std::string>::emit));

  doc->signal_highlight_set.connect(
      sigc::mem_fun(signal_highlight, &sigc::signal<void, std::string>::emit));

  doc->signal_text_view_request_file_open.connect(
      sigc::mem_fun(signal_text_view_request_file_open,
                    &sigc::signal<void, std::string>::emit));

  doc->get_label().signal_close_clicked.connect(sigc::bind<Document *>(
      sigc::mem_fun(signal_close_request,
                    &sigc::signal<void, Document *>::emit),
      doc));

  doc->emit_signals();

  return doc;
}

DocFactory::~DocFactory() {
  for (unsigned x = 0; x < children.size(); x++) {
    delete children[x];
  }

  for (unsigned x = 0; x < closed_children.size(); x++) {
    delete closed_children[x];
  }

  children.clear();
  closed_children.clear();
}

DocFactory::DocFactory(Conf *conf, Encodings *enc)
    : _conf(conf), _enc(enc), active(-1) {
  // 1 minute.
  Glib::signal_timeout().connect(sigc::mem_fun(this, &DocFactory::autosave),
                                 1 * 60 * 1000);
}

bool DocFactory::autosave() {
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

void DocFactory::init(Conf *conf, Encodings *enc) {
  _factory = new DocFactory(conf, enc);
}

DocFactory *DocFactory::get() {
  assert(_factory != NULL);
  return _factory;
}

void DocFactory::destroy() {
  delete _factory;
  _factory = NULL;
}

void DocFactory::remove_document(int idx) {
  // We emit this before deleting the Document to allow our MDI to remove the
  // Document.
  signal_pre_destroyed.emit(idx);

  // Now we erase:
  Document *doc = children[idx];
  std::vector<Document *>::iterator iter = children.begin() + idx;
  children.erase(iter);

  // Let's add it to our closed documents vector.
  // Is it enabled ?
  if (_conf->get("undo_closed", true)) {
    doc->autosave();
    closed_children.push_back(doc);
    // Now let's see how many items do we have.
    unsigned x = _conf->get("undo_closedno", 5);
    if ((x != 0) && (x < closed_children.size())) {
      while (closed_children.size() > x) {
        Document *_doc = closed_children[0];
        closed_children.erase(closed_children.begin());
        delete _doc;
      }
    }
    // emit the signal so the menu can rebuild.
    signal_closed.emit();
  } else {
    delete doc;
  }
  signal_destroyed.emit();
}

int DocFactory::get_index(Document *doc) {
  for (unsigned x = 0; x < children.size(); x++) {
    if (doc == children[x]) {
      return x;
    }
  }
  return -1;
}

Document *DocFactory::get_document(size_t idx) {
  if (idx >= children.size()) {
    return NULL;
  }

  return children[idx];
}

bool DocFactory::get_info(size_t idx, std::string &title, bool &read_only,
                          bool &modified) {
  if (idx >= children.size()) {
    return false;
  }

  title = children[idx]->get_title();

  read_only = children[idx]->get_readonly();

  modified = children[idx]->get_modified();

  return true;
}

bool DocFactory::get_closed_info(size_t idx, std::string &title,
                                 bool &read_only, bool &modified) {
  if (idx >= closed_children.size()) {
    return false;
  }

  title = closed_children[idx]->get_title();

  read_only = closed_children[idx]->get_readonly();

  modified = closed_children[idx]->get_modified();

  return true;
}

void DocFactory::activate_closed(size_t idx) {
  assert(idx < closed_children.size());

  Document *doc = closed_children[idx];
  closed_children.erase(closed_children.begin() + idx);
  children.push_back(doc);

  signal_created.emit(doc);
  signal_closed.emit();
}

void DocFactory::reset_ui() {
  for (unsigned x = 0; x < children.size(); x++) {
    children[x]->reset_gui();
  }

  // FIXME: We don't reset the GUI of closed Documents. A signal might get
  // emitted but we don't want the GUI to really respond. I need to block the
  // emission of all signals first probably.

  unsigned x = _conf->get("undo_closedno", 5);
  if ((x != 0) && (x < closed_children.size())) {
    while (closed_children.size() > x) {
      Document *_doc = closed_children[0];
      closed_children.erase(closed_children.begin());
      delete _doc;
    }
  }

  // emit the signal so the menu can rebuild.
  signal_closed.emit();
}

void DocFactory::set_active(int idx) { active = idx; }

void DocFactory::emit_signals() {
  assert(active != -1);

  children[active]->emit_signals();
}

DocFactory *DocFactory::_factory = NULL;

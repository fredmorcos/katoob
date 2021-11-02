/*
 * docfactory.hh
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

#pragma once

#include <string>
#include <vector>
#include <sigc++/sigc++.h>

// forward declaration:
class Conf;
class Document;
class Encodings;

class DocFactory : public sigc::trackable {
public:
  /**
   * \brief Get the DocFactory instance.
   * \return A pointer to an instance of DocFactory
   */
  static DocFactory *get();

  /**
   * \brief initialize DocFactory
   */
  static void init(Conf *, Encodings *);

  /**
   * \brief create an empty document.
   * \return a pointer to Document if the Document has been created otherwise NULL
   */
  Document *create();

  /**
   * \brief Create a document from a text utf8 string.
   * \param str the text to use.
   * \return a pointer to Document if the Document has been created otherwise NULL
   */
  Document *create(std::string& str);

  /**
   * \brief Create a document by reading a non utf8 file or from stdin.
   * \param file the full path to the file.
   * \param encoding the encoding index to use.
   * \param is_stdin whether to read the contents from the standard input or not.
   * \return a pointer to Document if the Document has been created otherwise NULL
   */
  Document *create(std::string& file, int encoding, bool is_stdin);

  /**
   * \brief destroy the DocFactory and all the Documents. It can no longer be used.
   */
  void destroy();

  /**
   * \brief autosave all our Documents.
   * \return Always true.
   */
  bool autosave();

  /**
   * \brief get a Document object knowing its index.
   * \param idx the Document index to return.
   * \return a pointer to a Document or NULL
   */
  Document *get_document(size_t idx);

  /**
   * \brief get the index of a Document given a pointer to its object.
   * \param doc a pointer to a Document object
   * \return the document index or -1 if not found.
   */
  int get_index(Document *doc);

  /**
   * \brief remove a document given its index. It can still be added to the closed document.
   * \param idx the index of the Document.
   */
  void remove_document(int idx);

  /**
   * \brief retrieve some information about a document.
   * \param idx the index of the Document to check.
   * \param title a reference to a std::string to place the Document title in.
   * \param read_only a reference to a bool that will carry the Document read_only state.
   * \param modified  a reference to a bool that will carry the Document modified state.
   * \return true if idx can be mapped to a Document, false otherwise.
   */
  bool get_info(size_t idx, std::string& title, bool& read_only, bool& modified);

  /**
   * \brief retrieve some information about a closed document.
   * \param idx the index of the Document to check.
   * \param title a reference to a std::string to place the Document title in.
   * \param read_only a reference to a bool that will carry the Document read_only state.
   * \param modified  a reference to a bool that will carry the Document modified state.
   * \return true if idx can be mapped to a Document, false otherwise.
   */
  bool get_closed_info(size_t idx, std::string& title, bool& read_only, bool& modified);

  /**
   * \brief Activate a closed Document.
   * \param the index of the closed Document
   */
  void activate_closed(size_t idx);

  /**
   * \brief this should be called when the configuration's changed (Preferences Dialog for example)
   */
  void reset_ui();

#if 0
  /**
   * \brief get the active (Focuses) Document.
   * \return the active (Focused) Document or NULL if we have none.
   */
  Document *get_active();
#endif

  /**
   * \brief Set the active Document.
   * \param idx the Document index to use.
   */
  void set_active(int idx);

  /**
   * \brief ask the active Document to emit its signals.
   */
  void emit_signals();

  // signals
  /**
   * \brief emitted when a new Document has been created.
   */
  sigc::signal<void, Document *> signal_created;

  /**
   * \brief emitted before we destroy a Document.
   */
  sigc::signal<void, int> signal_pre_destroyed;

  /**
   * \brief emitted after we destroy a Document.
   */
  sigc::signal<void> signal_destroyed;

  /**
   * \brief emitted when the list of our closed documents changes.
   */
  sigc::signal<void> signal_closed;

  // Document * signals.

  /**
   * \brief emitted when a document changes the "can undo" status.
   */
  sigc::signal<void, bool> signal_can_undo;

  /**
   * \brief emitted when a document changes the "can redo" status.
   */
  sigc::signal<void, bool> signal_can_redo;

  /**
   * \brief emitted when a document changes the read only or modified status.
   */
  sigc::signal<void, bool, bool> signal_rom_set;

  /**
   * \brief emitted when a document changes the file "attached" to it.
   */
  sigc::signal<void, std::string> signal_file_changed;

  /**
   * \brief emitted when the cursor gets moved.
   */
  sigc::signal<void, int, int> signal_cursor_moved;

  /**
   * \brief emitted when the document encoding gets changed.
   */
  sigc::signal<void, int> signal_encoding_changed;

  /**
   * \brief emitted when the document overwrite mode gets changed.
   */
  sigc::signal<void, bool> signal_overwrite_toggled;

  /**
   * \brief emitted when the Document title gets changed.
   */
  sigc::signal<void, std::string> signal_title_changed;

  /**
   * \brief emitted when the document text wrapping mode gets changed.
   */
  sigc::signal<void, bool> signal_wrap_text_set;

  /**
   * \brief emitted when the document line numbers status gets changed.
   */
  sigc::signal<void, bool> signal_line_numbers_set;

  /**
   * \brief emitted when the user drops a file on a Document.
   */
  sigc::signal<void, std::string> signal_text_view_request_file_open;

  /**
   * \brief emitted when a document requests looking up a word via the dict protocol.
   */
  sigc::signal<void, std::string> signal_dict_lookup_request;

  /**
   * \brief emitted when a document changes the "auto check spelling" property.
   */
  sigc::signal<void, bool> signal_auto_spell_set;

  /**
   * \brief emitted when the spell dictionary gets changed.
   */
  sigc::signal<void, std::string> signal_dictionary_changed;

  /**
   * \brief emitted when the Document Label close button is clicked.
   */
  sigc::signal<void, Document *> signal_close_request;

  /**
   * \brief emitted when the Document changes its syntax highlighting language.
   */
  sigc::signal<void, std::string> signal_highlight;

private:
  /**
   * \brief process a newly created document. Or destroy it if the creation's failed.
   * \param doc a pointer to a newly created Document
   * \return a pointer to a Document or NULL.
   */
  Document *process(Document *doc);

  /**
   * \brief Constructor
   * \param conf a pointer to a Conf class.
   * \param end a pointer to an Encodings class.
   */
  DocFactory(Conf *conf, Encodings *enc);

  /**
   * \brief Destructor
   */
  ~DocFactory();

  /**
   * \brief Not implemented.
   */
  DocFactory();
  DocFactory& operator=(const DocFactory&);
  DocFactory(const DocFactory&);

  Conf *_conf;
  Encodings *_enc;

  int active;

  static DocFactory *_factory;
  std::vector<Document *> children, closed_children;
  std::vector<int> numbers;
};

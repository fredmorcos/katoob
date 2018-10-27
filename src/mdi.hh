/*
 * mdi.hh
 * This file is part of katoob
 *
 * Copyright (C) 2006, 2007, 2008 Mohammed Sameer
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

#ifndef __MDI_HH__
#define __MDI_HH__

#include <vector>
#include <gtkmm/notebook.h>
#include "replacedialog.hh"
#include "encodings.hh"
#include "import.hh"
#include "export.hh"
#include "conf.hh"

#ifdef ENABLE_PRINT
#include "pagesetup.hh"
#include "printsettings.hh"
#endif

// forward declaration
class Document;

class MDI : public Gtk::Notebook {
public:
  MDI(Conf&, Encodings&);
  ~MDI();

  // Our methods.
  bool create_document(std::string&, int = -1);
  bool create_document();

  bool close_all();
  bool close(int n=-1);

  /* Signal callbacks. */
  void create_cb() { create_document(); }
  void open_cb();
  void open_location_cb();
  void save_cb();
  void save_as_cb();
  void save_copy_cb();
  void revert_cb();
#ifdef ENABLE_PRINT
  void print_cb();
  void page_setup_cb();
  void print_preview_cb();
#endif
  void close_cb();

  void recent_cb(std::string);

  void undo_cb();
  void redo_cb();
  void cut_cb();
  void copy_cb();
  void paste_cb();
  void erase_cb();
  void select_all_cb();
  void insert_file_cb();

  void set_wrap_text(bool);
  void set_line_numbers(bool);

  void save_all_cb();
  void close_all_cb();

#ifdef ENABLE_SPELL
  void set_auto_spell(bool);
  void do_spell();
  bool set_dictionary(std::string&, std::string&);
#endif

  void goto_line_cb();
  void goto_line_cb2(int);
  void find(std::string&);
  void find_cb();
  void find_next_cb();
  void replace_cb();

  void execute_cb();

  void signal_switch_page_cb(GtkNotebookPage*, guint);

  bool set_encoding(int, int&);

  void activate(int);

  void import_cb(Import *im);
  void export_cb(Export *ex);

  /* Our signals. */
  sigc::signal<void, bool> signal_reset_gui;

  // Signals we get from the Document class.
  sigc::signal<void, int, bool, bool> signal_document_rom;
  sigc::signal<void, std::string&> signal_document_file_changed;
  sigc::signal<void, int> signal_document_encoding_changed;
  sigc::signal<void, std::string, int> signal_document_title_changed;
  sigc::signal<void, int> signal_doc_activated;

#ifdef ENABLE_SPELL
  sigc::signal<void, bool> signal_document_spell_enabled;
#endif

  void closed_document_activated_cb(int);

  void reset_gui();

  void signal_extra_button_clicked_cb(std::string);

#ifdef ENABLE_HIGHLIGHT
  void set_highlight(std::string);
#endif
  void scan_temp();

  Document *get_active();

private:
  void signal_created_cb(Document *);
  void signal_destroyed_cb(int);

  bool replace_dialog_signal_find_cb(ReplaceDialog *);
  void replace_dialog_signal_replace_cb(ReplaceDialog *);
  void replace_dialog_signal_replace_all_cb(ReplaceDialog *);

  // Signal handlers.
  void signal_document_rom_cb(bool, bool);
  void signal_document_file_changed_cb(std::string s) { signal_document_file_changed.emit(s); }
  void signal_document_encoding_changed_cb(int e) { signal_document_encoding_changed.emit(e); }
  void signal_document_title_changed_cb(std::string);

  void signal_transfer_complete_cb(bool, const std::string&, const std::string, int, bool);
  void signal_dict_transfer_complete_cb(bool, const std::string&, std::string);

#ifdef ENABLE_SPELL
  void signal_document_spell_enabled_cb(bool s) { signal_document_spell_enabled.emit(s); }
#endif

  void signal_document_label_close_clicked_cb(Document *);

  void signal_document_dict_lookup_cb(std::string);

  void signal_text_view_request_file_open_cb(std::string);

  /**
   * \brief add a document to our notebook
   * \param doc a pointer to a Document object.
   */
  void add_document(Document *doc);
  bool save();
  bool save(bool);
  bool save(Document *);

  //  std::vector<Document *> children;
  //  std::vector<Document *> closed_children;

  Conf& _conf;
  Encodings& _encodings;
#ifdef ENABLE_PRINT
  Glib::RefPtr<PageSetup> page_setup;
  Glib::RefPtr<PrintSettings> settings;
#endif
};

#endif /* __MDI_HH__ */

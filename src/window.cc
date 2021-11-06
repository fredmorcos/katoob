/*
 * window.cc
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

#include "aboutdialog.hh"
#include "dialogs.hh"
#include "macros.h"
#include "preferencesdialog.hh"
#include "utils.hh"
#include "window.hh"
#include <cassert>
#include <iostream>
#include <sstream>

Window::Window(Conf &conf, Encodings &encodings, std::vector<std::string> &files):
 _conf(conf),
 _encodings(encodings),
 menubar(conf,
         encodings
#ifdef ENABLE_EMULATOR
         ,
         Emulator::list_layouts()
#endif
#ifdef ENABLE_MULTIPRESS
             ,
         Multipress::list_layouts()
#endif
             ),
 toolbar(conf),
 mdi(conf, encodings),
 statusbar(conf),
 box(false, 0)
{
  box.pack_start(menubar, Gtk::PACK_SHRINK, 0);
  box.pack_start(toolbar.get_main(), false, false, 0);
  //  box.pack_start(toolbar.get_extended(), Gtk::PACK_SHRINK, 0);

  box.pack_start(toolbar.get_extended(), Gtk::PACK_SHRINK, 0);

  box.pack_start(mdi, Gtk::PACK_EXPAND_WIDGET, 0);
  box.pack_start(statusbar, Gtk::PACK_SHRINK, 0);

  add(box);

  // Now let's connect our signals.
  connect_menubar_signals();
  connect_toolbar_signals();
  connect_mdi_signals();

  reset_gui();

  mdi.scan_temp();
  // This is here after we connect the signals
  // So we can get the signals emitted when we create the first document.
  // Now let's create the children.
  // If MDI has an active Document then we managed to recover something.
  // And in that case, we won't create a new document.
  if ((files.size() == 0) && (!mdi.get_active())) {
    mdi.create_document();
  } else {
    for (unsigned int x = 0; x < files.size(); x++) {
      mdi.create_document(files[x]);
    }

    files.clear();

    // It's possible that we might fail to detect the encoding of all the files
    // passed as arguments. In this case, we will create a blank document.
    // We can't the number of the documents because MDI has no size method.
    if (!mdi.get_active()) {
      mdi.create_document();
    }
  }

  move(conf.get("x", 50), conf.get("y", 50));
  resize(conf.get("w", 500), conf.get("h", 400));
#ifdef GLIBMM_EXCEPTIONS_ENABLED
  try {
    set_icon_from_file(Utils::get_data_path("katoob-small.png"));
  }
#ifndef _WIN32
  catch (Glib::Error &er) {
    std::cout << er.what() << std::endl;
  }
#else
  catch (...) {
    // NOTE: Why the hell can't I catch Glib::Error or Glib::FileError under win32 ?
#ifndef NDEBUG
    std::cout << "I can't set the main window icon to " << Utils::get_data_path("katoob-small.png")
              << std::endl;
#endif
  }
#endif
#else /* ! GLIBMM_EXCEPTIONS_ENABLED */
  std::auto_ptr<Glib::Error> error;
  set_icon_from_file(Utils::get_data_path("katoob-small.png"), error);
  if (error.get()) {
    std::cout << error->what() << std::endl;
  }
#endif
  //  set_title();
  signal_delete_event().connect(sigc::mem_fun(*this, &Window::signal_delete_event_cb));

  // We don't call show_all(); here as our statusbar needs to hide a few things!
  box.show();
  mdi.show_all();
  show();

  Document *doc = mdi.get_active();
  if (doc) {
    doc->grab_focus();
  }
#if (defined ENABLE_EMULATOR) || (defined ENABLE_MULTIPRESS)
  std::string err;
#endif
#ifdef ENABLE_EMULATOR
  if (!Emulator::ok(err)) {
    err += _(" The keyboard emulator will not work.");
    katoob_error(err);
  }
#endif
#ifdef ENABLE_MULTIPRESS
  if (!Multipress::ok(err)) {
    err += _(" Multipress will not work.");
    katoob_error(err);
  }
#endif
  std::string ver = conf.get_version();
  if ((ver.size() == 0) && conf.ok()) {
    katoob_info(_("A lot of the configuration options have been changed in this version.\nPlease "
                  "adjust the configuration first."));
    signal_preferences_activate_cb();
  }

  // DnD
  std::list<Gtk::TargetEntry> targets;
  targets.push_back(Gtk::TargetEntry("text/uri-list"));
  drag_dest_set(targets,
                Gtk::DEST_DEFAULT_ALL,
                Gdk::ACTION_DEFAULT | Gdk::ACTION_COPY | Gdk::ACTION_MOVE | Gdk::ACTION_LINK |
                    Gdk::ACTION_PRIVATE | Gdk::ACTION_ASK);

  signal_drag_data_received().connect(sigc::mem_fun(*this, &Window::signal_drag_data_received_cb));

  // Multipress.
#ifdef ENABLE_MULTIPRESS
  _multipress.signal_invalid_key.connect(sigc::mem_fun(*this, &Window::signal_invalid_key_cb));
  _multipress.signal_insert_key.connect(sigc::mem_fun(*this, &Window::signal_insert_key_cb));
#endif
#if defined(ENABLE_EMULATOR) || defined(ENABLE_MULTIPRESS)
  // Statusbar input button.
  statusbar.signal_input_toggled.connect(mem_fun(*this, &Window::signal_input_toggled_cb));
  // The input window signals.
  input_window.signal_button_clicked.connect(sigc::mem_fun(*this, &Window::signal_insert_key_cb));
  input_window.signal_dialog_closed.connect(
      sigc::mem_fun(*this, &Window::signal_input_window_dialog_closed_cb));
#endif
}

Window::~Window()
{
}

void Window::connect_toolbar_signals()
{
  toolbar.signal_create_clicked.connect(sigc::mem_fun(mdi, &MDI::create_cb));
  toolbar.signal_open_clicked.connect(sigc::mem_fun(mdi, &MDI::open_cb));
  toolbar.signal_save_clicked.connect(sigc::mem_fun(mdi, &MDI::save_cb));
#ifdef ENABLE_PRINT
  toolbar.signal_print_clicked.connect(sigc::mem_fun(mdi, &MDI::print_cb));
#endif
  toolbar.signal_close_clicked.connect(sigc::mem_fun(mdi, &MDI::close_cb));
  toolbar.signal_undo_clicked.connect(sigc::mem_fun(mdi, &MDI::undo_cb));
  toolbar.signal_redo_clicked.connect(sigc::mem_fun(mdi, &MDI::redo_cb));
  toolbar.signal_cut_clicked.connect(sigc::mem_fun(mdi, &MDI::cut_cb));
  toolbar.signal_copy_clicked.connect(sigc::mem_fun(mdi, &MDI::copy_cb));
  toolbar.signal_paste_clicked.connect(sigc::mem_fun(mdi, &MDI::paste_cb));
  toolbar.signal_erase_clicked.connect(sigc::mem_fun(mdi, &MDI::erase_cb));

  // The rest of the extended toolbar signals.
#ifdef ENABLE_SPELL
  toolbar.signal_dictionary_changed.connect(
      sigc::mem_fun(*this, &Window::signal_dictionary_changed_cb));
  toolbar.signal_spell_clicked.connect(sigc::mem_fun(mdi, &MDI::do_spell));
#endif
  toolbar.signal_search_activated.connect(
      sigc::mem_fun(*this, &Window::signal_search_activated_cb));
  toolbar.signal_go_to_activated.connect(sigc::mem_fun(mdi, &MDI::goto_line_cb2));

  toolbar.signal_extra_button_clicked.connect(
      sigc::mem_fun(mdi, &MDI::signal_extra_button_clicked_cb));
}

void Window::connect_menubar_signals()
{
  menubar.signal_create_activate.connect(sigc::mem_fun(mdi, &MDI::create_cb));
  menubar.signal_open_activate.connect(sigc::mem_fun(mdi, &MDI::open_cb));
  menubar.signal_open_location_activate.connect(sigc::mem_fun(mdi, &MDI::open_location_cb));
  menubar.signal_save_activate.connect(sigc::mem_fun(mdi, &MDI::save_cb));
  menubar.signal_save_as_activate.connect(sigc::mem_fun(mdi, &MDI::save_as_cb));
  menubar.signal_save_copy_activate.connect(sigc::mem_fun(mdi, &MDI::save_copy_cb));
  menubar.signal_revert_activate.connect(sigc::mem_fun(mdi, &MDI::revert_cb));
#ifdef ENABLE_PRINT
  menubar.signal_print_activate.connect(sigc::mem_fun(mdi, &MDI::print_cb));
  menubar.signal_print_preview_activate.connect(sigc::mem_fun(mdi, &MDI::print_preview_cb));
  menubar.signal_page_setup_activate.connect(sigc::mem_fun(mdi, &MDI::page_setup_cb));
#endif
  menubar.signal_close_activate.connect(sigc::mem_fun(mdi, &MDI::close_cb));
  menubar.signal_quit_activate.connect(sigc::mem_fun(*this, &Window::signal_quit_activate_cb));

  menubar.signal_undo_activate.connect(sigc::mem_fun(mdi, &MDI::undo_cb));
  menubar.signal_redo_activate.connect(sigc::mem_fun(mdi, &MDI::redo_cb));
  menubar.signal_cut_activate.connect(sigc::mem_fun(mdi, &MDI::cut_cb));
  menubar.signal_copy_activate.connect(sigc::mem_fun(mdi, &MDI::copy_cb));
  menubar.signal_paste_activate.connect(sigc::mem_fun(mdi, &MDI::paste_cb));
  menubar.signal_erase_activate.connect(sigc::mem_fun(mdi, &MDI::erase_cb));
  menubar.signal_select_all_activate.connect(sigc::mem_fun(mdi, &MDI::select_all_cb));
  menubar.signal_preferences_activate.connect(
      sigc::mem_fun(*this, &Window::signal_preferences_activate_cb));
  menubar.signal_insert_file_activate.connect(sigc::mem_fun(mdi, &MDI::insert_file_cb));

  menubar.signal_find_activate.connect(sigc::mem_fun(mdi, &MDI::find_cb));
  menubar.signal_find_next_activate.connect(sigc::mem_fun(mdi, &MDI::find_next_cb));
  menubar.signal_replace_activate.connect(sigc::mem_fun(mdi, &MDI::replace_cb));
  menubar.signal_goto_line_activate.connect(sigc::mem_fun(mdi, &MDI::goto_line_cb));

  signal_wrap_text_activate_conn = menubar.signal_wrap_text_activate.connect(
      sigc::mem_fun(*this, &Window::signal_wrap_text_activate_cb));
  signal_line_numbers_activate_conn = menubar.signal_line_numbers_activate.connect(
      sigc::mem_fun(*this, &Window::signal_line_numbers_activate_cb));
  menubar.signal_statusbar_activate.connect(sigc::mem_fun(statusbar, &Statusbar::show));
  menubar.signal_toolbar_activate.connect(sigc::mem_fun(toolbar, &Toolbar::show_main));
  menubar.signal_extended_toolbar_activate.connect(sigc::mem_fun(toolbar, &Toolbar::show_extended));

  // Toolbar type
  menubar.signal_text_activate.connect(sigc::mem_fun(toolbar, &Toolbar::set_text));
  menubar.signal_icons_activate.connect(sigc::mem_fun(toolbar, &Toolbar::set_icons));
  menubar.signal_both_activate.connect(sigc::mem_fun(toolbar, &Toolbar::set_both));
  menubar.signal_beside_activate.connect(sigc::mem_fun(toolbar, &Toolbar::set_beside));

  menubar.signal_execute_activate.connect(sigc::mem_fun(mdi, &MDI::execute_cb));

#ifdef ENABLE_SPELL
  menubar.signal_spell_activate.connect(sigc::mem_fun(mdi, &MDI::do_spell));
  signal_auto_spell_activate_conn =
      menubar.signal_auto_spell_activate.connect(sigc::mem_fun(mdi, &MDI::set_auto_spell));
#endif
  menubar.signal_save_all_activate.connect(sigc::mem_fun(mdi, &MDI::save_all_cb));
  menubar.signal_close_all_activate.connect(sigc::mem_fun(mdi, &MDI::close_all_cb));

  menubar.signal_about_activate.connect(sigc::ptr_fun(&AboutDialog::run));

  menubar.signal_recent_activate.connect(sigc::mem_fun(mdi, &MDI::recent_cb));
#if defined(ENABLE_EMULATOR) || defined(ENABLE_MULTIPRESS)
  menubar.signal_layout_activate.connect(sigc::mem_fun(*this, &Window::signal_layout_activate_cb));
#endif
  menubar.signal_encoding_activate.connect(
      sigc::mem_fun(*this, &Window::signal_encoding_activate_cb));
  menubar.signal_document_activate.connect(sigc::mem_fun(mdi, &MDI::activate));

#ifdef ENABLE_HIGHLIGHT
  menubar.signal_highlighter_activate.connect(sigc::mem_fun(mdi, &MDI::set_highlight));
#endif
  menubar.signal_closed_document_activate.connect(
      sigc::mem_fun(mdi, &MDI::closed_document_activated_cb));

  // Import
  menubar.signal_import_activate.connect(sigc::mem_fun(mdi, &MDI::import_cb));
  // Export
  menubar.signal_export_activate.connect(sigc::mem_fun(mdi, &MDI::export_cb));
}

void Window::connect_mdi_signals()
{
#ifdef ENABLE_SPELL
  mdi.signal_document_spell_enabled.connect(
      sigc::mem_fun(*this, &Window::on_document_spell_enabled_cb));
#endif
  mdi.signal_document_wrap_text.connect(
      sigc::mem_fun(*this, &Window::signal_document_wrap_text_cb));
  mdi.signal_document_line_numbers.connect(
      sigc::mem_fun(*this, &Window::signal_document_line_numbers_cb));

  mdi.signal_recent_regenerate.connect(sigc::mem_fun(menubar, &MenuBar::create_recent));

#ifdef ENABLE_HIGHLIGHT
  mdi.signal_document_highlight.connect(sigc::mem_fun(menubar, &MenuBar::set_highlight));
#endif
  // Document status signals.
  mdi.signal_document_encoding_changed.connect(
      sigc::mem_fun(*this, &Window::signal_document_encoding_changed_cb));
  mdi.signal_document_overwrite_toggled.connect(
      sigc::mem_fun(*this, &Window::signal_document_overwrite_toggled_cb));
  mdi.signal_document_cursor_moved.connect(
      sigc::mem_fun(*this, &Window::signal_document_cursor_moved_cb));
  mdi.signal_document_file_changed.connect(
      sigc::mem_fun(*this, &Window::signal_document_file_changed_cb));
  mdi.signal_document_readonly.connect(sigc::mem_fun(*this, &Window::signal_document_readonly_cb));
  mdi.signal_document_can_redo.connect(sigc::mem_fun(*this, &Window::signal_document_can_redo_cb));
  mdi.signal_document_can_undo.connect(sigc::mem_fun(*this, &Window::signal_document_can_undo_cb));
  mdi.signal_document_modified.connect(sigc::mem_fun(*this, &Window::signal_document_modified_cb));
  mdi.signal_document_title_changed.connect(
      sigc::mem_fun(*this, &Window::signal_document_title_changed_cb));
#ifdef ENABLE_SPELL
  mdi.signal_document_dictionary_changed.connect(
      sigc::mem_fun(*this, &Window::signal_document_dictionary_changed_cb));
#endif

  mdi.signal_doc_activated.connect(sigc::mem_fun(*this, &Window::on_doc_activated));
  mdi.signal_reset_gui.connect(sigc::mem_fun(*this, &Window::on_reset_gui));
  mdi.signal_document_added.connect(sigc::mem_fun(*this, &Window::on_document_added_cb));
  mdi.signal_document_removed.connect(sigc::mem_fun(*this, &Window::on_document_removed_cb));

  // undo closed documents signals.
  mdi.signal_closed_document_erased.connect(
      sigc::mem_fun(menubar, &MenuBar::signal_closed_document_erased_cb));
  mdi.signal_closed_document_added.connect(
      sigc::mem_fun(menubar, &MenuBar::signal_closed_document_added));
}

void Window::signal_drag_data_received_cb(const Glib::RefPtr<Gdk::DragContext> &context,
                                          int x,
                                          int y,
                                          const Gtk::SelectionData &selection,
                                          guint info,
                                          guint time)
{
  std::vector<std::string> files = selection.get_uris();
  std::string filename;
#ifndef GLIBMM_EXCEPTIONS_ENABLED
  std::auto_ptr<Glib::Error> error;
#endif

  for (unsigned x = 0; x < files.size(); x++) {
#ifdef GLIBMM_EXCEPTIONS_ENABLED
    try {
      filename = Glib::filename_from_uri(files[x]);
      mdi.create_document(filename);
    } catch (Glib::ConvertError &e) {
      katoob_error(e.what());
    }
#else
    filename = Glib::filename_from_uri(files[x], error);
    if (error.get()) {
      katoob_error(error->what());
      error.reset();
    }
#endif
  }
}

void Window::set_title(const char *str)
{
  std::stringstream title;
  if (str) {
    title << str << " - ";
  }
  title << PACKAGE;
  Gtk::Window::set_title(title.str());
}

void Window::signal_preferences_activate_cb()
{
  PreferencesDialog dialog(_conf, _encodings);
  dialog.signal_apply_clicked.connect(sigc::mem_fun(*this, &Window::reset_gui));

  if (dialog.run()) {
    reset_gui();
  }
}

#ifdef ENABLE_SPELL
void Window::on_document_spell_enabled_cb(bool s)
{
  signal_auto_spell_activate_conn.block();
  menubar.enable_auto_spell(s);
  toolbar.enable_dictionary(s);
  signal_auto_spell_activate_conn.unblock();
}
#endif

bool Window::signal_delete_event_cb(GdkEventAny *event)
{
  if (mdi.close_all()) {
    signal_quit.emit();
    return false;
  } else {
    return true;
  }
}

void Window::signal_quit_activate_cb()
{
  if (mdi.close_all()) {
    signal_quit.emit();
  }
}

#if defined(ENABLE_MULTIPRESS) || defined(ENABLE_EMULATOR)
void Window::signal_layout_activate_cb(int which, int x)
{
  // TODO: Store the input_window state and restore it when we enable any.
  assert(which < 2);
#ifdef ENABLE_EMULATOR
  if (which == 0) {
    _emulator.activate(x);
#ifdef ENABLE_MULTIPRESS
    _multipress.activate(-1);
    statusbar.activate_input(true);
    if (input_window.is_visible()) {
      signal_input_toggled_cb(true);
    }
#endif
  } else
#endif
#ifdef ENABLE_MULTIPRESS
      if (which == 1) {

    _multipress.activate(x);
#ifdef ENABLE_EMULATOR
    _emulator.activate(-1);
#endif
    statusbar.activate_input(true);
    if (input_window.is_visible()) {
      signal_input_toggled_cb(true);
    }
  } else
#endif
  {
    // x is -1 here but I'll set it manually just in case.
#ifdef ENABLE_EMULATOR
    _emulator.activate(-1);
#endif
#ifdef ENABLE_MULTIPRESS
    _multipress.activate(-1);
    statusbar.activate_input(false);
    statusbar.set_input_status(false);
#endif
  }
}
#endif

void Window::signal_encoding_activate_cb(int e)
{
  int x;
  if (!mdi.set_encoding(e, x)) {
    menubar.set_encoding(x);
  }
}

void Window::signal_document_readonly_cb(int x, bool r)
{
  menubar.set_read_only(x, r);
  toolbar.set_read_only(r);
}

void Window::on_reset_gui(int x)
{
  bool enable = (x == -1) ? false : true;

  toolbar.reset_gui(enable);
  menubar.reset_gui(enable);

  statusbar.set_overwrite(false);
  set_title();
}

void Window::signal_document_can_redo_cb(bool can)
{
  menubar.enable_redo(can);
  toolbar.enable_redo(can);
}

void Window::signal_document_can_undo_cb(bool can)
{
  menubar.enable_undo(can);
  toolbar.enable_undo(can);
}

void Window::signal_document_modified_cb(int x, bool m)
{
  statusbar.set_modified(m);
  menubar.document_set_modified(x, m);
}

void Window::signal_document_file_changed_cb(std::string f)
{
  // TODO: Do we need this ??
  //  set_title(const_cast<char *>(f.c_str()));
}

void Window::signal_document_encoding_changed_cb(int e)
{
  statusbar.set_encoding(_encodings.name(e));
  menubar.set_encoding(e);
}

void Window::signal_document_title_changed_cb(std::string t, int x)
{
  set_title(t.c_str());
  menubar.document_set_label(x, t);
}

void Window::on_document_added_cb(bool ro, bool m, std::string t)
{
  set_title(t.c_str());
  menubar.document_add(t, ro, m);
}

void Window::on_document_removed_cb(int x)
{
  menubar.document_remove(x);
}

void Window::on_doc_activated(int x)
{
  menubar.document_set_active(x);
}

void Window::signal_document_wrap_text_cb(bool w)
{
  signal_wrap_text_activate_conn.block();
  menubar.enable_wrap_text(w);
  signal_wrap_text_activate_conn.unblock();
}

void Window::signal_document_line_numbers_cb(bool ln)
{
  signal_line_numbers_activate_conn.block();
  menubar.enable_line_numbers(ln);
  signal_line_numbers_activate_conn.unblock();
}

void Window::signal_search_activated_cb(std::string s)
{
  mdi.find(s);
}

void Window::reset_gui()
{
  toolbar.reset_gui();
  menubar.reset_gui();
  statusbar.reset_gui();
  mdi.reset_gui();

  // NOTE: Should we enable this ??
  /*
    move(_conf.get("x", 50), _conf.get("y", 50));
    resize(_conf.get("w", 500), _conf.get("h", 400));
  */
}

#ifdef ENABLE_SPELL
void Window::signal_dictionary_changed_cb(std::string d)
{
  std::string old;
  if (!mdi.set_dictionary(old, d)) {
    toolbar.set_dictionary(old);
  }
}
#endif

#ifdef ENABLE_DBUS
void Window::open_files(std::vector<std::string> &f)
{
  for (unsigned int x = 0; x < f.size(); x++) {
    mdi.create_document(f[x]);
  }
  if (f.size() == 0) {
    mdi.create_document();
  }
  present();
}
#endif

void Window::signal_wrap_text_activate_cb(bool s)
{
  mdi.set_wrap_text(s);
}

void Window::signal_line_numbers_activate_cb(bool s)
{
  mdi.set_line_numbers(s);
}

#ifdef ENABLE_MULTIPRESS
void Window::signal_insert_key_cb(std::string &str)
{
  Document *doc = mdi.get_active();
  assert(doc != NULL);
  if ((!doc) || (doc->get_readonly())) {
    return;
  }
  doc->insert(str);
}

void Window::signal_invalid_key_cb(GdkEventKey *event)
{
#ifdef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
  Gtk::Window::on_key_press_event(event);
#else
  // TODO: This is not that good.
  Document *doc = mdi.get_active();
  if ((event->length > 0) && (doc) && (!doc->get_readonly())) {
    gunichar ch = gdk_unicode_to_keyval(event->keyval);
    if (ch != event->keyval | 0x01000000) {
      gchar out[6];
      int len = g_unichar_to_utf8(ch, out);
      if (len != 0) {
        std::string str(out, len);
        doc->insert(str);
      }
    }
  }
#endif
}
#endif

#if defined(ENABLE_EMULATOR) || defined(ENABLE_MULTIPRESS)
void Window::signal_input_toggled_cb(bool active)
{
  if (!active) {
    input_window.hide();
    return;
  }

#ifdef ENABLE_EMULATOR
  if (_emulator.get_active()) {
    input_window.set_layout(_emulator.get_layout());
  } else
#endif
#ifdef ENABLE_MULTIPRESS
      if (_multipress.get_active()) {
    input_window.set_layout(_multipress.get_layout());
  }
#endif
  input_window.show();
}

void Window::signal_input_window_dialog_closed_cb()
{
  statusbar.set_input_status(false);
}
#endif

void Window::autosave()
{
  mdi.autosave();
}

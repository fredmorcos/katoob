/*
 * window.cc
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <cassert>
#include <iostream>
#include <sstream>
#include "bars.hh"
#include "aboutdialog.hh"
#include "window.hh"
#include "utils.hh"
#include "macros.h"
#include "dialogs.hh"
#include "preferencesdialog.hh"
#include "document.hh"
#include "docfactory.hh"

#ifdef ENABLE_MAEMO
#include <gtkmm/settings.h>
#endif

Window::Window(Conf& conf, Encodings& encodings, std::vector<std::string>& files) :
  _conf(conf),
  _encodings(encodings),
  mdi(conf, encodings),
  statusbar(conf),
  box(false, 0) {

  try {
    bars = new Bars(&conf);
  }
  catch (...) {
    // We should not be here.
  }

  bars->set_encodings(&encodings);

  import_init(_import);
  export_init(_export);

  bars->set_import(_import);
  bars->set_export(_export);

  Gtk::Widget *menubar = bars->get_menubar(), *toolbar = bars->get_toolbar();
#ifdef ENABLE_MAEMO
  // TODO: Ugly.
  g_object_set(Gtk::Settings::get_default()->gobj(), "gtk-menu-images", FALSE, NULL);

  // Ugly too.
  if (menubar) {
    Gtk::Container *bar = dynamic_cast<Gtk::Container *>(menubar);
    if (bar) {
      Gtk::Menu *menu = Gtk::manage(new Gtk::Menu);
      std::vector<Gtk::Widget *> children = bar->get_children();
      for (unsigned x = 0; x < children.size(); x++) {
	children[x]->reparent(*menu);
      }
      Hildon::Window::set_menu(*menu);
    }
  }
  if (toolbar) {
    Gtk::Toolbar *tb = dynamic_cast<Gtk::Toolbar *>(toolbar);
    if (tb) {
      Hildon::Window::add_toolbar(*tb);
    }
  }
#else
  if (menubar) {
    box.pack_start(*menubar, Gtk::PACK_SHRINK, 0);
  }

  if (toolbar) {
    box.pack_start(*toolbar, false, false, 0);
  }
#endif
  if (bars->get_toolbar_extended()) {
    box.pack_start(*(bars->get_toolbar_extended()), Gtk::PACK_SHRINK, 0);
  }

  if ((!bars->get_toolbar()) || (!bars->get_toolbar_extended()) || (!bars->get_menubar())) {
    katoob_error(_("Failed to create parts of the user interface."));
  }

  add_accel_group(bars->get_accel_group());

  box.pack_start(mdi, Gtk::PACK_EXPAND_WIDGET, 0);
  box.pack_start(statusbar, Gtk::PACK_SHRINK, 0);

  add(box);

  // Now let's connect our signals.
  connect_bars_signals();
  connect_mdi_signals();
  connect_other_signals();

  reset_gui();

  mdi.scan_temp();
  // This is here after we connect the signals
  // So we can get the signals emitted when we create the first document.
  // Now let's create the children.
  // If MDI has an active Document then we managed to recover something.
  // And in that case, we won't create a new document.
  // But we only process files we are supposed to open.

  for (unsigned int x = 0; x < files.size(); x++) {
    mdi.create_document(files[x]);
  }

  files.clear();

  // It's possible that have failed to detect the encoding of all the files, to recover
  // anything because we failed or because there weren't any.
  // In this case, we will create a blank document.
  // We can't the number of the documents because MDI has no size method.

  if (!mdi.get_active()) {
    mdi.create_document();
  }

  move(conf.get("x", 50), conf.get("y", 50));
  resize (conf.get("w", 500), conf.get("h", 400));

  set_icon_from_file(Utils::get_data_path("katoob-small.png"));

  // We don't call this because we are sure we have a Document so we have a title set.
  //  set_title();

  // We don't call show_all(); here as our statusbar needs to hide a few things!
  box.show();
  mdi.show_all();
  show();

  Document *doc = mdi.get_active();
  if (doc) {
    set_focus(*doc);
  }

  std::string ver = conf.get_version();
  if ((ver.size() == 0) && conf.ok()) {
    katoob_info(_("A lot of the configuration options have been changed in this version.\nPlease adjust the configuration first."));
    signal_preferences_activate_cb();
  }

  // DnD
  std::list<Gtk::TargetEntry> targets;
  targets.push_back(Gtk::TargetEntry("text/uri-list") );
  drag_dest_set(targets, Gtk::DEST_DEFAULT_ALL, Gdk::ACTION_DEFAULT | Gdk::ACTION_COPY |
		Gdk::ACTION_MOVE | Gdk::ACTION_LINK | Gdk::ACTION_PRIVATE | Gdk::ACTION_ASK);

#ifdef ENABLE_MAEMO
  // TODO: Get rid of this.
  is_fullscreen = false;
#endif
}

Window::~Window() {
  delete bars;
  bars = NULL;

  for (unsigned x = 0; x < _import.size(); x++) {
    delete _import[x];
  }

  for (unsigned x = 0; x < _export.size(); x++) {
    delete _export[x];
  }

  _import.clear();
  _export.clear();
}

void Window::connect_bars_signals() {
  bars->signal_request_active.connect(sigc::mem_fun(mdi, &MDI::get_active));

  bars->signal_new.connect(sigc::mem_fun(mdi, &MDI::create_cb));
  bars->signal_open.connect(sigc::mem_fun(mdi, &MDI::open_cb));
  bars->signal_open_location.connect(sigc::mem_fun(mdi, &MDI::open_location_cb));
  bars->signal_save.connect(sigc::mem_fun(mdi, &MDI::save_cb));
#ifdef ENABLE_PRINT
  bars->signal_print.connect(sigc::mem_fun(mdi, &MDI::print_cb));
#endif
  bars->signal_close.connect(sigc::mem_fun(mdi, &MDI::close_cb));
  bars->signal_undo.connect(sigc::mem_fun(mdi, &MDI::undo_cb));
  bars->signal_redo.connect(sigc::mem_fun(mdi, &MDI::redo_cb));
  bars->signal_cut.connect(sigc::mem_fun(mdi, &MDI::cut_cb));
  bars->signal_copy.connect(sigc::mem_fun(mdi, &MDI::copy_cb));
  bars->signal_paste.connect(sigc::mem_fun(mdi, &MDI::paste_cb));
  bars->signal_delete.connect(sigc::mem_fun(mdi, &MDI::erase_cb));
  bars->signal_save_as.connect(sigc::mem_fun(mdi, &MDI::save_as_cb));
  bars->signal_revert.connect(sigc::mem_fun(mdi, &MDI::revert_cb));
  bars->signal_quit.connect(sigc::mem_fun(*this, &Window::signal_quit_activate_cb));
#ifdef ENABLE_SPELL
  bars->signal_spell.connect(sigc::mem_fun(mdi, &MDI::do_spell));
#endif
  bars->signal_find.connect(sigc::mem_fun(mdi, &MDI::find_cb));
  bars->signal_find_next.connect(sigc::mem_fun(mdi, &MDI::find_next_cb));
  bars->signal_find_replace.connect(sigc::mem_fun(mdi, &MDI::replace_cb));
  bars->signal_about.connect(sigc::ptr_fun(&AboutDialog::run));
#ifdef ENABLE_PRINT
  bars->signal_print_preview.connect(sigc::mem_fun(mdi, &MDI::print_preview_cb));
  bars->signal_page_setup.connect(sigc::mem_fun(mdi, &MDI::page_setup_cb));
#endif
  bars->signal_save_copy.connect(sigc::mem_fun(mdi, &MDI::save_copy_cb));
  bars->signal_select_all.connect(sigc::mem_fun(mdi, &MDI::select_all_cb));
  bars->signal_save_all.connect(sigc::mem_fun(mdi, &MDI::save_all_cb));
  bars->signal_close_all.connect(sigc::mem_fun(mdi, &MDI::close_all_cb));
  bars->signal_goto.connect(sigc::mem_fun(mdi, &MDI::goto_line_cb));
  bars->signal_preferences.connect(sigc::mem_fun(*this, &Window::signal_preferences_activate_cb));
  bars->signal_insert_file.connect(sigc::mem_fun(mdi, &MDI::insert_file_cb));
  bars->signal_statusbar.connect(sigc::mem_fun(statusbar, &Statusbar::show));

  bars->signal_wrap_text.connect(sigc::mem_fun(*this, &Window::signal_wrap_text_activate_cb));
  bars->signal_line_numbers.connect(sigc::mem_fun(*this, &Window::signal_line_numbers_activate_cb));

#ifdef ENABLE_SPELL
  bars->signal_auto_spell.connect(sigc::mem_fun(mdi, &MDI::set_auto_spell));
#endif
  bars->signal_execute.connect(sigc::mem_fun(mdi, &MDI::execute_cb));

  bars->signal_goto_line.connect(sigc::mem_fun(mdi, &MDI::goto_line_cb2));
#ifdef ENABLE_SPELL
  bars->signal_spell_dict_changed.connect(sigc::mem_fun(*this, &Window::signal_dictionary_changed_cb));
#endif
  bars->signal_extra_button.connect(sigc::mem_fun(mdi, &MDI::signal_extra_button_clicked_cb));
  bars->signal_search_string.connect(sigc::mem_fun(*this, &Window::signal_search_activated_cb));

  bars->signal_recent.connect(sigc::mem_fun(mdi, &MDI::recent_cb));

  bars->signal_encoding.connect(sigc::mem_fun(*this, &Window::signal_encoding_activate_cb));

  bars->signal_open_document.connect(sigc::mem_fun(mdi, &MDI::activate));
  bars->signal_closed_document.connect(sigc::mem_fun(mdi, &MDI::closed_document_activated_cb));

#ifdef ENABLE_MAEMO
  bars->signal_full_screen.connect(sigc::mem_fun(*this, &Window::toggle_full_screen));
#endif

  // Import
  bars->signal_import.connect(sigc::mem_fun(mdi, &MDI::import_cb));
  // Export
  bars->signal_export.connect(sigc::mem_fun(mdi, &MDI::export_cb));

#ifdef ENABLE_HIGHLIGHT
  bars->signal_highlight.connect(sigc::mem_fun(mdi, &MDI::set_highlight));
#endif
}

void Window::connect_other_signals() {
  signal_drag_data_received().connect(sigc::mem_fun(*this, &Window::signal_drag_data_received_cb));

#ifdef ENABLE_MAEMO
  signal_window_state_event().connect(sigc::mem_fun(*this, &Window::signal_window_state_event_cb));
#endif

  signal_delete_event().connect(sigc::mem_fun(*this, &Window::signal_delete_event_cb));
}

void Window::connect_mdi_signals() {
#ifdef ENABLE_SPELL
  mdi.signal_document_spell_enabled.connect(sigc::mem_fun(*this, &Window::on_document_spell_enabled_cb));
#endif

  // Document status signals.
  mdi.signal_document_encoding_changed.connect(sigc::mem_fun(*this, &Window::signal_document_encoding_changed_cb));
  mdi.signal_document_file_changed.connect(sigc::mem_fun(*this, &Window::signal_document_file_changed_cb));
  mdi.signal_document_rom.connect(sigc::mem_fun(*this, &Window::signal_document_rom_cb));
  mdi.signal_document_title_changed.connect(sigc::mem_fun(*this, &Window::signal_document_title_changed_cb));

  mdi.signal_doc_activated.connect(sigc::mem_fun(*this, &Window::on_doc_activated));
  mdi.signal_reset_gui.connect(sigc::mem_fun(*this, &Window::on_reset_gui));
  DocFactory::get()->signal_created.connect(sigc::mem_fun(*this, &Window::on_document_added_cb));
}

void Window::signal_drag_data_received_cb(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, const Gtk::SelectionData& selection, guint info, guint time) {
  std::vector<std::string> files = selection.get_uris();
  std::string filename;
  for (unsigned x = 0; x < files.size(); x++) {
    if (Utils::filename_from_uri(files[x], filename)) {
      mdi.create_document(filename);
    }
    else {
      katoob_error(filename);
    }
  }
}

void Window::set_title(const char *str) {
#ifdef ENABLE_MAEMO
  if (str) {
    Gtk::Window::set_title(str);
  }
#else
  std::stringstream title;
  if (str) {
    title << str << " - ";
  }
  title << PACKAGE;
  Gtk::Window::set_title(title.str());
#endif
}

void Window::signal_preferences_activate_cb() {
  PreferencesDialog dialog(_conf, _encodings);
  dialog.signal_apply_clicked.connect(sigc::mem_fun(*this, &Window::reset_gui));

  if (dialog.run()) {
    reset_gui();
  }
}

#ifdef ENABLE_SPELL
void Window::on_document_spell_enabled_cb(bool s) {
  bars->enable_spell(mdi.get_active()->get_dictionary(), s);
}
#endif

bool Window::signal_delete_event_cb(GdkEventAny *event) {
  if (mdi.close_all()) {
    signal_quit.emit();
    return false;
  }
  else {
    return true;
  }
}

void Window::signal_quit_activate_cb() {
  if (mdi.close_all()) {
    signal_quit.emit();
  }
}

void Window::signal_encoding_activate_cb(int e) {
  int x;
  if(!mdi.set_encoding(e, x)) {
    bars->set_encoding(x);
  }
}

void Window::on_reset_gui(bool enable) {
  bars->reset_widgets(enable);

  statusbar.set_overwrite(false);
  statusbar.set_modified(false);
  statusbar.set_position(0, 0);

  set_title();
}

void Window::signal_document_rom_cb(int x, bool ro, bool m) {
  statusbar.set_modified(m);
  bars->set_rom(x, ro, m);
}

void Window::signal_document_file_changed_cb(std::string f) {
  // TODO: Do we need this ??
  //  set_title(const_cast<char *>(f.c_str()));
}

void Window::signal_document_encoding_changed_cb(int e) {
  statusbar.set_encoding(_encodings.name(e));
  bars->set_encoding(e);
}

void Window::signal_document_title_changed_cb(std::string t, int x) {
  set_title(t.c_str());
  bars->set_label(x, t);
}

void Window::on_document_added_cb(Document *doc) {
  set_title(doc->get_title().c_str());
}

void Window::on_doc_activated(int x) {
  bars->set_active_document(x);
}

void Window::signal_search_activated_cb(std::string s) {
  mdi.find(s);
}

void Window::reset_gui() {
  DocFactory::get()->reset_ui();
  bars->reset_ui();
  statusbar.reset_gui();
  mdi.reset_gui();
}

#ifdef ENABLE_SPELL
void Window::signal_dictionary_changed_cb(std::string d) {
  std::string old;
  if (!mdi.set_dictionary(old, d)) {
    bars->set_spell_dict(old);
  }
}
#endif

#ifdef ENABLE_DBUS
void Window::open_files(std::vector<std::string>& f) {
  for (unsigned int x = 0; x < f.size(); x++) {
    mdi.create_document(f[x]);
  }
  if (f.size() == 0) {
    mdi.create_document();
  }
  present();
}
#endif

void Window::signal_wrap_text_activate_cb(bool s) {
  mdi.set_wrap_text(s);
}

void Window::signal_line_numbers_activate_cb(bool s) {
  mdi.set_line_numbers(s);
}

#ifdef ENABLE_MAEMO
bool Window::signal_window_state_event_cb(GdkEventWindowState* event) {
  if (event->type == GDK_WINDOW_STATE) {
    if (event->changed_mask & GDK_WINDOW_STATE_FULLSCREEN) {
      if (event->new_window_state & GDK_WINDOW_STATE_FULLSCREEN) {
	is_fullscreen = true;
      }
      else {
	is_fullscreen = false;
      }
    }
  }

  return false;
}

void Window::toggle_full_screen() {
  if (is_fullscreen) {
    unfullscreen();
  }
  else {
    fullscreen();
  }
}

void Window::signal_request_top_cb() {
  present();
}
#endif

//void Window::autosave() {
  //  mdi.autosave();
//}

void Window::set_icon_from_file(const std::string& file) {
  std::string error;

#ifdef GLIBMM_EXCEPTIONS_ENABLED
  try {
    Gtk::Window::set_icon_from_file(file);
  }
  catch (Glib::Error& er) {
    error = er.what();
  }
  catch (...) {
    // NOTE: Why the hell can't I catch Glib::Error or Glib::FileError under win32 ?
    error = "I can't set the main window icon to " + file;
  }
#else
  std::auto_ptr<Glib::Error> err;
  Hildon::Window::set_icon_from_file(file, err);
  if (err.get()) {
    error = err->what();
  }
#endif

#if !defined(_WIN32) || defined(NDEBUG)
  if (error.size() > 0) {
    std::cout << error << std::endl;
  }
#endif
}


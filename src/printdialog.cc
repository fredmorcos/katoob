/*
 * printdialog.cc
 * This file is part of katoob
 *
 * Copyright (C) 2006 Mohammed Sameer
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

#include <string>
#include <cassert>
#include <cerrno>
#include <gtkmm/stock.h>
#include <gtkmm/scrolledwindow.h>
#include "printdialog.hh"
#include "macros.h"
#include "utils.hh"
#include "filedialog.hh"
#include "dialogs.hh"
#include "previewdialog.hh"

struct Paper {
  std::string name;
  glong width;
  glong height;
};

/* *INDENT-OFF* */

/* from: http://astronomy.swin.edu.au/~pbourke/geomformats/postscript/ */
Paper Papers[] = {
  {"Letter", 612 , 792 },
  {"Legal" , 612 , 1008},
  {"A0"    , 2384, 3370},
  {"A1"    , 1684, 2384},
  {"A2"    , 1191, 1684},
  {"A3"    , 842 , 1191},
  {"A4"    , 595 , 842 },
  {"A5"    , 420 , 595 },
  {"A6"    , 297 , 420 },
  {"A7"    , 210 , 297 },
  {"A8"    , 148 , 210 },
  {"A9"    , 105 , 148 },
  {"B0"    , 2920, 4127},
  {"B1"    , 2064, 2920},
  {"B2"    , 1460, 2064},
  {"B3"    , 1032, 1460},
  {"B4"    , 729 , 1032},
  {"B5"    , 516 , 729 },
  {"B6"    , 363 , 516 },
  {"B7"    , 258 , 363 },
  {"B8"    , 181 , 258 },
  {"B9"    , 127 , 181 },
  {"B10"   , 91  , 127 },
  {NULL    , 0x0 , 0x0 }
};
/* *INDENT-ON* */

PrintDialog::PrintDialog(Conf& conf) :
  _conf(conf),
  to_file(_("Print to file.")),
  all_radio(range_group, _("_All"), true),
  selection_radio(range_group, _("_Selection"), true),
  lines_radio(range_group, _("_Lines"), true),
  printer_frame(_("Printer:")),
  from_adj(1, 1, 1),
  to_adj(1, 1, 1),
  copies_button_adj(_conf.print_get("copies", 1), 1, 100),
  lines_from(from_adj),
  lines_to(to_adj),
  copies_button(copies_button_adj),
  browse(Gtk::Stock::SAVE_AS),
  location_label(_("Location:")),
  copies_label(_("Number of copies")),
  range_frame(_("Print range")),
  from_label(_("From:")),
  to_label(_("To:")),
  paper_size_label(_("Paper size:")),
  orientation_label(_("Page orientation:")),
  backend(conf) {
  dialog.set_title (_("Print"));
  dialog.set_position (Gtk::WIN_POS_CENTER);
  dialog.set_modal (true);
  /*
  set_resizable (false);
  */
  Gtk::VBox *box = dialog.get_vbox();
  box->set_spacing(5);

  record.add(id);
  record.add(name);
  record.add(status);

  store = Gtk::ListStore::create(record);
  treeview.set_model(store);

  treeview.append_column(_("Printer name"), name);
  treeview.append_column(_("Status"), status);

  window.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  window.add(treeview);
  window.set_size_request(-1, 100);

  printer_frame.add(window);
  box->pack_start(printer_frame);

  std::vector<std::string> all_printers = backend.get_all();
  for (unsigned i = 0; i < all_printers.size(); i++) {
    Gtk::TreeModel::Row row = *(store->append ());
    row[id] = i;
    row[name] = all_printers[i];
    row[status] = backend.get_status(i);
  }
  Gtk::TreeModel::Row row = *(store->append ());
  row[id] = -1;
  row[name] = _("Custom");

  box1.pack_start(to_file);

  print_menu.append_text(_("Create a PDF document"));
  print_menu.append_text(_("Create a Postscript document"));

  box1.pack_start(print_menu, false, false);
  box->pack_start(box1);

  box2.pack_start(location_label, false, false);

  box2.pack_start(entry);

  box2.pack_start(browse, false, false);

  box->pack_start(box2, false, false);

  box3.pack_start(copies_label, false, false);

  box3.pack_start(copies_button, false, false);

  box->pack_start(box3, false, false);

  vbox.set_spacing(5);

  vbox.pack_start(box4, true, true);
  vbox.pack_start(box5, true, true);

  box4.set_spacing(5);
  box4.set_spacing(5);

  range_frame.add(vbox);

  box4.pack_start(all_radio);
  box4.pack_start (selection_radio);
  box4.pack_start (lines_radio);

  box5.pack_start(from_label, false, false);
  box5.pack_start(lines_from, true, false);
  box5.pack_start(to_label, false, false);
  box5.pack_start(lines_to, true, false);

  box->pack_start(range_frame, false, false);

  box6.pack_start(paper_size_label, false, false);
  box6.pack_start(paper_size, true, true);

  box7.pack_start(orientation_label, false, false);
  box7.pack_start(orientation, true, true);

  orientation.append_text(_("Portrait"));
  orientation.append_text(_("Landscape"));

  box->pack_start(box6, false, false);
  box->pack_start(box7, false, false);

#if 0
  Gtk::Frame custom_frame(_("Custom"));
  box6.pack_start(custom_frame, false, false);

  Gtk::VBox vbox;
  custom_frame.add(vbox);
  Gtk::HBox h1, h2;
  Gtk::Label custom_width(_("Width:")), custom_height(_("Height:"));
  Gtk::SpinButton width(adjustment1), height(adjustment1);
#endif

  dialog.add_button(Gtk::Stock::PRINT, Gtk::RESPONSE_OK);
  dialog.add_button(Gtk::Stock::PRINT_PREVIEW, Gtk::RESPONSE_APPLY);
  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);

  t_selection = treeview.get_selection();

  /* Signals. */
  to_file.signal_toggled().connect(sigc::mem_fun(*this, &PrintDialog::to_file_toggled_cb));
  all_radio.signal_toggled().connect(sigc::mem_fun(*this, &PrintDialog::all_radio_toggled_cb));
  selection_radio.signal_toggled().connect(sigc::mem_fun(*this, &PrintDialog::selection_radio_toggled_cb));
  lines_radio.signal_toggled().connect(sigc::mem_fun(*this, &PrintDialog::lines_radio_toggled_cb));
  browse.signal_clicked().connect(sigc::mem_fun(*this, &PrintDialog::browse_button_clicked_cb));
  print_menu.signal_changed().connect(sigc::mem_fun(*this, &PrintDialog::print_menu_changed_cb));
  t_selection->signal_changed().connect(sigc::mem_fun(*this, &PrintDialog::selection_signal_changed_cb));

  // TODO: Better way.
  std::string t = Utils::prepend_home_dir("katoob.ps");
  _ps = _conf.print_get("file_entry", t);
  t = Utils::prepend_home_dir("katoob.pdf");
  _pdf = _conf.print_get("pdf_entry", t);

  to_file_toggled_cb();

  lines_radio.set_sensitive(false);
  lines_from.set_sensitive(false);
  lines_to.set_sensitive(false);
  has_selection(false);

  orientation.set_active(_conf.print_get("page_orientation", 0));
  print_menu.set_active(_conf.print_get("print_to_file", 0));

  dialog.show_all();
}

PrintDialog::~PrintDialog() {

}

bool PrintDialog::run() {
  while (true) {
    res = dialog.run();

    if ((res == Gtk::RESPONSE_OK) || (res == Gtk::RESPONSE_APPLY)) {
      std::string file = entry.get_text();
      if (to_file.get_active()) {
	if (file.size() == 0) {
	  katoob_error(_("Please choose a file to print to."));
	  continue;
	}
	if (Glib::file_test(file, Glib::FILE_TEST_IS_DIR)) {
	  katoob_error(_("Please choose a file not a directory."));
	  continue;
	}
	if (Glib::file_test(file, Glib::FILE_TEST_EXISTS)) {
	  if (katoob_simple_question(Utils::substitute(_("Are you sure you want to overwrite the file %s ?"), file))) {
	    return init_backend();
	  }
	  else {
	    continue;
	  }
	}
	return init_backend();
      }
      else {
	return init_backend();
      }
    }
    return false;
  }
}

void PrintDialog::has_selection(bool sel) {
  _has_selection = sel;
  selection_radio.set_sensitive(sel);
}

void PrintDialog::lines(int num) {
  if (num == 0) {
    return;
  }
  lines_radio.set_sensitive(true);
  /*
  lines_from.set_sensitive(true);
  lines_to.set_sensitive(true);
  */
  from_adj.set_upper(num-1);
  to_adj.set_upper(num);
}

void PrintDialog::reset_paper_size(int x) {
  paper_size.clear();
  if (x == -1) {
    Paper *_Papers = Papers;
    while (_Papers->name) {
      paper_size.append_text(_Papers->name);
      _Papers++;
    }
    paper_size.set_active(_conf.print_get("paper_size", 6)); //NOTE: A4 = 6, We need a better way to get the default.
  }
  else {
    std::vector<std::string> s = backend.sizes(x);
    for (unsigned i = 0; i < s.size(); i++) {
      paper_size.append_text(s[i]);
    }
    paper_size.set_active_text(backend.get_default(x));
  }
}

void PrintDialog::to_file_toggled_cb() {
  bool active = to_file.get_active();
  printer_frame.set_sensitive(!active);
  print_menu.set_sensitive(active);
  box2.set_sensitive(active);
  copies_button.set_sensitive(!active);
  if (active) {
    reset_paper_size(-1);
  }
  else {
    selection_signal_changed_cb();
  }
}

void PrintDialog::all_radio_toggled_cb() {
  bool active = all_radio.get_active();
  lines_from.set_sensitive(!active);
  lines_to.set_sensitive(!active);
}

void PrintDialog::selection_radio_toggled_cb() {
  bool active = selection_radio.get_active();
  lines_from.set_sensitive(!active);
  lines_to.set_sensitive(!active);
}

void PrintDialog::lines_radio_toggled_cb() {
  bool active = lines_radio.get_active();
  lines_from.set_sensitive(active);
  lines_to.set_sensitive(active);
}

void PrintDialog::browse_button_clicked_cb() {
  std::string file = SimpleFileDialog::get_file(_("Please select a file to print to."), FILE_OPEN, _conf);
  if (file.size() > 0) {
    entry.set_text(file);
  }
}

void PrintDialog::print_menu_changed_cb() {
  int x = print_menu.get_active_row_number();
  assert (x < 2);
  std::string str = entry.get_text();
  if (x == 1) { // PostScript.
    if (str.size()) {
      _pdf = str;
    }
    entry.set_text(_ps);
  }
  else { // PDF
    if (str.size()) {
      _ps = str;
    }
    entry.set_text(_pdf);
  }
}

void PrintDialog::selection_signal_changed_cb() {
  Gtk::TreeModel::iterator iter = t_selection->get_selected();
  if (iter) {
    Gtk::TreeModel::Row row = *iter;
    int n = row[id];
    reset_paper_size(n);
  }
}

bool PrintDialog::init_backend() {
  // We need to set the margins, width and height.
  // if it's a custom printer then we'll use 36 as a margin for now - TODO
  Gtk::TreeModel::iterator iter = t_selection->get_selected();
  if (iter) {
    Gtk::TreeModel::Row row = *iter;
    int n = row[id];

    int a = paper_size.get_active_row_number();

    int p = orientation.get_active_row_number();
    assert (p < 2);

    int w, h;

    if (n == -1) {
      backend.set_margins(36, 36, 36, 36);
      Paper _p = Papers[a];
      w = _p.width;
      h = _p.height;
    }
    else {
      backend.set_printer(n);
      w = backend.get_width(n, a);
      h = backend.get_height(n, a);
      backend.set_margins(backend.get_top(n, a),
			  backend.get_left(n, a),
			  backend.get_bottom(n, a),
			  backend.get_right(n, a));
    }
    backend.set_dimensions(w, h);

    backend.set_orientation(p);
    if (to_file.get_active()) {
      backend.set_type(-1);
      std::string f = entry.get_text();
      backend.set_file(f);
    }
    else {
      backend.set_type(print_menu.get_active_row_number());
    }
    int cp = copies_button.get_value_as_int();
    assert (cp > 0);
    backend.set_copies(cp);
    return true;
  }
  else {
    katoob_error(_("I couldn't get the selected printer."));
    return false;
  }
}

bool PrintDialog::range(int& x, int& y) {
  x = lines_from.get_value_as_int();
  y = lines_to.get_value_as_int();
  return lines_radio.get_active();
}

void PrintDialog::manipulate() {
  dialog.hide();
  if (is_preview()) {
    PreviewDialog preview(backend);
    if (!preview.ok()) {
      return;
    }
    if (preview.run()) {
      process();
    }
  }
  else {
    process();
  }
}

void PrintDialog::process() {
  // If it's a file, Then save it otherwise print it.

  int x = print_menu.get_active_row_number();
  assert (x < 2);
  std::string str = entry.get_text();
  assert(str.size() > 0);
  std::string file;
  if (x == 1) { // PostScript.
    file = backend.PS::get_name();
  }
  else { // PDF
    file = backend.PDF::get_name();
  }
  if (to_file.get_active()) {
    // We already have a PDF and a PS. We will just copy it.
    if (rename(file.c_str(), str.c_str())) {
      katoob_error(strerror(errno));
      return;
    }
  }
  else {
    std::string err;
    // TODO: Custom.
    // TODO: width, height, portrait || landscape, margins, options, ....
    if (!backend.print(file, err)) {
      katoob_error(err);
      return;
    }
  }
  // TODO: Save settings.
}

bool PrintDialog::ok(std::string& er) {
  return backend.ok(er);
}

void PrintDialog::start_process() {
  backend.start_process();
}

bool PrintDialog::end_process(std::string& er) {
  return backend.end_process(er);
}

void PrintDialog::add_line(std::string& l) {
  backend.add_line(l);
}

bool PrintDialog::all() {
  return all_radio.get_active();
}

bool PrintDialog::selection() {
  return selection_radio.get_active();
}

bool PrintDialog::is_preview() {
  return res == Gtk::RESPONSE_APPLY ? true : false;
}

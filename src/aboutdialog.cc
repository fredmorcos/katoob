/*
 * aboutdialog.cc
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

#include <config.h>
#include <sstream>
#include <iostream>
#include <gdkmm/pixbuf.h>
#include <glibmm/exception.h>
#include "aboutdialog.hh"
#include "utils.hh"
#include "macros.h"

AboutDialog::AboutDialog() {
  set_position(Gtk::WIN_POS_CENTER);

  try {
    Glib::RefPtr<Gdk::Pixbuf> icon = Gdk::Pixbuf::create_from_file
      (Utils::get_data_path("katoob.svg"));
    set_logo(icon);
  } catch (Glib::Exception& e) {
    std::cerr << "Error loading icon for about dialog: "
              << e.what() << std::endl;
  }

  std::vector<std::string> authors;
  std::vector<std::string> artists;
  std::stringstream license;

  license
    << _("Katoob is free software; you can redistribute it "
         "and/or modify it under the terms of the "
         "GNU General Public License as published by the "
         "Free Software Foundation; either version 2 of the "
         "License, or (at your option) any later version.")
    << std::endl << std::endl
    << _("Katoob is distributed in the hope that it will be "
         "useful, but WITHOUT ANY WARRANTY; without even the "
         "implied warranty of MERCHANTABILITY or FITNESS FOR "
         "A PARTICULAR PURPOSE. See the GNU General Public "
         "License for more details.")
    << std::endl << std::endl
    << _("You should have received a copy of the GNU General "
         "Public License along with this program; if not, "
         "write to the Free Software Foundation, Inc., 59 "
         "Temple Place, Suite 330, Boston, MA  02111-1307  USA")
    << std::endl << std::endl;

  set_name(_(PACKAGE));
  set_version(_(VERSION));
  set_website(URL);
  set_license(license.str());
  set_wrap_license(true);
  set_copyright
    (_("Copyright \xc2\xa9 2002, 2007 Mohammed Sameer.\n"
       "Copyright \xc2\xa9 2008, 2018 Frederic-Gerald Morcos.\n"));

  authors.push_back(_("Mohammed Sameer"));
  authors.push_back(_("Frederic-Gerald Morcos"));
  set_authors(authors);

  artists.push_back(_("Mohammed Sameer"));
  artists.push_back(_("Mostafa Hussein"));
  artists.push_back(_("Frederic-Gerald Morcos"));
  set_artists(artists);

  set_comments(_("A lightweight multilingual unicode and bidi aware "
                 "text editor"));
  set_translator_credits(_("translator-credits"));
}

AboutDialog::~AboutDialog() {
}

void AboutDialog::run() {
  AboutDialog dialog;
  dynamic_cast<Gtk::AboutDialog *>(&dialog)->run();
}

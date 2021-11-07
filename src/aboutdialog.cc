/*
 * aboutdialog.cc
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
#include "macros.h"
#include "utils.hh"
#include <gdkmm/pixbuf.h>
#include <glibmm.h>
#include <iostream>
#include <sstream>

/**
 * \brief constructor.
 */
AboutDialog::AboutDialog()
{
  std::vector<std::string> authors, artists;
  std::string iconPath = Glib::build_filename(APP_DATADIR, PACKAGE "-large.png");

#ifdef GLIBMM_EXCEPTIONS_ENABLED
  try {
    Glib::RefPtr<Gdk::Pixbuf> icon = Gdk::Pixbuf::create_from_file(iconPath);
    set_logo(icon);
  } catch (Glib::Exception &e) {
    std::cerr << "Error loading icon for about dialog: " << e.what() << std::endl;
  }
#else
  std::auto_ptr<Glib::Error> error;
  Glib::RefPtr<Gdk::Pixbuf> icon = Gdk::Pixbuf::create_from_file(iconPath, 150, -1, true, error);
  set_logo(icon);
#endif
  std::stringstream license;
  license << _("Katoob is free software; you can redistribute it and/or modify it under the "
               "terms of the GNU General Public License as published by the Free Software "
               "Foundation; either version 2 of the License, or (at your option) any later "
               "version.")
          << std::endl
          << std::endl
          << _("Katoob is distributed in the hope that it will be useful, but WITHOUT ANY "
               "WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS "
               "FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more "
               "details.")
          << std::endl
          << std::endl
          << _("You should have received a copy of the GNU General Public License along with "
               "This program; if not, write to the Free Software Foundation, Inc., 59 Temple "
               "Place, Suite 330, Boston, MA  02111-1307  USA")
          << std::endl
          << std::endl;
  set_license(license.str());
  set_wrap_license(true);

  set_name(_(PROJECT));
  set_version(VERSION " (" TAG ")");
  set_website(PROJECT_URL);
  set_website_label(PROJECT_URL_LABEL);

  std::stringstream copyright;
  copyright << _("Copyright \xc2\xa9 2008-2021 Fred Morcos.") << std::endl
            << _("Copyright \xc2\xa9 2002-2007 Mohammed Sameer.") << std::endl;
  set_copyright(copyright.str());

  authors.push_back(_("Fred Morcos"));
  authors.push_back(_("Mohammed Sameer"));
  set_authors(authors);

  artists.push_back(_("Fred Morcos"));
  artists.push_back(_("Mohammed Sameer"));
  artists.push_back(_("Mostafa Hussein"));
  set_artists(artists);

  set_comments(_(PROJECT_COMMENT));
  set_translator_credits(_("translator-credits"));
  set_position(Gtk::WIN_POS_CENTER);
}

/**
 * \brief destructor.
 */
AboutDialog::~AboutDialog()
{
}

/**
 * \brief create an AboutDialog and blocks until the dialog is closed.
 */
void AboutDialog::run()
{
  AboutDialog dialog;
  dynamic_cast<Gtk::AboutDialog *>(&dialog)->run();
}

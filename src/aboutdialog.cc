/*
 * aboutdialog.cc
 * This file is part of katoob
 *
 * Copyright (C) 2006, 2007 Mohammed Sameer
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

#include <sstream>
#include <gdkmm/pixbuf.h>
#include "aboutdialog.hh"
#include "utils.hh"
#include "macros.h"

/**
 * \brief constructor.
 */
AboutDialog::AboutDialog() {
  std::vector<std::string> authors, artists;
#ifdef GLIBMM_EXCEPTIONS_ENABLED
  try {
    Glib::RefPtr<Gdk::Pixbuf> icon = Gdk::Pixbuf::create_from_file(Utils::get_data_path("katoob.png"));
    set_logo(icon);
  }
  catch (...) {
  }
#else
  std::auto_ptr<Glib::Error> error;
  Glib::RefPtr<Gdk::Pixbuf> icon = Gdk::Pixbuf::create_from_file(Utils::get_data_path("katoob.png"), 150, -1, true, error);
    set_logo(icon);
#endif
  std::stringstream license;
  license << _("Katoob is free software; you can redistribute it and/or modify it under the "
	       "terms of the GNU General Public License as published by the Free Software "
	       "Foundation; either version 2 of the License, or (at your option) any later "
	       "version.")
	  << "\n\n"
	  << _("Katoob is distributed in the hope that it will be useful, but WITHOUT ANY "
	       "WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS "
	       "FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more "
	       "details.")
	  << "\n\n"
	  << _("You should have received a copy of the GNU General Public License along with "
	       "This program; if not, write to the Free Software Foundation, Inc., 59 Temple "
	       "Place, Suite 330, Boston, MA  02111-1307  USA")
	  << "\n\n";

  set_name(_("Katoob"));
  set_version(VERSION);
  set_website("http://www.foolab.org/projects/katoob");
  set_license(license.str());
  set_wrap_license(true);
  set_copyright(_("Copyright \xc2\xa9 2006, 2007 Mohammed Sameer."));

  authors.push_back(_("Mohammed Sameer"));
  set_authors(authors);

  artists.push_back(_("Mohammed Sameer"));
  artists.push_back(_("Mostafa Hussein"));
  set_artists(artists);

  set_comments(_("A lightweight multilingual unicode and bidi aware text editor"));
  set_translator_credits(_("translator-credits"));
  set_position(Gtk::WIN_POS_CENTER);
}

/**
 * \brief destructor.
 */
AboutDialog::~AboutDialog() {

}

/**
 * \brief create an AboutDialog and blocks until the dialog is closed.
 */
void AboutDialog::run() {
  AboutDialog dialog;
  dynamic_cast<Gtk::AboutDialog *>(&dialog)->run();
}

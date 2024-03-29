/*
 * sourcemanager.cc
 *
 * This file is part of Katoob.
 *
 * Copyright © 2008-2021 Fred Morcos <fm+Katoob@fredmorcos.com>
 * Copyright © 2002-2007 Mohammed Sameer <msameer@foolab.org>
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

#include "sourcemanager.hh"
#include <giomm.h>
#include <glibmm.h>
#include <gtksourceview/gtksourceview.h>
#include <iostream>

void SourceManager::init()
{
  manager = gtk_source_language_manager_get_default();
  if (manager == NULL) {
    return;
  }

  const gchar *const *ids = gtk_source_language_manager_get_language_ids(manager);
  while (*ids) {
    GtkSourceLanguage *lang = gtk_source_language_manager_get_language(manager, *ids);
    if (lang == NULL) {
      ++ids;
      continue;
    }
    if (gtk_source_language_get_hidden(lang)) {
      ++ids;
      continue;
    }
    std::string section = gtk_source_language_get_section(lang);
    std::string id = *ids;
    cats[section].push_back(id);
    ++ids;
  }
}

SourceCategory &SourceManager::get_categories()
{
  return cats;
}

// std::string SourceManager::get_name(unsigned x) {
//  return _languages[x]->get_name();
//}

const char *SourceManager::get_name(const std::string &id)
{
  return gtk_source_language_get_name(
      gtk_source_language_manager_get_language(manager, id.c_str()));
}

std::string SourceManager::get_language_for_file(const std::string &file)
{
  bool uncertain = false;
  Glib::ustring _mime = Gio::content_type_guess(file, NULL, 0, uncertain);

  if (uncertain) {
    return "";
  }

  for (SourceCategoryIter iter = cats.begin(); iter != cats.end(); iter++) {
    for (unsigned x = 0; x < iter->second.size(); x++) {
      const char *languageId = iter->second[x].c_str();
      GtkSourceLanguage *lang = gtk_source_language_manager_get_language(manager, languageId);
      gchar **mimes = gtk_source_language_get_mime_types(lang);

      if (!mimes) {
        continue;
      }

      gchar **m = mimes;

      while (*m) {
        if (!strcmp(*m, _mime.c_str())) {
          g_strfreev(mimes);
          return iter->second[x];
        }
        ++m;
      }

      g_strfreev(mimes);
    }
  }

  return "";
  /*
  GtkSourceLanguage *lang = NULL;
  for (unsigned x = 0; x < _languages.size(); x++) {
    gchar** mimes = gtk_source_language_get_mime_types(_languages[x]->gobj());
    gchar **m = mimes;
    while (*m) {
      if (!strcmp(_mime, *m)) {
        lang = _languages[x]->gobj();
        g_strfreev(mimes);
        break;
      }
      ++m;
    }
    g_strfreev(mimes);
  }
  // = gtk_source_language_manager_get_language_from_mime_type(manager, _mime);
  if (lang == NULL) {
    return -1;
  }

  const char *name = gtk_source_language_get_name(lang);
  //  g_object_unref(lang);

  for (unsigned x = 0; x < _languages.size(); x++) {
    if (!strcmp(name, _languages[x]->get_name().c_str())) {
      return x;
    }
  }

  return -1;
  */
}

GtkSourceLanguage *SourceManager::get_language(const std::string &id)
{
  return gtk_source_language_manager_get_language(manager, id.c_str());
}

GtkSourceLanguageManager *SourceManager::manager;
SourceCategory SourceManager::cats;

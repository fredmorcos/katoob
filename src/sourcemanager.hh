/*
 * sourcemanager.hh
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

#pragma once

#include <gtksourceview/gtksourcelanguage.h>
#include <gtksourceview/gtksourcelanguagemanager.h>
#include <map>
#include <string>
#include <vector>

typedef std::map<std::string, std::vector<std::string> > SourceCategory;
typedef SourceCategory::iterator SourceCategoryIter;

class SourceManager {
 public:
  static void init();
  static SourceCategory &get_categories();
  static GtkSourceLanguage *get_language(const std::string &);
  static std::string get_language_for_file(const std::string &);
  static const char *get_name(const std::string &);

 private:
  SourceManager();
  ~SourceManager();

  static GtkSourceLanguageManager *manager;
  static SourceCategory cats;
};

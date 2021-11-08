/*
 * encodings.hh
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

#include <glibmm/convert.h>
#include <glibmm/ustring.h>
#include <string>
#include <vector>

/**
 * \brief This structure represents an Encoding.
 */
struct Encoding {
  /** \brief The friendly name of the encoding (ex: Arabic (Windows). */
  std::string name;
  /** \brief The actual name of the encoding (ex: WINDOWS-1256). */
  std::string encoding;
};

/**
 * \brief This structure represents a language.
 */
struct Language {
  /** \brief The name of the language (ex: Arabic). */
  std::string name;
  /** \brief The encodings this language can be encoded in. */
  std::vector<Encoding *> children;
};

/**
 * \brief This class contains all of the encoding and languages we know and
 *        methods to convert between them.
 *
 * Each Encoding has a friendly Encoding::name to be shown to the user and another
 * Encoding::encoding that is known by the encoding conversion methods.
 * Each Language has a also a "friendly" Encoding::name and all the encodings
 * (Language::children) this Language can be encoded in.
 */
class Encodings {
 public:
  Encodings();
  ~Encodings();
  int languages();
  int languages(int);
  std::string &language(int);
  std::string &name(unsigned, unsigned);

  int get_by_charset(const std::string &);
  int get(const std::string &);
  const std::string &get_charset(unsigned);
  const std::string &name(unsigned);
  int default_save();
  int default_open();
  int size();
  const std::string &at(unsigned);
  bool utf8(const Glib::ustring &);
  int utf8();
  int convert_from(const Glib::ustring &, std::string &, int);
  int convert_to(const Glib::ustring &, std::string &, int);

 private:
  bool convert(const Glib::ustring &, std::string &, unsigned int, unsigned int, std::string &);
  /** \brief a vector holding all the encodings (Encoding) we know.  */
  std::vector<Encoding *> _encodings;
  /** \brief a vector holding all the languages (Language) we know. */
  std::vector<Language *> _languages;
  /** \brief our default save encoding. */
  int _default_save;
  /** \brief our default open encoding. */
  int _default_open;

 protected:
  friend class Conf;
  void default_save(unsigned);
  void default_open(unsigned);
};

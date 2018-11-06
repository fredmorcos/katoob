/*
 * encodings.cc
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

#include <iostream>
#include <cassert>
#include "encodings.hh"
#include "macros.h"

/**
 * \brief constructor
 */
Encodings::Encodings() {
  _default_save = 28;
  _default_open = 1;

  Language *lang;
  Encoding *enc;

  /* Arabic */
  lang = new Language;
  lang->name = "Arabic";
  _languages.push_back(lang);

  enc = new Encoding;
  enc->name = "ISO 8859-6 (Arabic)";
  enc->encoding = "ISO_8859-6";
  lang->children.push_back(enc);
  _encodings.push_back(enc);

  enc = new Encoding;
  enc->name = "WINDOWS-1256 (Arabic)";
  enc->encoding = "WINDOWS-1256";
  lang->children.push_back(enc);
  _encodings.push_back(enc);

  /* Baltic */
  lang = new Language;
  lang->name = "Baltic";
  _languages.push_back(lang);

  enc = new Encoding;
  enc->name = "ISO_8859-4 (Baltic)";
  enc->encoding = "ISO_8859-4";
  lang->children.push_back(enc);
  _encodings.push_back(enc);

  enc = new Encoding;
  enc->name = "ISO_8859-13 (Baltic)";
  enc->encoding = "ISO_8859-13";
  lang->children.push_back(enc);
  _encodings.push_back(enc);

  enc = new Encoding;
  enc->name = "WINDOWS-1257 (Baltic)";
  enc->encoding = "WINDOWS-1257";
  lang->children.push_back(enc);
  _encodings.push_back(enc);

  /* Central European */
  lang = new Language;
  lang->name = "Central European";
  _languages.push_back(lang);

  enc = new Encoding;
  enc->name = "ISO 8859-2 (Central European)";
  enc->encoding = "ISO_8859-2";
  lang->children.push_back(enc);
  _encodings.push_back(enc);

  enc = new Encoding;
  enc->name = "WINDOWS-1250 (Central European)";
  enc->encoding = "WINDOWS-1250";
  lang->children.push_back(enc);
  _encodings.push_back(enc);

  /* Cyrillic */
  lang = new Language;
  lang->name = "Cyrillic";
  _languages.push_back(lang);

  enc = new Encoding;
  enc->name = "ISO_8859-5 (Cyrillic)";
  enc->encoding = "ISO_8859-5";
  lang->children.push_back(enc);
  _encodings.push_back(enc);

  enc = new Encoding;
  enc->name = "WINDOWS-1251 (Cyrillic)";
  enc->encoding = "WINDOWS-1251";
  lang->children.push_back(enc);
  _encodings.push_back(enc);

  /* Greek */
  lang = new Language;
  lang->name = "Greek";
  _languages.push_back(lang);

  enc = new Encoding;
  enc->name = "ISO 8859-7 (Greek)";
  enc->encoding = "ISO_8859-7";
  lang->children.push_back(enc);
  _encodings.push_back(enc);

  enc = new Encoding;
  enc->name = "WINDOWS-1253 (Greek)";
  enc->encoding = "WINDOWS-1253";
  lang->children.push_back(enc);
  _encodings.push_back(enc);

  /* Hebrew */
  lang = new Language;
  lang->name = "Hebrew";
  _languages.push_back(lang);

  enc = new Encoding;
  enc->name = "ISO 8859-8-i (Hebrew - logical ordering)";
  enc->encoding = "ISO_8859-8";
  lang->children.push_back(enc);
  _encodings.push_back(enc);

  enc = new Encoding;
  enc->name = "WINDOWS-1255 (Hebrew)";
  enc->encoding = "WINDOWS-1255";
  lang->children.push_back(enc);
  _encodings.push_back(enc);

  /* Japanese */
  lang = new Language;
  lang->name = "Japanese";
  _languages.push_back(lang);

  enc = new Encoding;
  enc->name = "EUC-JP (Japanese)";
  enc->encoding = "EUC-JP";
  lang->children.push_back(enc);
  _encodings.push_back(enc);

  enc = new Encoding;
  enc->name = "SHIFT_JIS (Japanese)";
  enc->encoding = "SHIFT_JIS";
  lang->children.push_back(enc);
  _encodings.push_back(enc);

  enc = new Encoding;
  enc->name = "ISO-2022-JP (Japanese)";
  enc->encoding = "ISO-2022-JP";
  lang->children.push_back(enc);
  _encodings.push_back(enc);

  /* Romanian */
  lang = new Language;
  lang->name = "Romanian";
  _languages.push_back(lang);

  enc = new Encoding;
  enc->name = "ISO_8859-16 (Romanian)";
  enc->encoding = "ISO-8859-16";
  lang->children.push_back(enc);
  _encodings.push_back(enc);

  /* Turkish */
  lang = new Language;
  lang->name = "Turkish";
  _languages.push_back(lang);

  enc = new Encoding;
  enc->name = "ISO 8859-9 (Turkish)";
  enc->encoding = "ISO_8859-9";
  lang->children.push_back(enc);
  _encodings.push_back(enc);

  enc = new Encoding;
  enc->name = "WINDOWS-1254 (Turkish)";
  enc->encoding = "WINDOWS-1254";
  lang->children.push_back(enc);
  _encodings.push_back(enc);

  /* Western */
  lang = new Language;
  lang->name = "Western";
  _languages.push_back(lang);

  enc = new Encoding;
  enc->name = "ISO 8859-1 (Western European)";
  enc->encoding = "ISO_8859-1";
  lang->children.push_back(enc);
  _encodings.push_back(enc);

  enc = new Encoding;
  enc->name = "ISO_8859-15 (Western, New)";
  enc->encoding = "ISO_8859-15";
  lang->children.push_back(enc);
  _encodings.push_back(enc);

  enc = new Encoding;
  enc->name = "WINDOWS-1252 (Western)";
  enc->encoding = "WINDOWS-1252";
  lang->children.push_back(enc);
  _encodings.push_back(enc);

  /* Other */
  lang = new Language;
  lang->name = "Other";
  _languages.push_back(lang);

  enc = new Encoding;
  enc->name = "ISO_8859-3 (South European)";
  enc->encoding = "ISO_8859-3";
  lang->children.push_back(enc);
  _encodings.push_back(enc);

  enc = new Encoding;
  enc->name = "ISO_8859-10 (Nordic)";
  enc->encoding = "ISO_8859-10";
  lang->children.push_back(enc);
  _encodings.push_back(enc);

  enc = new Encoding;
  enc->name = "ISO_8859-11";
  enc->encoding = "ISO_8859-11";
  lang->children.push_back(enc);
  _encodings.push_back(enc);

  enc = new Encoding;
  enc->name = "ISO_8859-12";
  enc->encoding = "ISO_8859-12";
  lang->children.push_back(enc);
  _encodings.push_back(enc);

  enc = new Encoding;
  enc->name = "ISO_8859-14";
  enc->encoding = "ISO_8859-14";
  lang->children.push_back(enc);
  _encodings.push_back(enc);

  enc = new Encoding;
  enc->name = "WINDOWS-1258 (Vietnamese)";
  enc->encoding = "WINDOWS-1258";
  lang->children.push_back(enc);
  _encodings.push_back(enc);

  enc = new Encoding;
  enc->name = "UTF-8";
  enc->encoding = "UTF-8";
  lang->children.push_back(enc);
  _encodings.push_back(enc);
}

/**
 * \brief destructor
 */
Encodings::~Encodings() {
  // TODO: Free all what we've allocated.
}

/**
 * \brief get the character set for an encoding given its position.
 * \param x position of the encoding.
 * \return the encoding itself (ex: WINDOWS-1256).
 */
const std::string& Encodings::get_charset(unsigned x) {
  assert(x <= _encodings.size());
  return _encodings[x]->encoding;
}

/**
 * \brief get the position of an encoding given its name.
 * \param enc the name of the encoding (ex: Arabic (Windows))
 * \return the position of the encoding.
 */
int Encodings::get(const std::string& enc) {
  for (unsigned int x = 0; x < _encodings.size(); x++) {
    if (_encodings[x]->name == enc) {
      return x;
    }
  }
  return -1;
}

/**
 * \brief get the position of an encoding given its character set.
 * \param enc the character set of the encoding (ex: WINDOWS-1256)
 * \return the position of the encoding.
 */
int Encodings::get_by_charset(const std::string& cset) {
  for (unsigned int x = 0; x < _encodings.size(); x++) {
    if (_encodings[x]->encoding == cset) {
      return x;
    }
  }
  return -1;
}

/**
 * \brief get the name of the encoding given its position.
 * \param x the position of the encoding.
 * \return the name of the encoding
 */
const std::string& Encodings::name(unsigned x) {
  assert(x <= _encodings.size());
  return _encodings[x]->name;
}

/**
 * \brief convert a string from one encoding to another.
 * \param text the string to convert
 * \param res the output result of the conversion
 * \param from the encoding position to convert from
 * \param to the encoding position to convert to
 * \param err a string to contain an error in case of a conversion error.
 * \return true if the conversion is correct, false otherwise. err is set to any error.
 */
bool Encodings::convert(const Glib::ustring& text, std::string& res, unsigned int from, unsigned int to, std::string& err) {
  assert(from <= _encodings.size());
  assert(to <= _encodings.size());
  assert(from != to);
#ifdef GLIBMM_EXCEPTIONS_ENABLED
  try {
    res = Glib::convert (text, get_charset(to), get_charset(from));
    if (utf8(res)) {
      return true;
    }
    else {
      err = _("I wasn't able to convert the encoding.");
      return false;
    }
  }
  catch (Glib::ConvertError& _err) {
    err = _err.what();
    return false;
  }
#else
  std::unique_ptr<Glib::Error> error;
  res = Glib::convert (text, get_charset(to), get_charset(from), error);
  if (error.get()) {
    err = error->what();
    return false;
  }

  if (utf8(res)) {
    return true;
  }
  else {
    err = _("I wasn't able to convert the encoding.");
    return false;
  }
#endif
}

/**
 * \brief convert to utf8 from an encoding.
 * \param in the string to convert.
 * \param out a string to contain the conversion output or the error in case of an error.
 * \param enc the encoding position to convert from.
 * \return -1 in case of error, out wil contain the error string. otherwise the encoding.
 */
int Encodings::convert_to(const Glib::ustring& in, std::string& out, int enc) {
  std::string err;
  if (in.validate()) {
    out = in;
    return utf8();
  }
  else {
    if (convert(in, out, enc, utf8(), err)) {
      return enc;
    }
    out = err;
    return -1;
  }
}

/**
 * \brief convert from utf8 to an encoding.
 * \param in the string to convert.
 * \param out a string to contain the conversion output or the error in case of an error.
 * \param enc the encoding position to convert to.
 * \return -1 in case of error, out wil contain the error string. otherwise the encoding.
 */
int Encodings::convert_from(const Glib::ustring& in, std::string& out, int enc) {
  if (enc == utf8()) {
    out = in;
    return enc;
  }

  std::string err;
  if (convert(in, out, utf8(), enc, err)) {
    return enc;
  }
  out = err;
  return -1;
}

/**
 * \brief check if a string contents are valid utf8
 * \param text the text to check.
 * \return true if the contents are valid utf8, false otherwise.
 */
bool Encodings::utf8(const Glib::ustring& text) {
  return text.validate();
}

/**
 * \brief get the number of languages we know.
 * \return the number of languages.
 */
int Encodings::languages() {
  return _languages.size();
}

/**
 * \brief get the number of encodings belonging to a language given its position.
 * \param x the position of the language.
 * \return the number of encodings.
 */
int Encodings::languages(int x) {
  return _languages[x]->children.size();
}

/**
 * \brief get the name of a language.
 * \param x the number of the language to query.
 * \return the name of the language (Ex: Arabic).
 */
std::string& Encodings::language(int x) {
  return _languages[x]->name;
}

/**
 * \brief get the name name of an encoding given the language and encoding positions.
 * \param x the position of the language
 * \param y the position of the encoding within the language children.
 * \return the name of the encoding.
 */
std::string& Encodings::name(unsigned x, unsigned y) {
  return _languages[x]->children[y]->name;
}

/**
 * \brief get default save encoding.
 * \return the number of the default save encoding.
 */
int Encodings::default_save() {
  return _default_save;
}

/**
 * \brief get default open encoding.
 * \return the number of the default open encoding.
 */
int Encodings::default_open() {
  return _default_open;
}

/**
 * \brief return the size (Number of encodings we know).
 * \return the number (size) of encodings.
 */
int Encodings::size() {
  return _encodings.size();
}

/**
 * \brief get the name of the encoding given its position (ex: Arabic (Windows)).
 * \param x the encoding position.
 * \return the name of the encoding.
 */
const std::string& Encodings::at(unsigned x) {
  return _encodings[x]->name;
}

/**
 * \brief return the utf8 encoding.
 * \return the number "position" of the utf8 encoding.
 */
int Encodings::utf8() {
  return 28;
}

/**
 * \brief set the default save encoding.
 * \param x the encoding.
 */
void Encodings::default_save(unsigned x) {
  if (x <= _encodings.size()) {
    _default_save = x;
  }
}

/**
 * \brief set the default open encoding.
 * \param x the encoding.
 */
void Encodings::default_open(unsigned x) {
  if (x <= _encodings.size()) {
    _default_open = x;
  }
}

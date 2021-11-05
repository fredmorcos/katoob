/*
 * undoredo.hh
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

#pragma once

#include <string>

typedef enum
{
  KATOOB_DO_INSERT,
  KATOOB_DO_DELETE
  /*  KATOOB_DO_ENCODING */
} KatoobDoType;

class KatoobDoElem {
 public:
  KatoobDoElem(KatoobDoType, const std::string &, int);
  ~KatoobDoElem();
  std::string &text();
  int pos();
  KatoobDoType &action();
  void toggle();

 private:
  std::string _text;
  int _pos;
  KatoobDoType _action;
};

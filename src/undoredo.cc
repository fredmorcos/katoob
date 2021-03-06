/*
 * undoredo.cc
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

#include "undoredo.hh"

KatoobDoElem::KatoobDoElem(KatoobDoType action, const std::string& t, int p) :
  _text(t),
  _pos(p),
  _action(action) {
}

KatoobDoElem::~KatoobDoElem() {
}

void KatoobDoElem::toggle() {
  _action = _action == KATOOB_DO_INSERT ? KATOOB_DO_DELETE : KATOOB_DO_INSERT;
}

std::string& KatoobDoElem::text() {
  return _text;
}

int KatoobDoElem::pos() {
  return _pos;
}

KatoobDoType& KatoobDoElem::action() {
  return _action;
}

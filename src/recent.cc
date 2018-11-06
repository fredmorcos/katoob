/*
 * recent.cc
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

#include "recent.hh"
#include <cassert>
#include "conf.hh"

Recent::Recent(Conf *conf) : _conf(conf) {
  _items = _conf->get_recent();
}

Recent::~Recent() {}

Recent *Recent::get() {
  assert(_recent != NULL);
  return _recent;
}

void Recent::init(Conf *conf) {
  _recent = new Recent(conf);
}

void Recent::destroy() {
  delete _recent;
  _recent = NULL;
}

std::string& Recent::operator[](unsigned idx) {
  return _items[idx];
}

unsigned Recent::size() {
  return _items.size();
}

void Recent::add_item(const std::string& item) {
  _conf->recent_prepend(item);
  _items = _conf->get_recent();
  signal_changed.emit();
}

Recent *Recent::_recent = NULL;

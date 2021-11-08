/*
 * redgreen.hh
 *
 * This file is part of Katoob.
 *
 * Copyright © 2008-2021 Fred Morcos <fm+Katoob@fredmorcos.com>
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

#include <gtkmm.h>

/**
 *  \brief Displays a red or green circle.
 *
 *  Used to represent a document's saved state, or the state of spell checking. Starts in
 *  green (yes) mode.
 *
 *  Yes signifies green and No signifies red.
 */
class RedGreen: public Gtk::Image {
 public:
  RedGreen();

  void setYes() noexcept;
  void setNo() noexcept;

 private:
  bool red;
};

/*
 * sourceview.hh
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

#pragma once

#include "sourcemanager.hh"
#include "textview.hh"
#include <gtksourceview/gtksource.h>

class SourceView : public TextView {
public:
  SourceView();
  virtual ~SourceView();
  void show_line_numbers(bool);

private:
  unsigned long __drag_data_received;
};

void _drag_data_received_cb(GtkWidget *, GdkDragContext *, gint, gint,
                            GtkSelectionData *, guint, guint, void *);

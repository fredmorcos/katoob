/*
 * sourceview.cc
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

#include "sourceview.hh"
//#include "gtkmm/private/widget_p.h"
#include <gtksourceview/gtksourcelanguagemanager.h>

void _drag_data_received_cb(GtkWidget *self, GdkDragContext *context, gint x, gint y, GtkSelectionData *selection_data, guint info, guint time, void *user_data) {
  static_cast<TextView *>(user_data)->on_drag_data_received(Glib::wrap(context, true), x, y, Glib::wrap(selection_data, true), info, time);
}

SourceView::SourceView() :
#ifdef ENABLE_HIGHLIGHT
  TextView(GTK_TEXT_VIEW(gtk_source_view_new())),
#endif
  __drag_data_received(0) {
  __drag_data_received = g_signal_connect(G_OBJECT(gobj()), "drag_data_received", G_CALLBACK(_drag_data_received_cb), this);
}

SourceView::~SourceView() {
  if (__drag_data_received) {
    g_signal_handler_disconnect(G_OBJECT(gobj()), __drag_data_received);
  }
}

void SourceView::show_line_numbers(bool show) {
  gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(gobj()), show ? TRUE : FALSE);
}
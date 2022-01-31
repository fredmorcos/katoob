/*
 * window.cc
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

#include "window.hh"
#include "config.h"
#include <giomm.h>
#include <glibmm.h>
#include <gtkmm.h>
#include <iostream>
#include <stdexcept>

Window::Window(BaseObjectType *cObject, Glib::RefPtr<Gtk::Builder> builder_):
 Gtk::ApplicationWindow(cObject),
 builder(std::move(builder_)),
 stack(builder->get_widget<Gtk::Stack>("stack")),
 menuButton(builder->get_widget<Gtk::MenuButton>("menuButton"))
{
  if (stack == nullptr) {
    throw std::runtime_error("No stack object found under " APP_PATH "/mainWindow.ui");
  }

  if (menuButton == nullptr) {
    throw std::runtime_error("No menuButton object found under " APP_PATH "/mainWindow.ui");
  }

  auto menuBuilder = Gtk::Builder::create_from_resource(APP_PATH "/mainWindowMenu.ui");
  auto menuModel = menuBuilder->get_object<Gio::MenuModel>("menuModel");

  if (!menuModel) {
    throw std::runtime_error("No menuModel object found under " APP_PATH "/mainWindowMenu.ui");
  }

  menuButton->set_menu_model(menuModel);
}

auto Window::create() -> Window *
{
  auto builder = Gtk::Builder::create_from_resource(APP_PATH "/mainWindow.ui");
  auto *window = Gtk::Builder::get_widget_derived<Window>(builder, "mainWindow");

  if (window == nullptr) {
    throw std::runtime_error("No mainWindow object found under " APP_PATH "/mainWindow.ui");
  }

  return window;
}

auto Window::openFile(const Glib::RefPtr<Gio::File> &file) -> void
{
  std::cout << "Opening file " << file->get_parse_name() << std::endl;

  Glib::ustring uContents;

  try {
    gchar *contents = nullptr;
    gsize length = 0;
    file->load_contents(contents, length);
    uContents.assign(contents);
    g_free(contents);
  } catch (const Glib::Error &ex) {
    std::cerr << ex.what() << std::endl;
    return;
  }

  const Glib::ustring title = file->get_basename();

  auto *scrolled = Gtk::make_managed<Gtk::ScrolledWindow>();
  scrolled->set_expand(true);

  auto *textView = Gtk::make_managed<Gtk::TextView>();
  textView->set_editable(false);
  textView->set_cursor_visible(false);
  textView->get_buffer()->set_text(uContents);

  scrolled->set_child(*textView);
  stack->add(*scrolled, title, title);
}

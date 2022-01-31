/*
 * application.cc
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

#include "application.hh"
#include "sigc++/functors/ptr_fun.h"
#include <config.h>
#include <exception>
#include <giomm.h>
#include <glibmm.h>
#include <gtkmm.h>
#include <sigc++/sigc++.h>

#include <iostream>

static const auto flags = Gio::Application::Flags::HANDLES_OPEN;
// | Gio::Application::Flags::HANDLES_COMMAND_LINE
// | Gio::Application::Flags::SEND_ENVIRONMENT;

Application::Application(): Gtk::Application(APP_ID, flags)
{
}

auto Application::create() -> Glib::RefPtr<Application>
{
  return Glib::make_refptr_for_instance<Application>(new Application());
}

auto Application::createMainWindow() -> Window *
{
  auto *mainWindow = Window::create();
  this->add_window(*mainWindow);

  auto onWindowHideCb = sigc::ptr_fun(&Application::onWindowHide);
  auto onWindowHideBind = sigc::bind(onWindowHideCb, mainWindow);
  mainWindow->signal_hide().connect(onWindowHideBind);

  return mainWindow;
}

auto Application::on_startup() -> void
{
  Gtk::Application::on_startup();

  add_action("preferences", sigc::mem_fun(*this, &Application::onActionPreferences));
  add_action("quit", sigc::mem_fun(*this, &Application::onActionQuit));
  set_accel_for_action("app.quit", "<Ctrl>Q");
}

auto Application::on_activate() -> void
{
  try {
    auto *mainWindow = this->createMainWindow();
    mainWindow->present();
  } catch (const Glib::Error &ex) {
    std::cerr << __PRETTY_FUNCTION__ << ": " << ex.what() << std::endl;
  } catch (const std::exception &ex) {
    std::cerr << __PRETTY_FUNCTION__ << ": " << ex.what() << std::endl;
  }
}

auto Application::on_open(const Gio::Application::type_vec_files &files,
                          const Glib::ustring & /* hint */) -> void
{
  Window *mainWindow = nullptr;
  auto currentWindows = this->get_windows();

  if (!currentWindows.empty()) {
    // The most recently focused window.
    mainWindow = dynamic_cast<Window *>(currentWindows[0]);
  } else {
    try {
      mainWindow = this->createMainWindow();
    } catch (const Glib::Error &ex) {
      std::cerr << __PRETTY_FUNCTION__ << ": " << ex.what() << std::endl;
    } catch (const std::exception &ex) {
      std::cerr << __PRETTY_FUNCTION__ << ": " << ex.what() << std::endl;
    }
  }

  for (const auto &file: files) {
    mainWindow->openFile(file);
  }

  mainWindow->present();
}

auto Application::on_command_line(
    const Glib::RefPtr<Gio::ApplicationCommandLine> & /* command_line */) -> int
{
  return 0;
}

auto Application::on_handle_local_options(const Glib::RefPtr<Glib::VariantDict> & /* options */)
    -> int
{
  return -1;
}

auto Application::onWindowHide(Gtk::Window *window) -> void
{
  delete window;
}

auto Application::onActionPreferences() -> void
{
}

auto Application::onActionQuit() -> void
{
  auto currentWindows = this->get_windows();

  for (auto *window: currentWindows) {
    window->hide();
  }
}

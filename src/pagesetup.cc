/*
 * pagesetup.cc
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

#include <config.h>

#include "pagesetup.hh"

Glib::RefPtr<PageSetup> PageSetup::create(Conf &conf)
{
  return Glib::RefPtr<PageSetup>(new PageSetup(conf));
}

PageSetup::PageSetup(Conf &conf): _conf(conf)
{
  _page_setup = Gtk::PageSetup::create();

  // If it's not custom then we will use the name and that's it.
  if (!_conf.print_get("paper_custom", false)) {
    _page_setup->set_paper_size(Gtk::PaperSize(_conf.print_get("paper_name", "")));
    return;
  }

  // PaperSize properties.
  _page_setup->set_paper_size(Gtk::PaperSize(
      _conf.print_get("paper_name", _page_setup->get_paper_size().get_name().c_str()),
      _conf.print_get("paper_display_name",
                      _page_setup->get_paper_size().get_display_name().c_str()),
      _conf.print_get("paper_width", _page_setup->get_paper_size().get_width(Gtk::UNIT_INCH)),
      _conf.print_get("paper_height", _page_setup->get_paper_size().get_height(Gtk::UNIT_INCH)),
      Gtk::UNIT_INCH));

  _page_setup->set_top_margin(
      _conf.print_get("margin_top", _page_setup->get_top_margin(Gtk::UNIT_INCH)),
      Gtk::UNIT_INCH);
  _page_setup->set_left_margin(
      _conf.print_get("margin_left", _page_setup->get_left_margin(Gtk::UNIT_INCH)),
      Gtk::UNIT_INCH);
  _page_setup->set_right_margin(
      _conf.print_get("margin_right", _page_setup->get_right_margin(Gtk::UNIT_INCH)),
      Gtk::UNIT_INCH);
  _page_setup->set_bottom_margin(
      _conf.print_get("margin_bottom", _page_setup->get_bottom_margin(Gtk::UNIT_INCH)),
      Gtk::UNIT_INCH);
  _page_setup->set_orientation(
      (Gtk::PageOrientation) _conf.print_get("orientation", _page_setup->get_orientation()));
}

void PageSetup::save()
{
  // PageSetup properties
  _conf.print_set("margin_top", _page_setup->get_top_margin(Gtk::UNIT_INCH));
  _conf.print_set("margin_left", _page_setup->get_left_margin(Gtk::UNIT_INCH));
  _conf.print_set("margin_bottom", _page_setup->get_bottom_margin(Gtk::UNIT_INCH));
  _conf.print_set("margin_right", _page_setup->get_right_margin(Gtk::UNIT_INCH));
  _conf.print_set("orientation", _page_setup->get_orientation());

  // Calculated properties
  //  _conf.print_set("page_width", get_page_width(Gtk::UNIT_INCH));
  //  _conf.print_set("page_height", get_page_height(Gtk::UNIT_INCH));

  // PaperSize properties.
  Gtk::PaperSize ps = _page_setup->get_paper_size();
  _conf.print_set("paper_display_name", ps.get_display_name().c_str());
  _conf.print_set("paper_width", ps.get_width(Gtk::UNIT_INCH));
  _conf.print_set("paper_height", ps.get_height(Gtk::UNIT_INCH));
  _conf.print_set("paper_name", ps.get_name().c_str());
  _conf.print_set("paper_custom", ps.is_custom());
}

PageSetup::~PageSetup()
{
  // We do not call it in the destructor otherwise we will crash!
  // save();
}

void PageSetup::reset(const Glib::RefPtr<Gtk::PageSetup> &setup)
{
  _page_setup = setup;
}

Glib::RefPtr<Gtk::PageSetup> PageSetup::get_page_setup()
{
  return _page_setup;
}

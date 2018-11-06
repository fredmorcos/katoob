/*
 * network.hh
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

#include <string>
#include <map>
#include <curl/curl.h>
#include <sigc++/slot.h>
#include <sigc++/connection.h>
#include "conf.hh"

class Network {
public:
  Network(Conf&);
  ~Network();
  static bool add_transfer(const std::string&,
                           std::string&,
                           sigc::slot<void, bool, const std::string&>,
                           void *get_handle = NULL);
  static bool network_perform();
  static void del_transfer(void *);
  static void destroy();

private:
  Network();
  Network(const Network&);
  Network& operator=(const Network&);

  static void clean_handle(void *);
  static CURLcode populate_proxy(CURL *,
                                 const std::string&,
                                 const int& port,
                                 const bool&,
                                 const std::string&, const std::string&);
  static size_t __curl_data_callback(void *ptr,
                                     size_t size,
                                     size_t nmemb,
                                     void  *stream);

  static Conf *conf;
  static std::map<CURL *, sigc::slot<void, bool, const std::string&>> cons;
  static std::map<CURL *, std::string> data;
  static CURLM *m_handle;
  static sigc::connection conn;
};

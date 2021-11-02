/*
 * network.cc
 * This file is part of katoob
 *
 * Copyright (C) 2002-2007 Mohammed Sameer
 * Copyright (C) 2008-2021 Frederic-Gerald Morcos
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

#include <glibmm/main.h>
#include "network.hh"
#include "macros.h"

class URL {
public:
  URL(const char *url): _url(url) {}

  ~URL() {
    if (_url) {
      g_free(const_cast<char *>(_url));
    }
  }

  const char *get() { return _url; }

  char *release() {
    char *url = const_cast<char *>(_url);
    _url = NULL;
    return url;
  }

private:
  URL(const URL&);
  URL& operator=(const URL&);
  const char *_url;
};

CURLcode Network::populate_proxy(CURL *handle,
                                 const std::string& host,
                                 const int& port,
                                 const bool& auth,
                                 const std::string& user,
                                 const std::string& pass)
{
  CURLcode code;
  code = curl_easy_setopt(handle, CURLOPT_PROXY, host.c_str());
  if (code != CURLE_OK) {
    return code;
  }

  code = curl_easy_setopt(handle, CURLOPT_PROXYPORT, port);
  if (code != CURLE_OK) {
    return code;
  }

  if (auth) {
    std::string str = user + ":" + pass;
    code = curl_easy_setopt(handle, CURLOPT_PROXYUSERPWD, str.c_str());
    if (code != CURLE_OK) {
      return code;
    }
    return CURLE_OK;
  }
  else {
    return CURLE_OK;
  }
}

bool Network::add_transfer(const std::string& uri,
                           std::string& error,
                           sigc::slot<void, bool, const std::string&> slot,
                           void *get_handle)
{
  if (!m_handle) {
    m_handle = curl_multi_init();
  }

  if (!m_handle) {
    error = _("Failed to initialize the networking backend.");
    return false;
  }

  CURL *handle = curl_easy_init();
  if (handle == NULL) {
    error = _("Couldn't initialize network handler.");
    return false;
  }

  CURLcode code;
  // todo: escape url and proxy ?
  std::string proxyuser = conf->get("proxyuser", "");
  std::string proxypass = conf->get("proxypass", "");
  bool proxyauth = conf->get("proxyauth", false);

  int proxyport = conf->get("proxyport", 8080);
  std::string proxyhost = conf->get("proxyhost", "");
  int proxytype = conf->get("proxytype", 0);
  switch (proxytype) {
  case 0: // None
    code = curl_easy_setopt(handle, CURLOPT_PROXY, "");
    if (code != CURLE_OK) {
      error = curl_easy_strerror(code);
      curl_easy_cleanup(handle);
      return false;
    }
    break;
  case 1: // "Get from the environment
    break;
  case 2: // HTTP
  case 3: // SOCKS4
  case 4: // SOCKS5
    if (proxyauth == true) {
      if (proxyuser.size() == 0) {
        error = _("Please set the proxy authentication username.");
        curl_easy_cleanup(handle);
        return false;
      }
      if (proxypass.size() == 0) {
        error = _("Please set the proxy authentication password.");
        curl_easy_cleanup(handle);
        return false;
      }
    }

    if (proxyhost == "") {
      error = _("Please set a proxy host.");
      curl_easy_cleanup(handle);
      return false;
    }

    code = populate_proxy(handle,
                          proxyhost,
                          proxyport,
                          proxyauth,
                          proxyuser,
                          proxypass);

    if (code != CURLE_OK) {
      error = curl_easy_strerror(code);
      curl_easy_cleanup(handle);
      return false;
    }

    code = curl_easy_setopt(handle,
                            CURLOPT_PROXYTYPE,
                            proxytype == 2 ? CURLPROXY_HTTP :
                            proxytype == 3 ? CURLPROXY_SOCKS4 :
                            CURLPROXY_SOCKS5);

    if (code != CURLE_OK) {
      error = curl_easy_strerror(code);
      curl_easy_cleanup(handle);
      return false;
    }

    break;

  default:
    error =
      "The proxy setting code is not in sync with the network code. "
      "Please inform the developer.";

    return false;
  }

  code = curl_easy_setopt(handle, CURLOPT_TIMEOUT, conf->get("timeout", 180));
  if (code != CURLE_OK) {
    error = curl_easy_strerror(code);
    curl_easy_cleanup(handle);
    return false;
  }

  code = curl_easy_setopt(handle, CURLOPT_HTTPPROXYTUNNEL, conf->get("proxytunnel", false));
  if (code != CURLE_OK) {
    error = curl_easy_strerror(code);
    curl_easy_cleanup(handle);
    return false;
  }

  code = curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, Network::__curl_data_callback);
  if (code != CURLE_OK) {
    error = curl_easy_strerror(code);
    curl_easy_cleanup(handle);
    return false;
  }

  URL url(g_strdup(uri.c_str()));

  code = curl_easy_setopt(handle, CURLOPT_URL, url.get());
  if (code != CURLE_OK) {
    error = curl_easy_strerror(code);
    curl_easy_cleanup(handle);
    return false;
  }

  code = curl_easy_setopt(handle, CURLOPT_PRIVATE, url.get()); // TODO: Hack
  if (code != CURLE_OK) {
    error = curl_easy_strerror(code);
    curl_easy_cleanup(handle);
    return false;
  }

  code = curl_easy_setopt(handle, CURLOPT_WRITEDATA, handle); // TODO: Hack
  if (code != CURLE_OK) {
    error = curl_easy_strerror(code);
    curl_easy_cleanup(handle);
    return false;
  }

  code = curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1);
  if (code != CURLE_OK) {
    error = curl_easy_strerror(code);
    curl_easy_cleanup(handle);
    return false;
  }

  //  curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, error);
  //  curl_easy_setopt(handle, CURLOPT_WRITEDATA, this); // TODO: hack

  CURLMcode c = curl_multi_add_handle(m_handle, handle);
  if (c != CURLM_OK) {
    //    curl_easy_cleanup(handle);
    error = curl_multi_strerror(c);
    curl_easy_cleanup(handle);
    return false;
  }

  cons[handle] = slot;
  data[handle] = "";

  if (!conn.connected()) {
    conn = Glib::signal_idle().connect(sigc::ptr_fun(Network::network_perform), G_PRIORITY_LOW);
  }
  else if (conn.blocked()) {
    conn.unblock();
  }

  if (get_handle) {
    get_handle = handle;
  }

  url.release();
  return true;
}

Network::Network(Conf& _conf) {
  conf = &_conf;
};

Network::~Network() {
  if (m_handle) {
    curl_multi_cleanup(m_handle);
  }
}

void Network::del_transfer(void *handle) {
  std::map<CURL *, sigc::slot<void, bool, const std::string&>>::iterator iter =
    cons.find(handle);

  std::map<CURL *, std::string>::iterator d_iter = data.find(handle);

  clean_handle(handle);
  cons.erase(iter);
  data.erase(d_iter);
}

void Network::clean_handle(void *handle) {
  char *url;
  CURLcode code = curl_easy_getinfo(handle, CURLINFO_PRIVATE, &url);

  if (code == CURLE_OK) {
    URL url2(url);
    // TODO: error
  }
  curl_multi_remove_handle(m_handle, handle);

  curl_easy_cleanup(handle);
  // TODO: error
}

bool Network::network_perform() {
  int running_handles;
  int msgs_in_queue;
  CURLMcode code;
  CURLMsg *msg;

  if (!m_handle) {
    return true;
  }

  while (true) {
    code = curl_multi_perform(m_handle, &running_handles);

    if (code == CURLM_CALL_MULTI_PERFORM) {
      continue;
    }
    else if (code == CURLM_OK) {
      break;
    }
    else {
      // TODO: Error.
    }
  }

  //  curl_multi_perform(m_handle, &running_handles);
  if (running_handles == 0) {
    conn.block();
  }

  if (running_handles < 0 || static_cast<size_t>(running_handles) < cons.size()) {
    while (true) {
      msg = curl_multi_info_read(m_handle, &msgs_in_queue);

      if (msg == NULL) {
        return true;
      } else {
        if (msg->msg == CURLMSG_DONE) {
          // done.
          bool good = msg->data.result == CURLE_OK;

          std::map<CURL *, sigc::slot<void, bool, const std::string&>>::iterator iter =
            cons.find(msg->easy_handle);
          std::map<CURL *, std::string>::iterator d_iter = data.find(msg->easy_handle);
          std::string er = curl_easy_strerror(msg->data.result);

          iter->second(good, good ? d_iter->second : er);
          clean_handle(msg->easy_handle);
          cons.erase(iter);
          data.erase(d_iter);
          //	  curl_easy_cleanup(msg->easy_handle);
        }
      }
    }
  }
  return true;
}

size_t Network::__curl_data_callback(void *ptr, size_t size, size_t nmemb, void  *stream) {
  // TODO: This is _really_ bad
  std::map<CURL *, std::string>::iterator iter = data.find(static_cast<CURL *>(stream));
  char *str = static_cast<char *>(ptr);
  for (size_t x = 0; x < nmemb*size; x++) {
    iter->second += str[x];
  }
  return nmemb*size;
}

void Network::destroy() {
  for (std::map<CURL *, std::string>::iterator iter = data.begin(); iter != data.end(); iter++) {
    del_transfer(iter->first);
  }
  if (m_handle) {
    curl_multi_cleanup(m_handle);
  }
}

/* Our static members */
std::map<CURL *, sigc::slot<void, bool, const std::string&>> Network::cons;
std::map<CURL *, std::string> Network::data;
//std::map<CURL *, sigc::signal<void, bool, const std::string&>> *Network::cons = NULL;
//std::map<CURL *, std::string> *Network::data = NULL;
Conf *Network::conf = NULL;
CURLM *Network::m_handle = NULL;
sigc::connection Network::conn;

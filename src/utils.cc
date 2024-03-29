/*
 * utils.cc
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

#include "glibmm/miscutils.h"
#include "macros.h"
#include "utils.hh"
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <glibmm.h>
#include <gtkmm.h>
#include <iostream>
#include <unistd.h>

std::string Utils::configDir()
{
  static const std::string dir = Glib::build_filename(Glib::get_user_config_dir(), PACKAGE);
  return dir;
}

std::string Utils::cacheDir()
{
  static const std::string dir = Glib::build_filename(Glib::get_user_cache_dir(), PACKAGE);
  return dir;
}

std::string Utils::recoveryDir()
{
  static const std::string dir = Glib::build_filename(Utils::cacheDir(), "recovery");
  return dir;
}

std::string Utils::get_data_path(const char *str)
{
  return Glib::build_filename(APP_DATADIR, str);
}

std::string Utils::get_dir_separator()
{
  return G_DIR_SEPARATOR_S;
}

std::string Utils::prepend_home_dir(char *str)
{
  return Glib::build_filename(Glib::get_home_dir(), str);
}

bool Utils::is_lam_alef(const Glib::ustring &src, Glib::ustring &res)
{
  res = src;
  bool ret = false;
  ret = is_lam_alef(res, static_cast<gunichar>(0x000fef7));
  ret = (is_lam_alef(res, static_cast<gunichar>(0x000fefb)) == false ? ret : true);
  ret = (is_lam_alef(res, static_cast<gunichar>(0x000fef5)) == false ? ret : true);
  ret = (is_lam_alef(res, static_cast<gunichar>(0x000fef9)) == false ? ret : true);

  return ret;
}

bool inline Utils::is_lam_alef(Glib::ustring &src, gunichar ch)
{
  gunichar repl;
  switch (ch) {
    case 0x000fef7:
      repl = 0x0623;
      break;
    case 0x000fefb:
      repl = 0x0627;
      break;
    case 0x000fef5:
      repl = 0x0622;
      break;
    case 0x000fef9:
      repl = 0x0625;
      break;
  }

  bool ret = false;
  Glib::ustring::size_type offset = src.find(ch);

  if (offset != Glib::ustring::npos) {
    ret = true;
  }

  while (offset != Glib::ustring::npos) {
    src.replace(offset, 1, 1, static_cast<gunichar>(0x0644));
    src.insert(offset + 1, 1, repl);
    offset = src.find(ch, offset);
  }

  return ret;
}

bool Utils::katoob_file_is_writable(std::string &file)
{
  if (Glib::file_test(file, Glib::FILE_TEST_EXISTS)) {
    return (access(file.c_str(), W_OK) == 0 ? true : false);
  } else {
    return true;
  }
}

void Utils::katoob_set_color(Conf &conf, Gtk::Label &label, KatoobColor c)
{
  int r = 0, g = 0, b = 0;

  switch (c) {
    case KATOOB_COLOR_READONLY: {
      r = conf.get("readonly_red", 0);
      g = conf.get("readonly_green", 0);
      b = conf.get("readonly_blue", 65535);
    } break;
    case KATOOB_COLOR_MODIFIED: {
      r = conf.get("modified_red", 65535);
      g = conf.get("modified_green", 0);
      b = conf.get("modified_blue", 0);
    } break;
    default: {
      r = conf.get("normal_red", 0);
      g = conf.get("normal_green", 0);
      b = conf.get("normal_blue", 0);
    } break;
  }

  Pango::AttrColor color(Pango::Attribute::create_attr_foreground(r, g, b));
  Pango::AttrList attrs;
  attrs.insert(color);
  label.set_attributes(attrs);
}

void Utils::katoob_set_color(Conf &cf, Gtk::Label *l, KatoobColor c)
{
  katoob_set_color(cf, *l, c);
}

bool Utils::katoob_write(Conf &conf, std::string &file, std::string &text, std::string &error)
{
  gchar *f = NULL;
  GError *er = NULL;

  if (Glib::file_test(file, Glib::FILE_TEST_IS_SYMLINK)) {
    f = g_file_read_link(file.c_str(), &er);
    if (!f) {
      error = er->message;
      g_error_free(er);
      return false;
    }
  }

  if (er) {
    g_error_free(er);
    er = NULL;
  }

  bool exists = Glib::file_test(f ? f : file.c_str(), Glib::FILE_TEST_EXISTS);
  struct stat buf;
  int stat_error = -1;

  if (exists) {
    stat_error = stat(f ? f : file.c_str(), &buf);
    if (stat_error) {
      std::cerr << std::strerror(errno) << std::endl;
    }
    // Let's take the backup first.
    if (conf.get("backup", true)) {
      std::string new_path = f ? f : file.c_str() + conf.get("backup_ext", "~");
      std::string err;
      if (!file_copy(f ? f : file.c_str(), new_path.c_str(), err)) {
        if (f) {
          g_free(f);
        }
        error = _("I can't backup the old file.\n");
        error += err;
        return false;
      }
      if (!stat_error) {
        katoob_set_perms(new_path.c_str(), buf);
      }
    }
  }

  if (er) {
    g_error_free(er);
    er = NULL;
  }

  if (katoob_write(f ? f : file.c_str(), text.c_str(), text.size(), error)) {
    if (!stat_error) {
      katoob_set_perms(f ? f : file.c_str(), buf);
    }
    if (f) {
      g_free(f);
    }
    return true;
  }

  if (f) {
    g_free(f);
  }
  return false;
}

// We are not using g_file_set_contents() because it'll create a file first
// and then copy it overwriting the target file. This will fail if we
// don't have write access to that directory although we have write access
// to the target file.
bool Utils::katoob_write(const char *file, const char *text, unsigned len, std::string &error)
{
  int fd = open(file, O_CREAT | O_TRUNC | O_WRONLY, 0644);
  if (fd == -1) {
    error = Utils::substitute("I can't create the file %s\n", file) + std::strerror(errno);
    return false;
  }

  ssize_t sz = write(fd, text, len);
  if (sz == -1) {
    error = Utils::substitute("I can't write to the file %s\n", file) + std::strerror(errno);
    close(fd);
    return false;
  }
  int x = close(fd);
  if (x == -1) {
    error = Utils::substitute("I can't close the file %s\n", file) + std::strerror(errno);
    return false;
  }
  return true;
}

bool Utils::katoob_read(const std::string &file, std::string &out)
{
  if (Glib::file_test(file, Glib::FILE_TEST_IS_DIR)) {
    out = substitute(_("%s is a directory."), file);
    return false;
  }

  try {
    out = Glib::file_get_contents(file);
    return true;
  } catch (Glib::FileError &err) {
    out = err.what();
    return false;
  }
}

void Utils::katoob_set_perms(const char *file, const struct stat &buff)
{
  chmod(file, buff.st_mode);
}

bool Utils::file_copy(const char *f1, const char *f2, std::string &error)
{
  int ifd, ofd;
  ifd = open(f1, O_RDONLY);
  if (ifd == -1) {
    error = std::strerror(errno);
    return false;
  }

  ofd = open(f2, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (ofd == -1) {
    error = std::strerror(errno);
    close(ifd);
    return false;
  }

  char buff[1024];
  ssize_t rfd, wfd;

  while (true) {
    rfd = read(ifd, buff, 1024);
    if (rfd == 0) {
      break;
    }
    if (rfd == -1) {
      error = std::strerror(errno);
      close(ifd);
      close(ofd);
      return false;
    }
    wfd = write(ofd, buff, rfd);
    if (wfd == -1) {
      error = std::strerror(errno);
      close(ifd);
      close(ofd);
      return false;
    }
  }
  close(ifd);
  close(ofd);
  return true;
}

std::string Utils::substitute(const std::string &orig, const int n)
{
  gchar *tmp_str = g_strdup_printf(orig.c_str(), n);
  std::string final_str(tmp_str);
  g_free(tmp_str);
  return final_str;
}

std::string Utils::substitute(const std::string &orig, const std::string &arg)
{
  gchar *tmp_str = g_strdup_printf(orig.c_str(), arg.c_str());
  std::string final_str(tmp_str);
  g_free(tmp_str);
  return final_str;
}

std::string Utils::substitute(const std::string &orig, const std::string &a, const std::string &b)
{
  gchar *tmp_str = g_strdup_printf(orig.c_str(), a.c_str(), b.c_str());
  std::string final_str(tmp_str);
  g_free(tmp_str);
  return final_str;
}

std::string Utils::substitute(const std::string &orig, const int a, const int b)
{
  gchar *tmp_str = g_strdup_printf(orig.c_str(), a, b);
  std::string final_str(tmp_str);
  g_free(tmp_str);
  return final_str;
}

std::string
Utils::substitute(const std::string &orig, const int a, const int b, const std::string &c)
{
  gchar *tmp_str = g_strdup_printf(orig.c_str(), a, b, c.c_str());
  std::string final_str(tmp_str);
  g_free(tmp_str);
  return final_str;
}

std::string Utils::substitute(const std::string &orig, const std::string &a, const int b)
{
  gchar *tmp_str = g_strdup_printf(orig.c_str(), a.c_str(), b);
  std::string final_str(tmp_str);
  g_free(tmp_str);
  return final_str;
}

std::string Utils::substitute(const std::string &orig,
                              const std::string &a,
                              const int b,
                              const std::string &c,
                              const std::string &d)
{
  gchar *tmp_str = g_strdup_printf(orig.c_str(), a.c_str(), b, c.c_str(), d.c_str());
  std::string final_str(tmp_str);
  g_free(tmp_str);
  return final_str;
}

std::string Utils::katoob_get_default_font()
{
  Gtk::TextView tv;
  tv.show();
  GtkStyle *s = gtk_rc_get_style(GTK_WIDGET(tv.gobj()));
  char *font = pango_font_description_to_string(s->font_desc);
  std::string _font(font);
  g_free(font);
  return _font;
}

std::vector<std::string> Utils::split(const std::string &src, const char n)
{
  std::vector<std::string> result;
  std::string str;

  for (unsigned x = 0; x < src.size(); x++) {
    if ((src[x] == n) && (str.size() > 0)) {
      result.push_back(str);
      str.clear();
    } else if (src[x] != n) {
      str += src[x];
    }
  }
  if (str.size() > 0) {
    result.push_back(str);
  }
  return result;
}

bool Utils::lock_file(int fd, std::string &error)
{
  struct flock lock;
  lock.l_type = F_WRLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = 0;

  if (fcntl(fd, F_SETLK, &lock) == -1) {
    error = std::strerror(errno);
    return false;
  }
  return true;
}

bool Utils::unlock_file(int fd, std::string &error)
{
  struct flock lock;
  lock.l_type = F_UNLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = 0;

  if (fcntl(fd, F_SETLK, &lock) == -1) {
    error = std::strerror(errno);
    return false;
  }
  return true;
}

bool Utils::file_is_locked(const std::string &file)
{
  int fd = ::open(file.c_str(), O_RDONLY);
  if (fd == -1) {
    std::cerr << "Failed to open " << file.c_str() << " " << std::strerror(errno) << std::endl;
    return true;
  }
  bool locked = file_is_locked(fd);

  if (::close(fd) == -1) {
    std::cerr << "Failed to close " << file.c_str() << " " << std::strerror(errno) << std::endl;
    return true;
  }
  return locked;
}

bool Utils::file_is_locked(int fd)
{
  struct flock lock;
  lock.l_type = F_WRLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = 0;
  lock.l_pid = 0;

  if (fcntl(fd, F_GETLK, &lock) == -1) {
    std::cerr << "Failed to check for locking: " << std::strerror(errno) << std::endl;
    return false;
  }
  return lock.l_pid != 0;
}

bool Utils::create_recovery_file(std::string &file, int &fd)
{
  file = Glib::build_filename(recoveryDir(), get_recovery_template());
  fd = Glib::mkstemp(file);
  return fd != -1;
}

std::string Utils::get_recovery_template(std::string suffix)
{
  return "katoob_autosave_" + suffix;
}

bool Utils::get_recovery_files(std::map<std::string, std::string> &files, std::string &error)
{
  try {
    Glib::Dir dir(recoveryDir());
    std::vector<std::string> entries(dir.begin(), dir.end());
    dir.close();
    for (unsigned x = 0; x < entries.size(); x++) {
      if (entries[x].find(get_recovery_template("")) == std::string::npos) {
        std::cerr << "Skipping " << entries[x] << std::endl;
        continue;
      }

      std::string file = Glib::build_filename(recoveryDir(), entries[x]);
      // is it free ?
      if (file_is_locked(file)) {
        std::cerr << "Skipping locked file " << file << std::endl;
        continue;
      }

      // Try to read it:
      try {
        std::string str = Glib::file_get_contents(file);
        if (str.size() == 0) {
          std::cerr << "Erasing zero sized file " << file << std::endl;
          unlink(file.c_str());
          continue;
        }
        files[file] = str;
      } catch (Glib::FileError &err) {
        std::cerr << "Failed to read " << file << ": " << err.what() << std::endl;
        continue;
      }
    }
    return true;
  } catch (Glib::FileError &err) {
    error = err.what();
    return false;
  }
}

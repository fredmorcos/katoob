/*
 * import.cc
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

#include <glibmm/miscutils.h>
#include "import.hh"
#include "macros.h"
#ifdef HAVE_GZIP
#include <zlib.h>
#endif
#ifdef HAVE_BZIP2
#include <bzlib.h>
#endif
#include <glibmm/fileutils.h>
#include <glibmm/ustring.h>
#include "utils.hh"
#include <fribidi/fribidi.h>
#include "shape_arabic.h"

void import_init(std::vector<Import>& import) {
  Import imp;

  imp.name = _("_HTML Character Reference...");
  imp.func = katoob_import_html;
  import.push_back(imp);

#ifdef HAVE_GZIP
  imp.name = _("Text file compressed with _gzip");
  imp.func = katoob_import_gz;
  import.push_back(imp);
#endif

#ifdef HAVE_BZIP2
  imp.name = _("Text file compressed with _bzip2");
  imp.func = katoob_import_bz2;
  import.push_back(imp);
#endif

  imp.name = _("Text with _shaping and bidi applied");
  imp.func = katoob_import_bidi_shape;
  import.push_back(imp);
}

bool katoob_import_html(std::string& file, std::string& _out) {
  Glib::ustring out;
  std::string contents;

  if (!Utils::katoob_read(file, contents)) {
    out = contents;
    return false;
  }

  std::string num;
  for (unsigned x = 0; x < contents.size(); x++) {
    if (x+1 < contents.size()) {
      if ((contents[x] == '&') && (contents[x+1] == '#')) {
	++x;
	++x;
	num.clear();
	while (x < contents.size()) {
	  if (contents[x] == ';') {
	    gunichar n = atoi(num.c_str());
	    out += n;
	    break;
	  }
	  else {
	    num += contents[x++];
	  }
	}
      }
      else {
	out += contents[x];
      }
    }
    else {
      out += contents[x];
    }
  }
  _out = out;
  return true;
}

#ifdef HAVE_GZIP
bool katoob_import_gz(std::string& file, std::string& out) {
  // Is it really a gzipped file ?
  FILE *fp = fopen(file.c_str(), "rb");
  if (!fp) {
    out = _("Can't open file.");
    return false;
  }

  int fd = fileno(fp);
  if (fd == -1) {
    out = _("Can't open file.");
    fclose(fp);
    return false;
  }

  unsigned char buff[2];
  if ((read(fd, &buff, 2) == 2) && (buff[0] == 0x1f) && (buff[1] == 0x8b)) {
    close(fd);
    fclose(fp);
    // Do nothing.
    ;
  }
  else {
    close(fd);
    fclose(fp);
    out = _("Error reading file.");
    return false;
  }

  gzFile zfile = gzopen(file.c_str(), "rb");
  if (zfile == NULL) {
    out = _("Can't open file.");
    return false;
  }

  int x;
  char c;
  while (true) {
    x = gzread(zfile, &c, 1);
    if (x == 1) {
      out += c;
    }
    else if (x == 0) {
      break;
    }
    else {
      // Error.
      out = _("Error reading file.");
      gzclose(zfile);
      return false;
    }
  }
  gzclose(zfile);
  return true;
}
#endif

#ifdef HAVE_BZIP2
bool katoob_import_bz2(std::string& file, std::string& out) {
  BZFILE *bzfile;
  bzfile = BZ2_bzopen(file.c_str(), "rb");
  if (bzfile == NULL) {
    out = _("Can't open file.");
    return false;
  }

  int x;
  char c;
  while (true) {
    x = BZ2_bzread(bzfile, &c, 1);
    if (x == 1) {
      out += c;
    }
    else if (x == 0) {
      break;
    }
    else {
      // Error.
      out = _("Error reading file.");
      BZ2_bzclose(bzfile);
      return false;
    }
  }
  BZ2_bzclose(bzfile);
  return true;
}
#endif

bool katoob_import_bidi_shape(std::string& file, std::string& out) {
  Glib::ustring contents;

  if (!Utils::katoob_read(file, out)) {
    return false;
  }

  contents = out;
  out.clear();

  // We need to feed fribidi  line only or it'll reverse the whole text string.
  gchar **tmp = g_strsplit(contents.c_str(), "\n", 0);
  std::vector<Glib::ustring> lines;
  for (gchar **t = tmp; *t; t++) {
    lines.push_back(*t);
  }

  g_strfreev(tmp);
  contents.clear();

  for (unsigned i = 0; i < lines.size(); i++) {
    gunichar *ch = new gunichar[lines[i].size()+1];
    for (unsigned x = 0; x < lines[i].size(); x++) {
      ch[x] = lines[i][x];
    }

    unsigned l = lines[i].size();

    unsigned length;

    // let's unshape.
    gunichar *_ch = unshape_arabic(ch, l, &length);
    delete [] ch;

    // Now we apply the bidi.
    FriBidiCharType base = FRIBIDI_TYPE_RTL;
    ch = new gunichar[length+1];
    if (!fribidi_log2vis(_ch, length, &base, ch, NULL, NULL, NULL)) {
      g_free(_ch);
      delete [] ch;
      out = _("Couldn't apply the bidi algorithm.");
      return false;
    }
    g_free(_ch);

    for (unsigned x = 0; x < length; x++) {
      contents += ch[x];
    }
    // TODO: We need to check whether the document has a \n at the end or not.
    contents += '\n';

    delete [] ch;
  }

  out = contents;

  return true;
}

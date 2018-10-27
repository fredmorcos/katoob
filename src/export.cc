/*
 * export.cc
 * This file is part of katoob
 *
 * Copyright (C) 2006, 2008 Mohammed Sameer
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

#include "export.hh"
#include "macros.h"
#include "utils.hh"
#ifdef HAVE_GZIP
#include <zlib.h>
#include "tempfile.hh"
#endif
#ifdef HAVE_BZIP2
#include <bzlib.h>
#endif
#ifdef HAVE_FRIBIDI
#include <fribidi/fribidi.h>
#include "shape_arabic.h"
#endif

// TODO: Export HTML character references.

void export_init(std::vector<Export *>& exprt) {
#ifdef HAVE_FRIBIDI
  exprt.push_back(new Export(_("_Text file with no unicode control characters..."), katoob_export_plain, false));
#endif

#ifdef HAVE_GZIP
  exprt.push_back(new Export(_("Text file compressed with _gzip..."), katoob_export_gz, false));
#endif

#ifdef HAVE_BZIP2
  exprt.push_back(new Export(_("Text file compressed with _bzip2..."), katoob_export_bz2, false));
#endif

#ifdef HAVE_FRIBIDI
  exprt.push_back(new Export(_("Text with _shaping and bidi applied..."), katoob_export_bidi_shape, true));
#endif
}

#ifdef HAVE_FRIBIDI
bool katoob_export_plain(Glib::ustring& text, std::string& out, std::string& error) {
  gunichar *ch = new gunichar[text.size()+1];
  for (unsigned i = 0; i < text.size(); i++) {
    ch[i] = text[i];
  }
  ch[text.size()] = 0x0;

  unsigned int len = fribidi_remove_bidi_marks(ch, text.size(), NULL, NULL, NULL);

  Glib::ustring _out;
  for (unsigned i = 0; i < len; i++) {
    _out += ch[i];
  }
  out = _out;

  delete [] ch;

  return true;
}
#endif

#ifdef HAVE_GZIP
bool katoob_export_gz(Glib::ustring& text, std::string& out, std::string& error) {
  TempFile tf;
  if (!tf.ok(error)) {
    return false;
  }

  gzFile zfile = gzopen(tf.get_name().c_str(), "wb");
  if (zfile == NULL) {
    error = _("Can't open temporary file.");
    return false;
  }

  int state = gzwrite(zfile, text.c_str(), text.bytes());
  if (state == 0) {
    error = _("Error writing to temporary file.");
    gzclose(zfile);
    return false;
  }
  gzclose(zfile);
  return tf.read(out, error);
}
#endif

#ifdef HAVE_BZIP2
bool katoob_export_bz2(Glib::ustring& text, std::string& out, std::string& error) {
  unsigned dest_len;

  // From the php bzip2 extension.
  dest_len = (unsigned )(text.bytes() + (0.01*text.bytes())+600);
  char *dest = (char *)malloc(dest_len + 1);
  if (!dest) {
    error = _("Couldn't allocate memory for compression.");
    return false;
  }

  int er = BZ2_bzBuffToBuffCompress(dest, &dest_len, const_cast<char *>(text.c_str()), text.bytes(), 4, 0, 0);
  if (er != BZ_OK) {
    error = _("Couldn't compress the text.");
    free(dest);
    return false;
  }
  for (unsigned x = 0; x < dest_len; x++) {
    out += dest[x];
  }

  free(dest);
  return true;
}
#endif

#ifdef HAVE_FRIBIDI
bool katoob_export_bidi_shape(Glib::ustring& text, std::string& out, std::string& error) {
  // We apply shaping first then bidi.
  // Memory representation  0 1 2 3
  // If we have a word like WaSaLaKa
  // After bidi It'll be KaLaSaWa
  // If we shape, The kaf will be treated as initial although it's final and waw will be
  // considered final although it's initial
  // So we shape first then we apply the bidi.

  Glib::ustring no_lam_alef;
  if (!Utils::is_lam_alef(text, no_lam_alef)) {
    no_lam_alef = text;
  }

  gunichar *ch = new gunichar[no_lam_alef.size()+1], *_ch;

  for (unsigned i = 0; i < no_lam_alef.size(); i++) {
    ch[i] = no_lam_alef[i];
  }
  ch[no_lam_alef.size()] = 0x0;

  // Now shaping.
  unsigned new_len;
  _ch = shape_arabic(ch, no_lam_alef.size(), &new_len);

  delete [] ch;

  ch = new gunichar[new_len+1];

  FriBidiCharType base = FRIBIDI_TYPE_RTL;

  // then bidi.
  if (!fribidi_log2vis(_ch, new_len, &base, ch, NULL, NULL, NULL)) {
    delete [] ch;
    g_free(_ch); // _ch is allocated via g_new();
    error = _("Couldn't apply the bidi algorithm.");
    return false;
  }
  g_free(_ch);

  Glib::ustring _out;
  for (unsigned i = 0; i < new_len; i++) {
    _out += ch[i];
  }
  out = _out;

  delete [] ch;

  return true;
}
#endif

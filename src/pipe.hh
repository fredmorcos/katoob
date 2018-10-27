/*
 * pipe.hh
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

#ifndef __PIPE_HH__
#define __PIPE_HH__

#include <string>

/**
 * \brief Use this class to execute any command and capture its output.
 */
class Pipe {
public:
  static bool exec(const std::string&, const std::string&, std::string&, std::string&);
private:
  Pipe(const std::string&);
  ~Pipe();
  bool exec(const std::string&, std::string&, std::string&);
  bool ok(std::string&);

  Pipe();
  Pipe(const Pipe&);
  Pipe& operator=(const Pipe&);
  bool _ok;
  std::string _err;
};

#endif /* __PIPE_HH__ */

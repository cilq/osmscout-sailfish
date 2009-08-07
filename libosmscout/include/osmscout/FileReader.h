#ifndef OSMSCOUT_FILEREADER_H
#define OSMSCOUT_FILEREADER_H

/*
  Import/TravelJinni - Openstreetmap offline viewer
  Copyright (C) 2009  Tim Teulings

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*/

#include <cstdio>
#include <string>

#include <osmscout/TypeConfig.h>

class FileReader
{
private:
  FILE   *file;
  char   *buffer;
  size_t size;
  size_t offset;
  bool   hasError;

public:
  FileReader();
  virtual ~FileReader();

  bool Open(const std::string& filename);
  bool Close();

  bool HasError() const;

  bool ReadFileToBuffer();

  bool Read(unsigned long& number);
  bool Read(unsigned int& number);

  bool ReadNumber(unsigned long& number);
  bool ReadNumber(unsigned int& number);
  bool ReadNumber(NodeCount& number);
};

#endif

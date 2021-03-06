/*
  OSMScout - a Qt backend for libosmscout and libosmscout-map
  Copyright (C) 2016 Lukas Karas

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "OnlineTileProvider.h"


OnlineTileProvider OnlineTileProvider::fromJson(QJsonValue val)
{
  if (!val.isObject())
    return OnlineTileProvider();
  
  QJsonObject obj = val.toObject();
  auto id = obj["id"];
  auto name = obj["name"];
  auto servers = obj["servers"];
  auto maximumZoomLevel = obj["maximumZoomLevel"];
  auto copyright = obj["copyright"];
  
  if (!(id.isString() && name.isString() && servers.isArray() && 
          maximumZoomLevel.isDouble() && copyright.isString())){
    return OnlineTileProvider();      
  }
  
  QStringList serverList;
  for (auto serverVal: servers.toArray()){
      if (serverVal.isString()){
          serverList.append(serverVal.toString());
      }
  }
  if (serverList.empty()){
    return OnlineTileProvider();
  }
  
  return OnlineTileProvider(id.toString(), name.toString(), serverList, 
          maximumZoomLevel.toDouble(), copyright.toString());
}

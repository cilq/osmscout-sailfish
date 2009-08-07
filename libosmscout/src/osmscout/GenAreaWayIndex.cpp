/*
  TravelJinni - Openstreetmap offline viewer
  Copyright (C) 2009  Tim Teulings

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

#include <osmscout/GenAreaWayIndex.h>

#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <set>

#include <osmscout/FileWriter.h>
#include <osmscout/Tiles.h>
#include <osmscout/Way.h>

bool GenerateAreaWayIndex(size_t wayIndexIntervalSize)
{
  /*
  std::cout << "Tile -180 -90   : " << GetTileId(-180.0,-90.0) << std::endl;
  std::cout << "Tile -180 -89.91: " << GetTileId(-180.0,-89.91) << std::endl;
  std::cout << "Tile -180 -89.9 : " << GetTileId(-180.0,-89.9) << std::endl;
  std::cout << "Tile -180 -89.89: " << GetTileId(-180.0,-89.89) << std::endl;
  std::cout << "Tile -180 -89.85: " << GetTileId(-180.0,-89.85) << std::endl;
  std::cout << "Tile -180 -89.8 : " << GetTileId(-180.0,-89.8) << std::endl;
  std::cout << "Tile -180  89.9 : " << GetTileId(-180.0,89.9) << std::endl;
  std::cout << "Tile -180  89.8 : " << GetTileId(-180.0,89.8) << std::endl;
  std::cout << "Tile -180  89.85: " << GetTileId(-180.0,89.85) << std::endl;
  std::cout << "Tile -180  89.89: " << GetTileId(-180.0,89.89) << std::endl;
  std::cout << "Tile -180  89.91: " << GetTileId(-180.0,89.91) << std::endl;
  std::cout << "Tile  180  90   : " << GetTileId( 180.0,90.0) << std::endl;*/

  //
  // Analysing ways regarding draw type and matching tiles.
  //

  std::cout << "Analysing distribution..." << std::endl;

  std::ifstream                                  in;
  std::vector<size_t>                            drawTypeDist;
  std::vector<std::map<TileId,NodeCount > >      drawTypeTileNodeCount;
  std::vector<std::map<TileId,std::set<Page> > > drawTypeTilePages;
  size_t                                         wayCount=0;

  //
  // * Go through the list of ways
  // * For every way detect the tiles it covers (rectangular min-max region over nodes)
  // * For every type/tile combnation get the node index pages (node.id/wayIndexIntervalSize)
  //   that hold the matching ways for this type/tile combination
  //

  in.open("ways.dat",std::ios::in|std::ios::out|std::ios::binary);
  std::ostream out(in.rdbuf());

  if (!in) {
    return false;
  }

  while (in) {
    Way way;

    way.Read(in);

    if (in) {
      wayCount++;

      double xmin,xmax,ymin,ymax;

      xmin=way.nodes[0].lon;
      xmax=way.nodes[0].lon;
      ymin=way.nodes[0].lat;
      ymax=way.nodes[0].lat;

      for (size_t i=1; i<way.nodes.size(); i++) {
        xmin=std::min(xmin,way.nodes[i].lon);
        xmax=std::max(xmax,way.nodes[i].lon);
        ymin=std::min(ymin,way.nodes[i].lat);
        ymax=std::max(ymax,way.nodes[i].lat);
      }

      // By Type

      if ((size_t)way.type>=drawTypeDist.size()) {
        drawTypeDist.resize(way.type+1,0);
        drawTypeTileNodeCount.resize(way.type+1);
        drawTypeTilePages.resize(way.type+1);
      }
      drawTypeDist[way.type]++;

      // By Type and tile

      for (size_t y=GetTileY(ymin); y<=GetTileY(ymax); y++) {
        for (size_t x=GetTileX(xmin); x<=GetTileX(xmax); x++) {
          std::map<size_t,NodeCount>::iterator tile=drawTypeTileNodeCount[way.type].find(GetTileId(x,y));

          if (tile!=drawTypeTileNodeCount[way.type].end()) {
            drawTypeTileNodeCount[way.type][GetTileId(x,y)]+=way.nodes.size();
          }
          else {
            drawTypeTileNodeCount[way.type][GetTileId(x,y)]=way.nodes.size();
          }

          drawTypeTilePages[way.type][GetTileId(x,y)].insert(way.id/wayIndexIntervalSize);
        }
      }
    }
  }

  in.close();

  std::cout << "Ways scanned: " << wayCount << std::endl;
  /*
  std::cout << "Distribution by type" << std::endl;
  for (size_t i=0; i<styleTypes.size(); i++) {
    if ((size_t)styleTypes[i].GetType()<drawTypeDist.size() && drawTypeDist[styleTypes[i].GetType()]>0) {
      std::cout << styleTypes[i].GetType() << ": " << drawTypeDist[styleTypes[i].GetType()] << std::endl;
    }
  }*/

  size_t drawTypeSum=0;
  size_t tileSum=0;
  size_t nodeSum=0;
  size_t pageSum=0;

  //std::cout << "Number of tiles per type" << std::endl;
  for (size_t i=0; i<drawTypeTilePages.size(); i++) {
    if (i!=typeIgnore && drawTypeTilePages[i].size()>0) {

      drawTypeSum++;

      //std::cout << styleTypes[i].GetType() << ": " << drawTypeTileDist[styleTypes[i].GetType()].size();
      tileSum+=drawTypeTilePages[i].size();

      for (std::map<size_t,std::set<Page> >::const_iterator tile=drawTypeTilePages[i].begin();
           tile!=drawTypeTilePages[i].end();
           ++tile) {
        pageSum+=tile->second.size();
      }

      for (std::map<size_t,NodeCount>::const_iterator tile=drawTypeTileNodeCount[i].begin();
           tile!=drawTypeTileNodeCount[i].end();
           ++tile) {
        nodeSum+=tile->second;
      }

      //std::cout << " " << ways << std::endl;
    }
  }

  std::cout << "Total number of draw types with tiles: " << drawTypeSum << std::endl;
  std::cout << "Total number of tiles: " << tileSum << std::endl;
  std::cout << "Total number of pages in tiles: " << pageSum << std::endl;
  std::cout << "Total number of nodes in tiles: " << nodeSum << std::endl;

  //
  // Writing index file (with blanks for way ids)
  //

  FileWriter writer;
  //std::ofstream indexFile;

  if (!writer.Open("areaway.idx")) {
    return false;
  }

  /*
  indexFile.open("areaway.idx",std::ios::out|std::ios::trunc|std::ios::binary);

  if (!indexFile) {
    return false;
  }*/

  // The number of draw types we have an index for
  writer.WriteNumber(drawTypeSum); // Number of entries
  //indexFile.write((const char*)&drawTypeSum,sizeof(drawTypeSum)); // Number of entries

  for (TypeId i=0; i<drawTypeTilePages.size(); i++) {
    size_t tiles=drawTypeTilePages[i].size();

    if (i!=typeIgnore && tiles>0) {
      writer.WriteNumber(i);     // The draw type id
      writer.WriteNumber(tiles); // The number of tiles
      /*
      indexFile.write((const char*)&i,sizeof(i)); // The draw type id
      indexFile.write((const char*)&tiles,sizeof(tiles)); // The number of tiles
        */

      for (std::map<TileId,std::set<Page> >::const_iterator tile=drawTypeTilePages[i].begin();
           tile!=drawTypeTilePages[i].end();
           ++tile) {
        //NodeCount nodeCount=drawTypeTileNodeCount[i][tile->first];
        //size_t    pageCount=tile->second.size();

        writer.WriteNumber(tile->first);                           // The tile id
        writer.WriteNumber(drawTypeTileNodeCount[i][tile->first]); // The number of nodes
        writer.WriteNumber(tile->second.size());                   // The number of pages

        /*
        indexFile.write((const char*)&tile->first,sizeof(tile->first)); // The tile id
        indexFile.write((const char*)&nodeCount,sizeof(nodeCount));     // The number of nodes
        indexFile.write((const char*)&pageCount,sizeof(pageCount));     // The number of pages
          */

        for (std::set<Page>::const_iterator page=tile->second.begin();
             page!=tile->second.end();
             ++page) {
          Page p=*page;

          writer.WriteNumber(p); // The id of the page

          //indexFile.write((const char*)&p,sizeof(p)); // The id of the page
        }
      }
    }
  }

  return !writer.HasError() && writer.Close();

  //indexFile.close();

  return true;
}

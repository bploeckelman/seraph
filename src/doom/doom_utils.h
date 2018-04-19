#ifndef SERAPH_DOOM_UTILS_H
#define SERAPH_DOOM_UTILS_H

#include <stdint.h>

/*

 https://zdoom.org/wiki/WAD

 Map:
   Map Lumps Sentinel: { name: "E1M1", filePos: 67500, size: 0 }
   Map Lumps:
   [
     THINGS,      // list of things in the map, (x, y) coordinates, facing angle, etc...
     LINEDEFS,    // list of linedefs defined by starting, ending vertices, flags, type, tag, args, etc..
     SIDEDEFS,    // list of sidedefs linking to linedefs, holds side data
     VERTEXES,    // each vertex in the map - (x, y) pairs
     SEGS,        // segments that connect to form sub-sectors
     SSECTORS,    // sub-sectors
     NODES,       // node tree for rendering acceleration
     SECTORS,     // floor / ceil heights and textures, light value, tag, type
     REJECT,      // (obsolete) visibility data for ai acceleration
     BLOCKMAP,    // collision info
     (BEHAVIOR),  // first in Hexen
     (SCRIPTS)    // nice to include for modders
   ]
   Next Map Lumps Sentinel: { name: "E1M2", filePos: ..., size: 0 }
   Next Map Lumps: [ ... ]
   ...

*/

typedef struct WadHeader {
    char identification[4];
    int32_t numLumps;
    int32_t infoTableOffset;
} WadHeader;

typedef struct WadFileLump {
    int32_t filePos;
    int32_t size;
    char name[8]; // allowed: A-Z 0-9 []-_
} WadFileLump;

// TODO: is this needed for reading?
//typedef struct WadLumpInfo {
//    char name[8];
//    int handle;
//    int position;
//    int size;
//} WadLumpInfo;

typedef struct THING {
    int16_t x;
    int16_t y;
    uint16_t angle; // 0=East, 45=NE, 90=North 135=NW, 180=West, 225=SW, 270=South
    uint16_t type;
    uint16_t flags;
} THING;

typedef struct LINEDEF {
    int16_t startVertex;
    int16_t endVertex;
    int16_t flags;
    int16_t types;
    int16_t tag;
    int16_t rightSide;
    int16_t leftSide;
} LINEDEF;

typedef struct VERTEX {
    int16_t x;
    int16_t y;
} VERTEX;

typedef struct Map {
    char name[5];
    size_t numThings;
    size_t numLinedefs;
    size_t numVertexes;
    THING *things;
    LINEDEF *linedefs;
//    SIDEDEFS
    VERTEX *vertexes;
//    SEGS
//    SSECTORS
//    NODES
//    SECTORS
//    REJECT
//    BLOCKMAP
} Map;

//typedef struct MapLumps {
//    char mapName[5];
//    WadFileLump things;
//    WadFileLump linedefs;
////    WadFileLump sidedefs;
//    WadFileLump vertexes;
////    WadFileLump segs;
////    WadFileLump ssectors;
////    WadFileLump nodes;
////    WadFileLump sectors;
////    WadFileLump reject;
////    WadFileLump blockmap;
//} MapLumps;

Map *maps;
size_t numMaps;

void readWad(const char *wadFileName);
void destroyMaps();


#endif //SERAPH_DOOM_UTILS_H

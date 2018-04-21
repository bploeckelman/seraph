#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "doom_utils.h"

//
// Dynamic array helpers
//
maplumps_t *initMapLumps(int initialSize) {
    maplumps_t *maplumps = (maplumps_t *) calloc(1, sizeof(maplumps_t));
    maplumps->lumps = (filelump_t *) calloc((size_t) initialSize, sizeof(filelump_t));
    maplumps->count = 0;
    maplumps->capacity = initialSize;
    return maplumps;
}

void insertMapLump(maplumps_t *maplumps, filelump_t *lump) {
    if (maplumps->count == maplumps->capacity) {
        maplumps->capacity *= 2;
        maplumps->lumps = (filelump_t *) realloc(maplumps->lumps, maplumps->capacity * sizeof(filelump_t));
    }
    maplumps->lumps[maplumps->count] = *lump;
    maplumps->count++;
}

void freeMapLumps(maplumps_t *maplumps) {
    free(maplumps->lumps);
    maplumps->lumps = NULL;
    maplumps->count = 0;
    maplumps->capacity = 0;
}

//
// Loading helper functions
//
bool isLumpMapLabel(filelump_t *lump) {
    assert(lump != NULL);
    // ExMx
    bool isDoom1MapLabelLump = (lump->name[0] == 'E' && lump->name[2] == 'M'
                             && lump->name[1] >= '1' && lump->name[1] <= '9'
                             && lump->name[3] >= '1' && lump->name[3] <= '9');
    // MAPxx
    bool isDoom2MapLabelLump = (lump->name[0] == 'M' && lump->name[1] == 'A' && lump->name[2] == 'P'
                             && lump->name[3] >= '1' && lump->name[3] <= '9'
                             && lump->name[4] >= '1' && lump->name[4] <= '9');
    return isDoom1MapLabelLump || isDoom2MapLabelLump;
}

//
// Read the specified WAD to populate the mapLumps struct
//
void readWadMaps(const char *wadFileName, maplumps_t *mapLumps) {
    FILE *wadFile = fopen(wadFileName, "rb");
    assert(wadFile != NULL);
    {
        // Read and validate wadinfo
        wadinfo_t wadinfo;
        fread(&wadinfo, sizeof(wadinfo_t), 1, wadFile);

        if (strncmp(wadinfo.identification, "IWAD", 4) != 0
         && strncmp(wadinfo.identification, "PWAD", 4) != 0) {
            printf("\nInvalid WAD info: identification '%.*s'", 4, wadinfo.identification);
            fclose(wadFile);
            return;
        }

        printf("\n%s - %.*s, %d lumps, dictionary @ %x (%d bytes)\n",
               wadFileName, 4, wadinfo.identification, wadinfo.numLumps,
               wadinfo.infoTableOffset, wadinfo.infoTableOffset);

        // Read in lumps, storing map lumps
        fseek(wadFile, wadinfo.infoTableOffset, SEEK_SET);
        for (int i = 0; i < wadinfo.numLumps; ++i) {
            filelump_t lump;
            fread(&lump, sizeof(filelump_t), 1, wadFile);

            if (isLumpMapLabel(&lump)) {
                insertMapLump(mapLumps, &lump);
                printf("Map %4d: %8.*s, %5d bytes, offset @ 0x%x (%d bytes)\n",
                       i, 8, lump.name, lump.size, lump.filePos, lump.filePos);
            } else {
                // Non-map-label lump
                printf("Lump %4d: %8.*s, %5d bytes, offset @ 0x%x (%d bytes)\n",
                       i, 8, lump.name, lump.size, lump.filePos, lump.filePos);
            }
        }
        printf("\nLoaded %d map label lumps.\n", mapLumps->count);
    }
    fclose(wadFile);
}

//
// Read the specified WAD to load the map specified by mapLabel
//
void loadWadMap(const char *wadFileName, filelump_t *mapLabel, map_t *map) {
    assert(mapLabel != NULL && map != NULL);
    map->label = *mapLabel;

    FILE *wadFile = fopen(wadFileName, "rb");
    assert(wadFile != NULL);
    {
        // Read and validate wadinfo
        wadinfo_t wadinfo;
        fread(&wadinfo, sizeof(wadinfo_t), 1, wadFile);

        if (strncmp(wadinfo.identification, "IWAD", 4) != 0
         && strncmp(wadinfo.identification, "PWAD", 4) != 0) {
            printf("\nInvalid WAD info: identification '%.*s'", 4, wadinfo.identification);
            fclose(wadFile);
            return;
        }

        printf("\n%s - %.*s, %d lumps, dictionary @ %x (%d bytes)\n",
               wadFileName, 4, wadinfo.identification, wadinfo.numLumps,
               wadinfo.infoTableOffset, wadinfo.infoTableOffset);

        fseek(wadFile, wadinfo.infoTableOffset, SEEK_SET);
        fpos_t fpos;
        fgetpos(wadFile, &fpos);

        // Read in map lumps
        filelump_t lump;
        for (int i = 0; i < wadinfo.numLumps; ++i) {
            fread(&lump, sizeof(filelump_t), 1, wadFile);
            if (strncmp(lump.name, map->label.name, 8) == 0) {
                printf("Found map label lump: %.*s (expected %.*s)\n",
                       8, lump.name, 8, map->label.name);
                break;
            }
        }

        // ---- Things
        fread(&lump, sizeof(filelump_t), 1, wadFile);
        if (strncmp(lump.name, "THINGS", 8) != 0) {
            printf("Unexpected lump: '%8.*s' (expected 'THINGS'), %5d bytes, offset @ 0x%x (%d bytes)\n",
                   8, lump.name, lump.size, lump.filePos, lump.filePos);
        } else {
            fgetpos(wadFile, &fpos);
            fseek(wadFile, lump.filePos, SEEK_SET);
            {
                map->numThings = lump.size / sizeof(mapthing_t);
                map->things = (mapthing_t *) calloc((size_t) map->numThings, sizeof(mapthing_t));
                printf("Reading %d things... ", map->numThings);
                for (int i = 0; i < map->numThings; ++i) {
                    fread(&map->things[i], sizeof(mapthing_t), 1, wadFile);
                    printf("{pos: (%d, %d), angle: %d, type: 0x%04x, options: 0x%04x} ",
                           map->things[i].x, map->things[i].y, map->things[i].angle,
                           map->things[i].type, map->things[i].options);
                }
                printf("\n");
            }
            fsetpos(wadFile, &fpos);
        }

        // ---- LineDefs
        fread(&lump, sizeof(filelump_t), 1, wadFile);
        if (strncmp(lump.name, "LINEDEFS", 8) != 0) {
            printf("Unexpected lump: '%8.*s' (expected 'LINEDEFS'), %5d bytes, offset @ 0x%x (%d bytes)\n",
                   8, lump.name, lump.size, lump.filePos, lump.filePos);
        } else {
            fgetpos(wadFile, &fpos);
            fseek(wadFile, lump.filePos, SEEK_SET);
            {
                map->numLinedefs = lump.size / sizeof(linedef_t);
                map->linedefs = (linedef_t *) calloc((size_t) map->numLinedefs, sizeof(linedef_t));
                printf("Reading %d linedefs... ", map->numLinedefs);
                for (int i = 0; i < map->numLinedefs; ++i) {
                    fread(&map->linedefs[i], sizeof(linedef_t), 1, wadFile);
                    printf("{v1,2: (%d, %d), flags: 0x%08x, special: 0x%08x, tag: %3d, sideNum 0x%2x%2x} ",
                           map->linedefs[i].v1, map->linedefs[i].v2, map->linedefs[i].flags,
                           map->linedefs[i].special, map->linedefs[i].tag, map->linedefs[i].sideNum[0],
                           map->linedefs[i].sideNum[1]);
                }
                printf("\n");
            }
            fsetpos(wadFile, &fpos);
        }

        // ---- SideDefs
        fread(&lump, sizeof(filelump_t), 1, wadFile);
        if (strncmp(lump.name, "SIDEDEFS", 8) != 0) {
            printf("Unexpected lump: '%8.*s' (expected 'SIDEDEFS'), %5d bytes, offset @ 0x%x (%d bytes)\n",
                   8, lump.name, lump.size, lump.filePos, lump.filePos);
        } else {
            fgetpos(wadFile, &fpos);
            fseek(wadFile, lump.filePos, SEEK_SET);
            {
                map->numSidedefs = lump.size / sizeof(sidedef_t);
                map->sidedefs = (sidedef_t *) calloc((size_t) map->numSidedefs, sizeof(sidedef_t));
                printf("Reading %d sidedefs... ", map->numSidedefs);
                for (int i = 0; i < map->numLinedefs; ++i) {
                    fread(&map->sidedefs[i], sizeof(sidedef_t), 1, wadFile);
                    printf("{texoff: %d, rowoff: %d, toptex: %.*s, bottex: %.*s, midtex: %.*s, sector: %d} ",
                           map->sidedefs[i].textureOffset, map->sidedefs[i].rowOffset,
                           8, map->sidedefs[i].topTexture, 8, map->sidedefs[i].bottomTexture, 8,
                           map->sidedefs[i].midTexture,
                           map->sidedefs[i].sector);
                }
                printf("\n");
            }
            fsetpos(wadFile, &fpos);
        }

        // ---- Vertexes
        fread(&lump, sizeof(filelump_t), 1, wadFile);
        if (strncmp(lump.name, "VERTEXES", 8) != 0) {
            printf("Unexpected lump: '%8.*s' (expected 'VERTEXES'), %5d bytes, offset @ 0x%x (%d bytes)\n",
                   8, lump.name, lump.size, lump.filePos, lump.filePos);
        } else {
            fgetpos(wadFile, &fpos);
            fseek(wadFile, lump.filePos, SEEK_SET);
            {
                map->numVertexes = lump.size / sizeof(mapvertex_t);
                map->vertices = (mapvertex_t *) calloc((size_t) map->numVertexes, sizeof(mapvertex_t));
                printf("Reading %d vertices... ", map->numVertexes);
                for (int i = 0; i < map->numVertexes; ++i) {
                    fread(&map->vertices[i], sizeof(mapvertex_t), 1, wadFile);
                    printf("{pos: (%d,%d)} ", map->vertices[i].x, map->vertices[i].y);
                }
                printf("\n");
            }
            fsetpos(wadFile, &fpos);
        }

        // TODO: read other map lumps as needed
    }

    fclose(wadFile);
}

void freeMap(map_t *map) {
    if (map == NULL) return;
    free(map->vertices); map->numVertexes = 0;
    free(map->sidedefs); map->numSidedefs = 0;
    free(map->linedefs); map->numLinedefs = 0;
    free(map->things);   map->numThings   = 0;
    free(map);
}

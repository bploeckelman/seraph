#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "doom_utils.h"

Map *maps = NULL;
size_t numMaps = 0;

void readWad(const char *wadFileName) {
    FILE *wadFile = fopen(wadFileName, "rb");
    assert(wadFile != NULL);
    {
        // Read Header
        WadHeader header;
        fread(&header, sizeof(header), 1, wadFile);
        if (strncmp(header.identification, "IWAD", 4) != 0
         && strncmp(header.identification, "PWAD", 4) != 0) {
            printf("\nInvalid WAD identifier: %.*s", 4, header.identification);
            fclose(wadFile);
            return;
        }
        printf("\n%s - %.*s, %d lumps, dictionary @ %x (%d bytes)\n", wadFileName, 4, header.identification, header.numLumps, header.infoTableOffset, header.infoTableOffset);

        // Determine how many Maps there are
        numMaps = 0;
        fseek(wadFile, header.infoTableOffset, SEEK_SET);
        {
            for (int i = 0; i < header.numLumps; ++i) {
                WadFileLump lump;
                fread(&lump, sizeof(lump), 1, wadFile);
                // Silly way to check if this is a map lump, but *shrug*, not gonna pull in a regex lib for this
                if (lump.name[0] == 'E' && lump.name[1] >= '1' && lump.name[1] <= '9'
                 && lump.name[2] == 'M' && lump.name[3] >= '1' && lump.name[3] <= '9') {
                    numMaps++;
                }
                // TODO: Doom2 format is MAPxy rather than ExMy
            }
            maps = (Map *) calloc(numMaps, sizeof(Map *));
        }
        fseek(wadFile, header.infoTableOffset, SEEK_SET);

        // Read map lumps
        int currentMap = -1;
        for (int i = 0; i < header.numLumps; ++i) {
            // Read lump info
            WadFileLump lump;
            fread(&lump, sizeof(lump), 1, wadFile);

            // Check for Map lump -----------------------------------
            if (lump.name[0] == 'E' && lump.name[1] >= '1' && lump.name[1] <= '9'
             && lump.name[2] == 'M' && lump.name[3] >= '1' && lump.name[3] <= '9') {
                currentMap++;
                memset(&maps[currentMap], 0, sizeof(Map));
                strncpy(maps[currentMap].name, lump.name, 4);
                printf("\t[----- Map %s -----]\n", maps[currentMap].name);
            } else {
                // Debug out non-map lump info
                printf("Lump %4d: %8.*s, %5d bytes, offset @ 0x%x (%d bytes)\n",
                       i, 8, lump.name, lump.size, lump.filePos, lump.filePos);
            }

            // Read in THINGS ---------------------------------------
            if (strncmp(lump.name, "THINGS", 8) == 0) {
                maps[currentMap].numThings = lump.size / sizeof(THING);
                printf("\t%d things: ", (int) maps[currentMap].numThings);

                if (maps[currentMap].things == NULL) {
                    maps[currentMap].things = (THING *) calloc(maps[currentMap].numThings, sizeof(THING));
                }

                fpos_t previousFilePos;
                fgetpos(wadFile, &previousFilePos);
                {
                    fseek(wadFile, lump.filePos, SEEK_SET);
                    for (int j = 0; j < maps[currentMap].numThings; ++j) {
                        fread(&maps[currentMap].things[j], sizeof(THING), 1, wadFile);
                        printf("{pos: (%d, %d), angle: %d, type: 0x%08x, flags: 0x%08x} ",
                               maps[currentMap].things[j].x, maps[currentMap].things[j].y, maps[currentMap].things[j].angle,
                               maps[currentMap].things[j].type, maps[currentMap].things[j].flags);
                    }
                    printf("\n");
                }
                fsetpos(wadFile, &previousFilePos);
            }

            // Read in LINEDEFS -------------------------------------
            if (strncmp(lump.name, "LINEDEFS", 8) == 0) {
                maps[currentMap].numLinedefs = lump.size / sizeof(LINEDEF);
                printf("\t%d linedefs: ", (int) maps[currentMap].numLinedefs);

                if (maps[currentMap].linedefs == NULL) {
                    maps[currentMap].linedefs = (LINEDEF *) calloc(maps[currentMap].numLinedefs, sizeof(LINEDEF));
                }

                fpos_t previousFilePos;
                fgetpos(wadFile, &previousFilePos);
                {
                    fseek(wadFile, lump.filePos, SEEK_SET);
                    for (int j = 0; j < maps[currentMap].numLinedefs; ++j) {
                        fread(&maps[currentMap].linedefs[j], sizeof(LINEDEF), 1, wadFile);
                        printf("{se: (%d, %d), f: 0x%x, t: 0x%x, tag: %d, rl: (%d, %d)} ",
                               maps[currentMap].linedefs[j].startVertex, maps[currentMap].linedefs[j].endVertex,
                               maps[currentMap].linedefs[j].flags, maps[currentMap].linedefs[j].types, maps[currentMap].linedefs[j].tag,
                               maps[currentMap].linedefs[j].rightSide, maps[currentMap].linedefs[j].leftSide);
                    }
                    printf("\n");
                }
                fsetpos(wadFile, &previousFilePos);
            }

            // Read in VERTEXES -------------------------------------
            if (strncmp(lump.name, "VERTEXES", 8) == 0) {
                maps[currentMap].numVertexes = lump.size / sizeof(VERTEX);
                printf("\t%d vertices: ", (int) maps[currentMap].numVertexes);

                if (maps[currentMap].vertexes == NULL) {
                    maps[currentMap].vertexes = (VERTEX *) calloc(maps[currentMap].numVertexes, sizeof(VERTEX));
                }

                fpos_t previousFilePos;
                fgetpos(wadFile, &previousFilePos);
                {
                    fseek(wadFile, lump.filePos, SEEK_SET);
                    for (int j = 0; j < maps[currentMap].numVertexes; ++j) {
                        fread(&maps[currentMap].vertexes[j], sizeof(VERTEX), 1, wadFile);
                        printf("(%d, %d) ", maps[currentMap].vertexes[j].x, maps[currentMap].vertexes[j].y);
                    }
                    printf("\n");
                }
                fsetpos(wadFile, &previousFilePos);
            }
        }
    }
    fclose(wadFile);
}

void destroyMaps() {
    if (maps != NULL) {
//        for (int i = 0; i < numMaps; ++i) {
//            if (maps[i].vertexes != NULL) free(maps[i].vertexes);
//            if (maps[i].linedefs != NULL) free(maps[i].linedefs);
//            if (maps[i].things   != NULL) free(maps[i].things);
//        }
        free(maps);
    }
}

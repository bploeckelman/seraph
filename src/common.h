#ifndef SERAPH_COMMON_H
#define SERAPH_COMMON_H

#define MIN(x, y) (((x) < (y) ? (x) : (y)))
#define MAX(x, y) (((x) > (y) ? (x) : (y)))

char *readFileToString(const char *path);

#endif //SERAPH_COMMON_H

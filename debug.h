#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

class cCollisionAABBBox;

void print_box(FILE* stream, cCollisionAABBBox* pBox, const char *pName);

#define PRINT_VECTOR(vec) printf(#vec ": (%g, %g, %g)\n", vec.x, vec.y, vec.z)

#define PRINT_BOX(pBox) print_box(stdout, pBox, #pBox)

#define PRINT(fmt, expr) printf(#expr ": " fmt, (expr))

#define COUT(expr) cout << #expr << ": " << (expr) << "\n"

#endif
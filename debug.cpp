#include "debug.h"

#include <collisions/CCollisionAABBBox.h>

void print_box( FILE* stream, cCollisionAABBBox* pBox, const char *pName )
{
	//fprintf(stream, "%s_%p: { ", pName, pBox);
	fprintf(stream, "%s: { ", pName);
	fprintf(stream, "m_center: (%g, %g, %g), ", pBox->m_center.x, pBox->m_center.y, pBox->m_center.z);
	fprintf(stream, "m_extent: (%g, %g, %g) ", pBox->m_extent.x, pBox->m_extent.y, pBox->m_extent.z);
	fprintf(stream, "}\n");
}

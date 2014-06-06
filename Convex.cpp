#include "Convex.h"

#include <windows.h>
#include <gl/GL.h>
#include <gl/glut.h>

#include <fstream>

#ifdef USE_CHAI3D

Convex::Convex( void )
{

}

Convex::~Convex( void )
{

}

void Convex::addVertex( const cVertex& vert )
{
	this->m_vertices.push_back(vert);
}


void Convex::addTriangle( const iTriangle& tria )
{
	this->m_triangles.push_back(tria);
}

void Convex::clear()
{
	this->m_triangles.clear();
	this->m_vertices.clear();
}

void Convex::render()
{
	int i, numItems = m_triangles.size();
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // 更改多边形绘制形
	glBegin(GL_TRIANGLES); // 开始绘图
	for(i=0; i<numItems; i++)
	{
		// get pointers to vertices
		cVertex* v0 = &m_vertices[m_triangles[i][0]];
		cVertex* v1 = &m_vertices[m_triangles[i][1]];
		cVertex* v2 = &m_vertices[m_triangles[i][2]];

		// render vertex 0
		// glNormal3dv(&v0->m_normal.x);
		glColor4fv(v0->m_color.pColor());
		glVertex3dv(&v0->m_localPos.x);

		// render vertex 1
		// glNormal3dv(&v1->m_normal.x);
		glColor4fv(v1->m_color.pColor());
		glVertex3dv(&v1->m_localPos.x);

		// render vertex 2
		// glNormal3dv(&v2->m_normal.x);
		glColor4fv(v2->m_color.pColor());
		glVertex3dv(&v2->m_localPos.x);
	}
	glEnd();
	glPolygonMode(GL_FRONT, GL_FILL);

	glFlush();
	glutSwapBuffers();

	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		printf("ERROR: %s\n", gluErrorString(err));
	}
}

void Convex::serialize( const char* filename )
{
	FILE* fout = fopen(filename, "w");
	int vtNum = this->m_vertices.size();
	for (int i=0; i<vtNum; ++i)
	{
		fprintf(fout, "vt %g %g %g\n", );
	}
}

#else // USE_CHAI3D

Convex::Convex(void)
{
	m_numVertices = 0;
	m_numTriangles = 0;
}

Convex::~Convex(void)
{
}

int Convex::addVertex( const Point& vert )
{
	m_vertices.push_back(vert);
	return m_numVertices++;
}

int Convex::addTriangle( const Triangle& tria )
{
	m_triangles.push_back(tria);
	return m_numTriangles++;
}

void Convex::clear()
{
	m_vertices.clear();
	m_triangles.clear();
	m_numVertices = 0;
	m_numTriangles = 0;
}

void Convex::render()
{
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();

	glTranslatef(m_position.x, m_position.y, m_position.z);

	// glPolygonMode(GL_FRONT, GL_LINE); // 更改多边形绘制形
#if 0
	glBegin(GL_TRIANGLES); // 开始绘图
	for (int i=0; i<m_numTriangles; ++i)
	{
		int idx0 = m_triangles[i][0];
		int idx1 = m_triangles[i][1];
		int idx2 = m_triangles[i][2];

		glColor3fv((float*)&m_vertices[idx0]);
		glVertex3fv((float*)&m_vertices[idx0]);

		glColor3fv((float*)&m_vertices[idx1]);
		glVertex3fv((float*)&m_vertices[idx1]);

		glColor3fv((float*)&m_vertices[idx2]);
		glVertex3fv((float*)&m_vertices[idx2]);
	}
	glEnd(); // 结束绘图
#else
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, &m_vertices[0]);

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3, GL_FLOAT, 0, &m_vertices[0]);
	
#if 1
	glBegin(GL_TRIANGLES);
	for (int i=0; i<m_numTriangles; ++i)
	{
		glArrayElement(m_triangles[i][0]);
		glArrayElement(m_triangles[i][1]);
		glArrayElement(m_triangles[i][2]);
	}
	glEnd();
	// glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, &m_triangles[0]);
#endif

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
#endif
	// glPolygonMode(GL_FRONT, GL_FILL);

	glFlush();
	glutSwapBuffers();
}


void Convex::serialize( const char* filename )
{
	int vtNum = this->m_vertices.size();
	int faceN = this->m_triangles.size();

#if 1
	FILE* fout = NULL;
	fout = fopen(filename, "w");

	if(fout == NULL) 
	{
		fprintf(stderr, "file %s open failed!\n", filename);
		return ;
	}

	fprintf(fout, 
		"# serialized Convex data file.\n"
		"# It is simplest .obj file.\n"
		);
	fprintf(fout, "# number of vertices: %d\n", vtNum);
	fprintf(fout, "# number of triangles: %d\n\n", faceN);

	fprintf(fout, "# vertices:\n");
	for (int i=0; i<vtNum; ++i)
	{
		fprintf(fout, "v %g %g %g\n", m_vertices[i][0], m_vertices[i][1], m_vertices[i][2]);
	}

	fprintf(fout, "\n# faces:\n");
	for (int i=0; i<faceN; ++i)
	{
		fprintf(fout, "f %d %d %d\n", m_triangles[i][0]+1, m_triangles[i][1]+1, m_triangles[i][2]+1);
	}

	fclose(fout);
#endif
}


#endif // USE_CHAI3D
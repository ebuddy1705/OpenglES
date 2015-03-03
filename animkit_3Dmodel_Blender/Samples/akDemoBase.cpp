/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2010 Xavier T.

    Contributor(s): none yet.
-------------------------------------------------------------------------------
  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------
*/

#include <stdio.h>
#include <piper.h>
#include "akDemoBase.h"

#include "akEntity.h"
#include "akMesh.h"
#include "akSkeleton.h"
#include "akAnimationClip.h"

#include <Debug.h>

#include "btAlignedAllocator.h"
#ifdef OPENGL_ES_2_0
#include "Timer.h"
#include "stdio.h"
#include "piper.h"
float WindowHeightW = 640;
float WindowWidthW = 480;
#endif
#include <sstream>

akDemoBase::akDemoBase() : m_frame(0), m_time(0), m_fpsLastTime(0), m_stepLastTime(0),
    m_lastfps(0), m_canUseVbo(false), m_drawNormals(true), m_wireframe(false), m_textured(true),
    m_shaded(true), m_drawColor(true), m_useVbo(true), m_dualQuatUse(0), m_normalMethod(1),
    m_drawSkeleton(false), m_windowx(800), m_windowy(800)
{
	m_camera = (akCamera*) btAlignedAlloc(sizeof(akCamera), 16);
}

akDemoBase::~akDemoBase()
{
	unsigned int i;
	
	for( i=0; i<m_objects.size(); i++)
	{
		delete m_objects[i];
	}
	
	for( i=0; i<m_animations.size(); i++)
	{
		delete m_animations[i];
	}
	
	for( i=0; i<m_meshes.size(); i++)
	{
		delete m_meshes[i];
	}
	
	for( i=0; i<m_skeletons.size(); i++)
	{
		delete m_skeletons[i];
	}
	
	for( i=0; i<m_textures.size(); i++)
	{
		glDeleteTextures( 1, &m_textures[i] );
	}
	
	m_meshes.clear();
	m_skeletons.clear();
	m_animations.clear();
	m_objects.clear();
	m_textures.clear();

	btAlignedFree(m_camera);
}

void akDemoBase::start(void)
{
	init();
	
#ifndef OPENGL_ES_2_0
    if (GLEW_ARB_vertex_buffer_object)
#endif

    m_canUseVbo = true;
    m_meshCount = m_objects.size();
    m_subCount = 0;
    m_vertexCount = 0;
    m_triCount = 0;
    for(unsigned int i=0; i<m_objects.size(); i++)
    {
        akEntity* object = m_objects.at(i);
        object->init(m_canUseVbo, this);
		
        m_subCount += object->getMesh()->getNumSubMeshes();
        m_vertexCount += object->getMesh()->getVertexCount();
        m_triCount  += object->getMesh()->getTriangleCount();
    }
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
}

void akDemoBase::step(akScalar time)
{
	unsigned int i;
	for( i=0; i<m_objects.size(); i++)
	{		
		akEntity* object = m_objects.at(i);
		object->step(time, m_dualQuatUse, m_normalMethod);
	}
}

void akDemoBase::drawString(float x, float y, const char *s)
{
#ifndef OPENGL_ES_2_0
    const char *c;
	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, m_windowx, 0, m_windowy);
	glScalef(1, -1, 1);
	glTranslatef(0, -m_windowy, 0);
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glLoadIdentity();
	
	
	glRasterPos2f(x, y);
	
	for (c = s; *c != '\0'; c++) {
		if( *c != '\n' )
			glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *c);
		else
		{
			y += 12;
			glRasterPos2f(x, y);
		}
	}
	
	glPopMatrix();
	glEnable(GL_DEPTH_TEST);
	
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	
	glMatrixMode(GL_MODELVIEW);
#else
    if (y == 15) {
        // lower string, only once
        static bool print(true);
        if (print) {
            print = false;
            printf("%s", s);
        }
    } else {
        // upper one
        static char lastValueOfS[512];
        if (strcmp(s, lastValueOfS)) {
            strcpy(lastValueOfS, s);
            printf("%s", s);
        }
    }
#endif
}

void akDemoBase::reshapeCallback(int w, int h)
{
	m_windowx = w;
	m_windowy = h ? h : 1;
}

#ifdef OPENGL_ES_2_0

#define GLUT_ELAPSED_TIME 1
static unsigned int glutGet(unsigned int i)
{
    assert(i == GLUT_ELAPSED_TIME);
    // elapsed time since last simulation step. used when calling physics.stepSimulation
    static structTimer elapsedTimer;
    static bool init = false;
    if (!init) {
        StartTimer(&elapsedTimer);
        ResetTimer(&elapsedTimer);
        init = true;
    }
    return 1000 * elapsedTimeInS(&elapsedTimer);
}

#endif

void akDemoBase::idleCallback(void)
{
	//avoid steping time for the 1rst frame
	if(m_stepLastTime == 0.f)
	{
		m_stepLastTime = glutGet(GLUT_ELAPSED_TIME);
		m_fpsLastTime = m_stepLastTime;
	}
	
	m_time = glutGet(GLUT_ELAPSED_TIME);
	// fps
	m_frame++;
	if (m_time - m_fpsLastTime > 1000) {
		m_lastfps = m_frame*1000.0f/(m_time-m_fpsLastTime);
		m_fpsLastTime = m_time;
		m_frame = 0;
	}
	
	// step time
	akScalar dt;
	dt = (m_time - m_stepLastTime)/1000.f;
	m_stepLastTime = m_time;
	
	//avoid too big steps while debugging
	if(dt >= 0.2)
		dt = 0.016666;
	
    step(dt);
	render();
}

void akDemoBase::displayCallback(void)
{
	render();
}

void akDemoBase::keyboardCallback(unsigned char key,int x, int y)
{
//	printf("%d\n", key);
	switch(key)
	{
		case 'n':
		case 'N':
			m_drawNormals = m_drawNormals? false:true;
			break;
		case 't':
		case 'T':
			m_textured = m_textured? false:true;
			break;
		case 's':
		case 'S':
			m_shaded = m_shaded? false:true;
			break;
		case 'd':
		case 'D':
			m_dualQuatUse +=1;
            if(m_dualQuatUse>3) m_dualQuatUse = 0;
			break;
		case 'w':
		case 'W':
			m_wireframe = m_wireframe? false:true;
			break;
		case 'c':
		case 'C':
			m_drawColor = m_drawColor? false:true;
			break;
		case 'v':
		case 'V':
			m_useVbo = m_useVbo? false:true;
			break;
		case 'm':
		case 'M':
			m_normalMethod +=1;
			if(m_normalMethod>3) m_normalMethod = 0;
			break;
		case 'p':
		case 'P':
			m_drawSkeleton = m_drawSkeleton? false:true;
			break;
	}
}

void akDemoBase::addAnimation(const utHashedString& name, akAnimationClip* anim)
{    
    DBG("addAnimation: %s \n", name.str().c_str());
    m_animations.insert(name.hash(), anim);
}

void akDemoBase::addMesh(const utHashedString& name, akMesh *mesh)
{
    DBG("addMesh: %s \n", name.str().c_str());
	m_meshes.insert(name.hash(), mesh);
}

void akDemoBase::addSkeleton(const utHashedString &name, akSkeleton *skel)
{
    DBG("addSkeleton: %s \n", name.str().c_str());
	m_skeletons.insert(name.hash(), skel);
}

void akDemoBase::addEntity(const utHashedString &name, akEntity *ent)
{
    DBG("addEntity: %s \n", name.str().c_str());
	m_objects.insert(name.hash(), ent);
}

void akDemoBase::addTexture(const utHashedString &name, GLuint tex)
{
    DBG("addTexture: %s \n", name.str().c_str());
	m_textures.insert(name.hash(), tex);
}

akAnimationClip* akDemoBase::getAnimation(const utHashedString &name)
{
	UTsize pos = m_animations.find(name.hash());
	if(pos==UT_NPOS)
		return 0;
	return m_animations.at(pos);
}

akMesh * akDemoBase::getMesh(const utHashedString &name)
{
	UTsize pos = m_meshes.find(name.hash());
	if(pos==UT_NPOS)
		return 0;
	return m_meshes.at(pos);
}

akSkeleton * akDemoBase::getSkeleton(const utHashedString &name)
{
	UTsize pos = m_skeletons.find(name.hash());
	if(pos==UT_NPOS)
		return 0;
	return m_skeletons.at(pos);
}

akEntity * akDemoBase::getEntity(const utHashedString &name)
{
	UTsize pos = m_objects.find(name.hash());
	if(pos==UT_NPOS)
		return 0;
	return m_objects.at(pos);
}

GLuint akDemoBase::getTexture(const utHashedString &name)
{
	UTsize pos = m_textures.find(name.hash());
	if(pos==UT_NPOS)
		return 0;
	return m_textures.at(pos);
}

int akDemoBase::getFps(void)
{
	return m_lastfps;
}

#ifdef OPENGL_ES_2_0
/*static void MatrixConvert(const akMatrix4 &m, MATRIX &out)
{
    void *p = &(out.f);
    for (int i = 0; i < 4; i++) {
        akVector4 row = m.getCol(i);
        memcpy(p, &row, 4 * sizeof(float));
        p += 4*sizeof(float);
    }
}*/
#endif


void akDemoBase::render()
{
    Piper::instance()->initFrame();

    // Set the OpenGL projection matrix
    MATRIX	MyPerspMatrix;
	
    MatrixPerspectiveFovRH(MyPerspMatrix, m_camera->m_fov, f2vt((WindowWidthW / WindowHeightW)), m_camera->m_clipStart, m_camera->m_clipEnd, (WindowHeightW > WindowWidthW));
    Piper::instance()->setMatrix(MyPerspMatrix, Piper::PROJECTION);

    akMatrix4 cam_m = m_camera->m_transform.toMatrix();
    MATRIX camM((GLfloat*)&cam_m);
    
    MATRIX model1;
    MatrixInverse(model1, camM);
    Piper::instance()->setMatrix(model1, Piper::MODEL);    

			


	// objects
//    DBG("Number of Obj: %d \n", m_objects.size());
    bool shaded = m_shaded && (!m_wireframe);
	unsigned int i;
	for( i=0; i<m_objects.size(); i++)
	{
		m_objects.at(i)->draw(m_drawNormals, m_drawColor, m_textured, m_useVbo, shaded, m_drawSkeleton);
	}


	// Stats
	std::stringstream UIString;
	UIString << "FPS: " << getFps() << "\n\n";
	UIString << "Meshes: " << m_meshCount <<"\n";
	UIString << "Submeshes: " << m_subCount <<"\n";
	UIString << "Triangles: " << m_triCount <<"\n";
	UIString << "Vertices: " << m_vertexCount <<"\n";
	utString str = UIString.str();
    static int lastfps = getFps();
    if (getFps() != lastfps) {
        lastfps = getFps();
        printf("fps = %d", lastfps);
    }
#ifndef OPENGL_ES_2_0
    glColor3f(0.2f, 0.2f, 0.2f);
#endif
	drawString(10, 15, str.c_str());
	
	// Info text
	str.clear();
	
	str += "P: Togle draw pose (";
	str += (m_drawSkeleton? "ON":"OFF");
	str += ")\n";
	
	str += "W: Togle wireframe (";
	str += (m_wireframe? "ON":"OFF");
	str += ")\n";
	
	str += "S: Togle shading (";
	str += (m_shaded? "ON":"OFF");
	str += ")\n";
	
	str += "T: Togle textures (";
	str += (m_textured? "ON":"OFF");
	str += ")\n";
	
	str += "C: Togle vertex color (";
	str += (m_drawColor? "ON":"OFF");
	str += ")\n";
	
	str += "N: Togle draw normals (";
	str += (m_drawNormals? "ON":"OFF");
	str += ")\n";
	
	str += "D: Cycle dual quaternion skinning (";
	switch(m_dualQuatUse)
	{
	case 0:
		str += "File setting";
		break;
	case 1:
		str += "Matrix";
		break;
	case 2:
		str += "Dual Quat";
		break;
	case 3:
		str += "Dual Quat Antiposality";
		break;
	}
	str += ")\n";
	
	str += "M: Cycle normal skinning (";
	switch(m_normalMethod)
	{
	case 0:
		str += "No normal skinning";
		break;
	case 1:
		str += "No scaling";
		break;
	case 2:
		str += "Uniform scaling";
		break;
	case 3:
		str += "Full";
		break;
	}
	str += ")\n";
	
	if(m_canUseVbo)
	{
		str += "V: Togle VBO (";
		str += (m_useVbo? "ON":"OFF");
		str += ")\n";
	}
	drawString(10, m_windowy-100, str.c_str());
#ifndef OPENGL_ES_2_0
	glFlush();
	glutSwapBuffers();
#endif
}


#ifndef ESVERTEX_H
#define ESVERTEX_H

#include "esUtil.h"


//for glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
GLfloat vVertexStrip[]={
		/* Top Left Of The Quad (Front) */
		-1.0f,1.0f, 0.0f,
		/* Bottom Left Of The Quad (Front) */
		-1.0f,-1.0f, 0.0f,
		/* Top Right Of The Quad (Front) */
		1.0f,1.0f, 0.0f,
		/* Bottom Right Of The Quad (Front) */
		1.0f,-1.0f, 0.0f,
};


GLfloat vVertexCoordStrip[]={
		0.0f,0.0f,
		0.0f,1.0f,
		1.0f,0.0f,
        1.0f,1.0f
};




//for glDrawArrays(GL_TRIANGLE_FAN, 0, 9);
GLfloat vVertexFan[]={
        0.0f,0.0f, 0.0f,
        -1.0f,-1.0f, 0.0f,
        -1.0f,0.0f, 0.0f,
        -1.0f,1.0f, 0.0f,
        0.0f,1.0f, 0.0f,
        1.0f,1.0f, 0.0f,
        1.0f,0.0f, 0.0f,
        1.0f,-1.0f, 0.0f,
        0.0f,-1.0f, 0.0f,
    };


GLfloat vTexCoordFan[]={
        0.5f,0.5f,
        0.0f,1.0f,
        0.0f,0.5f,
        0.0f,0.0f,
        0.5f,0.0f,
        1.0f,0.0f,
        1.0f,0.5f,
        1.0f,1.0f,
        0.5f,1.0f,
    };
    
    



//for glDrawElements ( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices );
GLfloat vVertexTriangles[] = {  -1.0f,  1.0f, 0.0f,  // Position 0
                                -1.0f, -1.0f, 0.0f,  // Position 1
                                 1.0f, -1.0f, 0.0f,  // Position 2
                                 1.0f,  1.0f, 0.0f,  // Position 3

                              };

GLfloat vTexCoordTriangles[]={  0.0f,  0.0f,        // TexCoord 0
                                0.0f,  1.0f,        // TexCoord 1
                                1.0f,  1.0f,        // TexCoord 2
                                1.0f,  0.0f         // TexCoord 3
                            };

GLushort indices[] = { 0, 1, 2, 0, 2, 3 };


    
#endif //ESVERTEX_H

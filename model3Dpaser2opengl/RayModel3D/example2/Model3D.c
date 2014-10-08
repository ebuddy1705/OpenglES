/*
 * components.c
 *
 *  Created on: Jul 28, 2014
 *      Author: ninhld
 */


#include <stdlib.h>
#include <math.h>
#include "esUtil.h"
#include "centic.h"
#include "Model3D.h"



static myObj myobj = CUBE_OBJ;
//static myObj myobj = STARSHIP_OBJ;
//static myObj myobj = MUSHROOM_OBJ;



typedef struct
{
    GLfloat m[3][3];
} ESMatrix3;




typedef struct
{
   // Handle to a program object
   GLuint program;

   // Attribute locations
   GLint  aPosition;
   GLint  aNormal;
   GLint  aTexel;

   // Uniform
    //GLint  uProjectionMatrix;
    //GLint  uModelViewMatrix;
   GLint uMvpMatrix;
   GLint  uNormalMatrix;

   GLint  uDiffuse;
   GLint  uSpecular;
   GLint  uTexture;

   //Rotation
   GLfloat   angle;

   //Texture
   GLuint  texMapId;

   // MVP matrix
   ESMatrix  mvpMatrix;
   ESMatrix3 normalMatrix;

} UserData;


const float EPSILON = 0.00001f;


///////////////////////////////////////////////////////////////////////////////
// inverse 3x3 matrix
// If cannot find inverse, set identity matrix
///////////////////////////////////////////////////////////////////////////////
// The elements of the matrix are stored as column major order.
// | 0 2 |    | 0 3 6 |    |  0  4  8 12 |
// | 1 3 |    | 1 4 7 |    |  1  5  9 13 |
//            | 2 5 8 |    |  2  6 10 14 |
//                         |  3  7 11 15 |
/*

  m[0]= m[0][0]
  m[1]= m[1][0]
  m[2]= m[2][0]
  m[3]= m[0][1]
  m[4]= m[1][1]
  m[5]= m[2][1]
  m[6]= m[0][2]
  m[7]= m[1][2]
  m[8]= m[2][2]

*/

static ESMatrix3 processNormalMatrix(ESMatrix modelViewMatrix)
{
    ESMatrix3 normalMatrix;
    GLfloat m[3][3];


    //get matrix from modelView matrix
    m[0][0] = modelViewMatrix.m[0][0];
    m[0][1] = modelViewMatrix.m[0][1];
    m[0][2] = modelViewMatrix.m[0][2];

    m[1][0] = modelViewMatrix.m[1][0];
    m[1][1] = modelViewMatrix.m[1][1];
    m[1][2] = modelViewMatrix.m[1][2];

    m[2][0] = modelViewMatrix.m[3][0];
    m[2][1] = modelViewMatrix.m[3][1];
    m[2][2] = modelViewMatrix.m[3][2];


    //invert
    //Matrix3invert(normalMatrix.m);
    GLfloat determinant, invDeterminant;
    GLfloat tmp[9];

    tmp[0] = m[1][1]*m[2][2] - m[2][1]*m[1][2]; //m[4] * m[8] - m[5] * m[7];
    tmp[1] = m[2][0]*m[1][2] - m[1][0]*m[2][2]; //m[2] * m[7] - m[1] * m[8];
    tmp[2] = m[1][0]*m[2][1] - m[2][0]*m[1][1]; //m[1] * m[5] - m[2] * m[4];
    tmp[3] = m[2][1]*m[0][2] - m[0][1]*m[2][2]; //m[5] * m[6] - m[3] * m[8];
    tmp[4] = m[0][0]*m[2][2] - m[2][0]*m[0][2]; //m[0] * m[8] - m[2] * m[6];
    tmp[5] = m[2][0]*m[0][1] - m[0][0]*m[2][1]; //m[2] * m[3] - m[0] * m[5];
    tmp[6] = m[0][1]*m[1][2] - m[1][1]*m[0][2]; //m[3] * m[7] - m[4] * m[6];
    tmp[7] = m[1][0]*m[0][2] - m[0][0]*m[1][2]; //m[1] * m[6] - m[0] * m[7];
    tmp[8] = m[0][0]*m[1][1] - m[1][0]*m[0][1]; //m[0] * m[4] - m[1] * m[3];

    // check determinant if it is 0
    determinant = m[0][0] * tmp[0] + m[1][0] * tmp[3] + m[2][0] * tmp[6];
    if(fabs(determinant) <= EPSILON)
    {
        //return identity(); // cannot inverse, make it idenety matrix
        m[0][0] = 1;
        m[1][0] = 0;
        m[2][0] = 0;
        m[0][1] = 0;
        m[1][1] = 1;
        m[2][1] = 0;
        m[0][2] = 0;
        m[1][2] = 0;
        m[2][2] = 1;
        //return;
    }else{
		// divide by the determinant
		invDeterminant = 1.0f / determinant;
		m[0][0] = invDeterminant * tmp[0];
		m[1][0] = invDeterminant * tmp[1];
		m[2][0] = invDeterminant * tmp[2];
		m[0][1] = invDeterminant * tmp[3];
		m[1][1] = invDeterminant * tmp[4];
		m[2][1] = invDeterminant * tmp[5];
		m[0][2] = invDeterminant * tmp[6];
		m[1][2] = invDeterminant * tmp[7];
		m[2][2] = invDeterminant * tmp[8];


		//transpose
		//Matrix3transpose(normalMatrix.m);
		GLfloat tmp2;
		tmp2 = m[1][0];
		m[1][0] = m[0][1];
		m[0][1] = tmp2;

		tmp2 = m[2][0];
		m[2][0] = m[0][2];
		m[0][2] = tmp2;

		tmp2 = m[2][1];
		m[2][1] = m[1][2];
		m[1][2] = tmp2;
	}

    //normalMatrix.m = m;
    normalMatrix.m[0][0] = m[0][0];
    normalMatrix.m[1][0] = m[1][0];
    normalMatrix.m[2][0] = m[2][0];
    normalMatrix.m[0][1] = m[0][1];
    normalMatrix.m[1][1] = m[1][1];
    normalMatrix.m[2][1] = m[2][1];
    normalMatrix.m[0][2] = m[0][2];
    normalMatrix.m[1][2] = m[1][2];
    normalMatrix.m[2][2] = m[2][2];

    return normalMatrix;
}




///
// Load texture from disk
//
static GLuint LoadTexture ( char *fileName )
{
    int width,
    height;
    char hasAlpha =0;
    char *buffer=NULL;
    GLuint texId;

    if(strstr(fileName,"tga")!=NULL){
        buffer=esLoadTGA ( fileName, &width, &height );
    }else if(strstr(fileName,"bmp") !=NULL){
        Image *image1;
        // allocate space for texture we will use
        image1 = (Image *) malloc(sizeof(Image));

        if(esLoadBMP ( fileName, image1 ) == 0)
        {
            return 0;
        }
        width = image1->sizeX;
        height = image1->sizeY;
        buffer=image1->data;
    }else if(strstr(fileName,"png") !=NULL){
        buffer= esLoadPNG(fileName,&width, &height, &hasAlpha);
    } else if(strstr(fileName,"jpg") !=NULL){
#if 0
        texId = SOIL_load_OGL_texture(fileName,
                              SOIL_LOAD_AUTO,
                              SOIL_CREATE_NEW_ID,
                              SOIL_FLAG_POWER_OF_TWO |
                              SOIL_FLAG_MIPMAPS |
                              SOIL_FLAG_COMPRESS_TO_DXT);
        return texId;
#endif
    }


    if ( buffer == NULL )
    {
        esLogMessage ( "Error loading (%s) image.\n", fileName );
        return 0;
    }

    glGenTextures ( 1, &texId );
    glBindTexture ( GL_TEXTURE_2D, texId );
    glTexImage2D ( GL_TEXTURE_2D, 0, hasAlpha ? GL_RGBA : GL_RGB, width, height, 0,  hasAlpha ? GL_RGBA : GL_RGB , GL_UNSIGNED_BYTE, buffer);
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    free ( buffer );
    return texId;
}



///
// Initialize the shader and program object
//
static int Init ( ESContext *esContext )
{
   UserData *userData = esContext->userData;

   GLbyte vShaderStr[] =
           "attribute vec2 aTexel;                          \n"
           "attribute vec3 aPosition;                       \n"
           "attribute vec3 aNormal; \n"

           //"uniform mat4 uProjectionMatrix;                 \n"
           //"uniform mat4 uModelViewMatrix;                  \n"
           "uniform mat3 uNormalMatrix;                     \n"
           "uniform mat4 uMvpMatrix;                        \n"

           "varying vec3 vNormal;                           \n"
           "varying vec2 vTexel;                            \n"

           "void main()                                     \n"
           "{                                               \n"
               "vNormal = uNormalMatrix * aNormal;          \n"
               //"vNormal = aNormal;                          \n"
               "vTexel = aTexel;                            \n"
               //"gl_Position = uProjectionMatrix * uModelViewMatrix * vec4(aPosition, 1.0);  \n"
                "gl_Position = uMvpMatrix * vec4(aPosition, 1.0);  \n"
           "}                                               \n";


   GLbyte fShaderStr[] =
           "precision mediump float;                      \n"

           "varying highp vec3 vNormal;                   \n"
           "varying highp vec2 vTexel;                    \n"

           "uniform highp vec3 uDiffuse;                  \n"
           "uniform highp vec3 uSpecular;                 \n"
           "uniform sampler2D uTexture;                   \n"

           "void main()                                                                     \n"
           "{                                                                               \n"
               // Material
               "highp vec3 ka = vec3(0.05);                     \n"
               "highp vec3 kd = uDiffuse;                       \n"
               "highp vec3 ks = uSpecular;                      \n"
               "highp float alpha = 1.0;                        \n"

               //Light
               "highp vec3 ia = vec3(1.0);                      \n"
               "highp vec3 id = vec3(1.0);                      \n"
               "highp vec3 is = vec3(1.0);                      \n"

               //Vectors
               "highp vec3 L = normalize(vec3(1.0, 1.0, 1.0));  \n"
               "highp vec3 N = normalize(vNormal);              \n"
               "highp vec3 V = normalize(vec3(0.0, 0.0, 1.0));  \n"
               "highp vec3 R = reflect(L, N);                   \n"

                // Illumination factors
               "highp float df = max(0.0, dot(L, N));                  \n"
               "highp float sf = pow(max(0.0, dot(R, V)), alpha);      \n"

                // 3dmodel reflection equation
               "highp vec3 Ip = ka*ia + kd*id*df + ks*is*sf;           \n"

                //Decal
               "highp vec4 decal = texture2D(uTexture, vTexel);        \n"

                // Surface
               "highp vec3 surface;                                     \n"


               "if(decal.a > 0.0)                                       \n"
                    "surface = decal.rgb;                               \n"
               "else                \n"
                    "surface = Ip;                                      \n"

                "gl_FragColor = vec4(surface, 1.0);                     \n"
           "}                                                                               \n";


    // Load the shaders and get a linked program object
    userData->program = esLoadProgram ( (char *)vShaderStr, (char *)fShaderStr );

    // Attributes
    userData->aPosition = glGetAttribLocation(userData->program, "aPosition");
    userData->aNormal = glGetAttribLocation(userData->program, "aNormal");
    userData->aTexel = glGetAttribLocation(userData->program, "aTexel");

    // Uniforms
    //userData->uProjectionMatrix = glGetUniformLocation(userData->program, "uProjectionMatrix");
    //userData->uModelViewMatrix = glGetUniformLocation(userData->program, "uModelViewMatrix");
    userData->uMvpMatrix = glGetUniformLocation(userData->program, "uMvpMatrix");
    userData->uNormalMatrix = glGetUniformLocation(userData->program, "uNormalMatrix");
    userData->uDiffuse = glGetUniformLocation(userData->program, "uDiffuse");
    userData->uSpecular = glGetUniformLocation(userData->program, "uSpecular");
    userData->uTexture = glGetUniformLocation(userData->program, "uTexture");

    userData->angle = 0.0f;

    // Load the textures
    switch (myobj) {
        case CUBE_OBJ:
            userData->texMapId = LoadTexture("/carmeter/src/bin/testfile/cube_decal.png");
            break;
        case STARSHIP_OBJ:
            userData->texMapId = LoadTexture("/carmeter/src/bin/testfile/starship_decal.png");
            break;
        case MUSHROOM_OBJ:
            userData->texMapId = LoadTexture("/carmeter/src/bin/testfile/mushroom.png");
            break;
        default:
            break;
    }


    if ( userData->texMapId == 0 )
       return FALSE;

    glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f );
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

   return TRUE;
}




///
// Update a triangle using the shader pair created in Init()
//
static void Update ( ESContext *esContext, float deltaTime)
{

    UserData *userData = (UserData*) esContext->userData;
    ESMatrix perspective;
    ESMatrix modelview;

    float    aspect;

    // Compute a rotation angle based on time to rotate the cube
    userData->angle += 1;//( deltaTime * 40.0f );
    if( userData->angle >= 360.0f )
       userData->angle -= 360.0f;


    // Compute the window aspect ratio
    aspect = 1;//(GLfloat) esContext->width / (GLfloat) esContext->height;

    // Generate a perspective matrix with a 60 degree FOV
    esMatrixLoadIdentity( &perspective );
    esPerspective( &perspective, 50.0f, aspect, 1.0f, 20.0f );



    // Generate a model view matrix to rotate/translate the cube
    esMatrixLoadIdentity( &modelview );

    // Translate away from the viewer
    esTranslate( &modelview, 0.0, 0.0, -2.0 );

    esScale( &modelview,.2,.2,.2);
    // Rotate the cube
    esRotate( &modelview, userData->angle, .0, 1.0, .0 );

    // Compute the final MVP by multiplying the modevleiw and perspective matrices together
    esMatrixMultiply( &userData->mvpMatrix, &modelview, &perspective );

#if 1
    userData->normalMatrix = processNormalMatrix(modelview);


#else
    // Normal Matrix
    // Transform normals from object-space to eye-space
    bool invertible;
    GLKMatrix3 normalMatrix = GLKMatrix4GetMatrix3(GLKMatrix4InvertAndTranspose(modelViewMatrix, &invertible));
    if(!invertible)
        NSLog(@"MV matrix is not invertible");
    glUniformMatrix3fv(self.phongShader.uNormalMatrix, 1, 0, normalMatrix.m);
#endif

}



///
// Draw a triangle using the shader pair created in Init()
//
static int i=0;
static void Draw ( ESContext *esContext )
{

    UserData *userData = esContext->userData;

    glViewport ( 100, 0 , esContext->width, esContext->height);
    glClearColor( 0.0f, 1.0f, 0.0f, 1.0f );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    

    glUseProgram(userData->program);

    // Load the vertex, texture, normal
    switch (myobj) {
        case CUBE_OBJ:
            glVertexAttribPointer ( userData->aPosition, 3, GL_FLOAT, GL_FALSE, 0, cubePositions );
            glVertexAttribPointer ( userData->aTexel, 2, GL_FLOAT, GL_FALSE, 0, cubeTexels);
            glVertexAttribPointer ( userData->aNormal, 3, GL_FLOAT, GL_FALSE, 0, cubeNormals );
            break;
        case STARSHIP_OBJ:
            glVertexAttribPointer ( userData->aPosition, 3, GL_FLOAT, GL_FALSE, 0, starshipPositions );
            glVertexAttribPointer ( userData->aTexel, 2, GL_FLOAT, GL_FALSE, 0, starshipTexels);
            glVertexAttribPointer ( userData->aNormal, 3, GL_FLOAT, GL_FALSE, 0, starshipNormals );
            break;
        case MUSHROOM_OBJ:
            glVertexAttribPointer ( userData->aPosition, 3, GL_FLOAT, GL_FALSE, 0, mushroomPositions );
            glVertexAttribPointer ( userData->aTexel, 2, GL_FLOAT, GL_FALSE, 0, mushroomTexels);
            glVertexAttribPointer ( userData->aNormal, 3, GL_FLOAT, GL_FALSE, 0, mushroomNormals );
            break;
        default:
            break;
    }



    glEnableVertexAttribArray ( userData->aPosition );
    glEnableVertexAttribArray ( userData->aTexel );
    glEnableVertexAttribArray ( userData->aNormal );

    glActiveTexture ( GL_TEXTURE0 );
    glBindTexture ( GL_TEXTURE0, userData->texMapId );
    glUniform1i ( userData->uTexture, 0 );

    glUniformMatrix4fv( userData->uMvpMatrix, 1, GL_FALSE, (GLfloat*) &userData->mvpMatrix.m[0][0] );
    glUniformMatrix3fv(userData->uNormalMatrix, 1, GL_FALSE, (GLfloat*) &userData->normalMatrix.m[0][0]);


    switch (myobj) {
        case CUBE_OBJ:
            for(i=0; i < cubeMaterials; i++)
            {
                glUniform3f(userData->uDiffuse, cubeDiffuses[i][0], cubeDiffuses[i][1], cubeDiffuses[i][2]);
                glUniform3f(userData->uSpecular, cubeSpeculars[i][0], cubeSpeculars[i][1], cubeSpeculars[i][2]);

                glDrawArrays(GL_TRIANGLES, cubeFirsts[i], cubeCounts[i]);
            }
            break;
        case STARSHIP_OBJ:
            for(i=0; i < starshipMaterials; i++)
            {
                glUniform3f(userData->uDiffuse, starshipDiffuses[i][0], starshipDiffuses[i][1], starshipDiffuses[i][2]);
                glUniform3f(userData->uSpecular, starshipSpeculars[i][0], starshipSpeculars[i][1], starshipSpeculars[i][2]);

                glDrawArrays(GL_TRIANGLES, starshipFirsts[i], starshipCounts[i]);
            }
            break;
        case MUSHROOM_OBJ:
            for(i=0; i < mushroomMaterials; i++)
            {
                glUniform3f(userData->uDiffuse, mushroomDiffuses[i][0], mushroomDiffuses[i][1], mushroomDiffuses[i][2]);
                glUniform3f(userData->uSpecular, mushroomSpeculars[i][0], mushroomSpeculars[i][1], mushroomSpeculars[i][2]);

                glDrawArrays(GL_TRIANGLES, mushroomFirsts[i], mushroomCounts[i]);
            }
            break;
        default:
            break;
    }



    eglSwapBuffers ( esContext->eglDisplay, esContext->eglSurface );
}

///
// Cleanup
//
static void ShutDown ( ESContext *esContext )
{
   UserData *userData = esContext->userData;

   // Delete texture object
   glDeleteTextures ( 1, &userData->texMapId );


   // Delete program object
   glDeleteProgram ( userData->program);
}


int main(int argc, char *argv[])
{
    ESContext esContext;
    UserData  userData;



    //==========================================================
    // Init GLES Context
    //==========================================================
    esInitContext ( &esContext );
    esContext.userData = &userData;

    esCreateWindow ( &esContext, "Model 3D Render", 640, 480, ES_WINDOW_ALPHA | ES_WINDOW_DEPTH);

    if ( !Init ( &esContext ) )
        return 0;

    esRegisterDrawFunc ( &esContext, Draw );
    esRegisterUpdateFunc ( &esContext, Update );


    esMainLoop ( &esContext );

    ShutDown ( &esContext );
    return 1;
}







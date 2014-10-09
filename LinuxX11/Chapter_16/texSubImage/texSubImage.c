//
// Book:      OpenGL(R) ES 2.0 Programming Guide
// Authors:   Aaftab Munshi, Dan Ginsburg, Dave Shreiner
// ISBN-10:   0321502795
// ISBN-13:   9780321502797
// Publisher: Addison-Wesley Professional
// URLs:      http://safari.informit.com/9780321563835
//            http://www.opengles-book.com
//

// MultiTexture.c
//
//    This is an example that draws a quad with a basemap and
//    lightmap to demonstrate multitexturing.
//

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "esUtil.h"




typedef struct
{
    // Handle to a program object
    GLuint programObject;

    // Attribute locations
    GLint  positionLoc;
    GLint  texCoordLoc;

    // Sampler locations
    GLint baseMapLoc;


    // Texture handle
    GLuint baseMapTexId;


    // Rotation angle
    GLfloat   angle;

    // Uniform locations
    GLint  mvpLoc;
    // MVP matrix
    ESMatrix  mvpMatrix;


} UserData;




char *imgBuffer = NULL;




///
// Load texture from disk
//
GLuint LoadTextureSub ( char *fileName , char *buffer)
{
    int width,
    height;
    char hasAlpha =0;
    //char *buffer;

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
    }

    if ( buffer == NULL )
    {
        esLogMessage ( "Error loading (%s) image.\n", fileName );
        return 0;
    }

    GLuint texId;
    glGenTextures ( 1, &texId );
    glBindTexture ( GL_TEXTURE_2D, texId );

    glTexImage2D ( GL_TEXTURE_2D, 0, hasAlpha ? GL_RGBA : GL_RGB, width, height, 0,  hasAlpha ? GL_RGBA : GL_RGB , GL_UNSIGNED_BYTE, NULL );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    //free ( buffer );
    return texId;
}


///
// Initialize the shader and program object
//
int Init ( ESContext *esContext )
{
   UserData *userData = esContext->userData;
   GLbyte vShaderStr[] =  
      "uniform mat4 u_mvpMatrix;    \n"
      "attribute vec4 a_position;   \n"
      "attribute vec2 a_texCoord;   \n"
      "varying vec2 v_texCoord;     \n"
      "void main()                  \n"
      "{                            \n"
      "   gl_Position = u_mvpMatrix*a_position; \n"
      "   v_texCoord = a_texCoord;  \n"
      "}                            \n";
   
   GLbyte fShaderStr[] =  
      "precision mediump float;                            \n"
      "varying vec2 v_texCoord;                            \n"
      "uniform sampler2D s_baseMap;                        \n"
      "uniform sampler2D s_lightMap;                       \n"
      "void main()                                         \n"
      "{                                                   \n"
      "  vec4 baseColor;                                   \n"
      "  vec4 lightColor;                                  \n"
      "                                                    \n"
      "  baseColor = texture2D( s_baseMap, v_texCoord );   \n"
      //"  lightColor = texture2D( s_lightMap, v_texCoord ); \n"
      //"  gl_FragColor = baseColor * (lightColor + 0.25);   \n"
      //"  gl_FragColor = vec4(255, 0, 0, 1);               \n"
        "  gl_FragColor = baseColor;                        \n"
      "}                                                    \n";

   // Load the shaders and get a linked program object
   userData->programObject = esLoadProgram ( (const char*)vShaderStr, (const char*)fShaderStr );

   // Get the attribute locations
   userData->positionLoc = glGetAttribLocation ( userData->programObject, "a_position" );
   userData->texCoordLoc = glGetAttribLocation ( userData->programObject, "a_texCoord" );
   
   // Get the sampler location
   userData->baseMapLoc = glGetUniformLocation ( userData->programObject, "s_baseMap" );

   // Get the uniform locations
   userData->mvpLoc = glGetUniformLocation( userData->programObject, "u_mvpMatrix" );

   // Load the textures
   userData->baseMapTexId = LoadTexture ( "./data/images/run_circle.png" );


   if ( userData->baseMapTexId == 0 /*|| userData->lightMapTexId == 0*/ )
      return FALSE;

   glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f );
   return TRUE;
}


///
// Handle keyboard input
//
void Key ( ESContext *esContext, unsigned char key, int x, int y)
{
   printf( "Saw an 'm'\n" );


    switch ( key )
   {
   case 'm':
      printf( "Saw an 'm'\n" );
      break;

   case 'a':
      printf( "Saw an 'a'\n" );
      break;

   case '1':
      printf( "Saw a '1'\n" );
      break;

   case 033: // ASCII Escape Key
       ShutDown( esContext );
       exit( 0 );
       break;
   }
}


///
// Update MVP matrix based on time
//
void Update ( ESContext *esContext, float deltaTime )
{
   UserData *userData = (UserData*) esContext->userData;
   ESMatrix perspective;
   ESMatrix modelview;
   float    aspect;

   // Compute a rotation angle based on time to rotate the cube
   userData->angle += ( deltaTime * 40.0f );
   if( userData->angle >= 360.0f )
      userData->angle -= 360.0f;

   //usleep(5000);
   count +=1;
   if(count > numberlines)count=0;

   // Compute the window aspect ratio
   aspect = (GLfloat) esContext->width / (GLfloat) esContext->height;

   // Generate a perspective matrix with a 60 degree FOV
   esMatrixLoadIdentity( &perspective );
   esPerspective( &perspective, 60.0f, 1, 1.0f, 20.0f );

   // Generate a model view matrix to rotate/translate the cube
   esMatrixLoadIdentity( &modelview );

   // Translate away from the viewer
   esTranslate( &modelview, 0.0, 0.0, -2.0 );

   // Rotate the cube
   //esRotate( &modelview, 180, 1.0, .0, .0 );

   // Compute the final MVP by multiplying the
   // modevleiw and perspective matrices together
   esMatrixMultiply( &userData->mvpMatrix, &modelview, &perspective );
}


///
// Draw a triangle using the shader pair created in Init()
//
void Draw ( ESContext *esContext )
{
   UserData *userData = esContext->userData;

      
   // Set the viewport
   glViewport ( 0, 0, esContext->width, esContext->height );
   //glViewport ( 0, 0, 340, 303 );
   
   // Clear the color buffer
   glClear ( GL_COLOR_BUFFER_BIT );

   // Use the program object
   glUseProgram ( userData->programObject );

   // Load the vertex position
   glVertexAttribPointer ( userData->positionLoc, 3, GL_FLOAT, GL_FALSE,  0, vVertexStrip );
   // Load the texture coordinate
   glVertexAttribPointer ( userData->texCoordLoc, 2, GL_FLOAT, GL_FALSE,  0, vVertexCoordStrip );

   glEnableVertexAttribArray ( userData->positionLoc );
   glEnableVertexAttribArray ( userData->texCoordLoc );

   // Load the MVP matrix
   glUniformMatrix4fv( userData->mvpLoc, 1, GL_FALSE, (GLfloat*) &userData->mvpMatrix.m[0][0] );


   // Bind the base map
   glActiveTexture ( GL_TEXTURE0 );
   glBindTexture ( GL_TEXTURE_2D, userData->baseMapTexId );

   // Set the base map sampler to texture unit to 0
   glUniform1i ( userData->baseMapLoc, 0 );



#if DRAW_TRIANGLE_FAN

   glDrawArrays(GL_TRIANGLE_FAN, 0, count);
   //DBG(" count = %d \n", count)

#else

   glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

#endif

   eglSwapBuffers ( esContext->eglDisplay, esContext->eglSurface );
}

///
// Cleanup
//
void ShutDown ( ESContext *esContext )
{
   UserData *userData = esContext->userData;

   // Delete texture object
   glDeleteTextures ( 1, &userData->baseMapTexId );
   //glDeleteTextures ( 1, &userData->lightMapTexId );

   // Delete program object
   glDeleteProgram ( userData->programObject );
}


int main ( int argc, char *argv[] )
{
   ESContext esContext;
   UserData  userData;

   esInitContext ( &esContext );
   esContext.userData = &userData;

   esCreateWindow ( &esContext, "MultiTexture", 600, 600, ES_WINDOW_ALPHA );
   
   if ( !Init ( &esContext ) )
      return 0;

   esRegisterDrawFunc ( &esContext, Draw );
   esRegisterUpdateFunc ( &esContext, Update );
   esRegisterKeyFunc( &esContext, Key );


   
   esMainLoop ( &esContext );

   ShutDown ( &esContext );
}


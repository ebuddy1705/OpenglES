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

#include "Fan.h"

#define DRAW_TRIANGLE_FAN 1




#if 1
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
#endif



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



#if 0

    GLfloat vVertices[] = { /* Top Left Of The Quad (Front) */
                            -1.0f,1.0f, 0.0f,
                            /* Bottom Left Of The Quad (Front) */
                            -1.0f,-1.0f, 0.0f,
                            /* Bottom Right Of The Quad (Front) */
                            1.0f,-1.0f, 0.0f,
                            /* Top Right Of The Quad (Front) */
                            1.0f,1.0f, 0.0f,
                          };

    GLfloat vTexCoord[]={
            0.0f,  0.0f,        // TexCoord 0
            0.0f,  1.0f,        // TexCoord 1

            1.0f,  1.0f,        // TexCoord 2
            1.0f,  0.0f,        // TexCoord 3
    };

    GLushort indices[] = { 0, 1, 2, 0, 2, 3 };

#endif

GLfloat *vVertices = NULL;
GLfloat *vTexCoord = NULL;
GLushort *indices = NULL;

int numberlines = 360;
int startlines = 270;
GLsizei count = 0;
#define CYCLE_PATH "./data/images/run_circle.png"
//#define CYCLE_PATH "./data/images/Meter_02_1-new.png"


// Copy vertices and normals to the memory of the GPU
void PrintFan2file(char *obj)
{
    unsigned int i = 0;
    char tmpout[128];
    sprintf(tmpout, "%s.h", obj);
    DBG("outfile: %s\n", tmpout);


    FILE *pFile=NULL;
    pFile = fopen(tmpout, "w");
    if(!pFile) return;


    fflush(pFile);
    fprintf(pFile, "unsigned int %sNumLines = %d; \n\n", obj, numberlines);
    fprintf(pFile, "unsigned int %sStartLines = %d; \n\n", obj, startlines);


    fprintf(pFile, "float %sVerts [] = { \n", obj);
    for(i = 0; i < numberlines +1; i++){

        fprintf(pFile, "%f, %f, %f, \n", vVertices[(i)*3], vVertices[((i)*3)+1], vVertices[((i)*3)+2]);
    }
    fprintf(pFile, "};\n\n");




    fprintf(pFile, "float %sTexCoords [] = { \n", obj);
    for(i = 0; i < numberlines +1 ; i++){

        fprintf(pFile, "%f, %f, \n", vTexCoord[(i)*2], vTexCoord[((i)*2)+1]);

    }
    fprintf(pFile, "};\n\n");


    fflush(pFile);
    fclose (pFile);
    return;

}








/*
 * lines: number of line (0 < lines < 360)
 * radius_x: max x <=1
 * radius_y: max y <=1
 * startlines: start lines
 * draw: direc = 1  => clockwise
 *       direc = -1 =>
 *
 * */

void loadVertex(int lines, int startlines, int direc, float radius_x, float radius_y, float center_x, float center_y)
{

    int i = 0;

    vVertices = (float*)malloc(sizeof(float)*(lines+1)*3);
    vTexCoord = (float*)malloc(sizeof(float)*(lines+1)*2);
    //indices   = (GLushort*)malloc(sizeof(GLushort)*(lines+2));

    vVertices[0*3]     = 0.0f; //set 1st to center
    vVertices[(0*3)+1] = 0.0f;
    vVertices[(0*3)+2] = 0.0f;

    vTexCoord[0] = 0.5f;
    vTexCoord[1] = 0.5f;

    //indices[0] = 0;


#if 0 //fan cycle
    for (i = 0; i < lines;i++)
    {

        vVertices[(i+1)*3]     = (radius_x * cos(2*M_PI*(float)(startlines-i)/(float)lines) + center_x);
        vVertices[((i+1)*3)+1] = (float) (radius_y * sin(2*M_PI*direc*(float)(startlines-i)/(float)lines) + center_y);
        vVertices[((i+1)*3)+2] = 0.0f;//z
        vTexCoord[(i+1)*2] =(float) (0.5*cos(2*M_PI*direc*(float)(i-startlines)/(float)lines) + 0.5);
        vTexCoord[((i+1)*2)+1] = (float) (0.5*sin(2*M_PI*direc*(float)(i-startlines)/(float)lines) + 0.5);

        DBG(" %f %f %f \n", vVertices[(i+1)*3], vVertices[((i+1)*3)+1], vVertices[((i+1)*3)+2]);
        DBG(" %f %f  \n", vTexCoord[(i+1)*2], vTexCoord[((i+1)*2)+1]);


        //indices[(i+1)] = (GLushort)i;
    }
#else  //fan square
    for (i = 0; i < lines;i++)
    {

        if((startlines-i) == 270){
            vVertices[(i+1)*3]     = 0;
            vVertices[((i+1)*3)+1] = -1;
            vVertices[((i+1)*3)+2] = 0.0f;//z

            vTexCoord[(i+1)*2] =  0.5;
            vTexCoord[((i+1)*2)+1] =  1;

            DBG("startlines-i == 270 - i = %d \n", i);
        }else if((225<= (startlines-i)) && ((startlines-i) <270)){
            vVertices[(i+1)*3]     = -1/tan(2*M_PI*(startlines-i)/(float)lines);
            vVertices[((i+1)*3)+1] = -1;
            vVertices[((i+1)*3)+2] = 0.0f;//z

            vTexCoord[(i+1)*2] = -0.5/tan(2*M_PI*(startlines-i)/(float)lines)+0.5;
            vTexCoord[((i+1)*2)+1] =  1;

            DBG("225<=startlines-i<270 - i = %d \n", i);
        }else if((135<= (startlines-i)) && ((startlines-i) <225)){
            vVertices[(i+1)*3]     = -1;
            vVertices[((i+1)*3)+1] = -tan(2*M_PI*(startlines-i)/(float)lines);
            vVertices[((i+1)*3)+2] = 0.0f;//z

            vTexCoord[(i+1)*2] = 0;
            vTexCoord[((i+1)*2)+1] =  0.5*tan(2*M_PI*(startlines-i)/(float)lines) + 0.5;

            DBG("135<=startlines-i<225 - i = %d \n", i);
        }else if((90< (startlines-i)) && ((startlines-i)<135)){
            vVertices[(i+1)*3]     = 1/tan(2*M_PI*(startlines-i)/(float)lines);
            vVertices[((i+1)*3)+1] = 1;
            vVertices[((i+1)*3)+2] = 0.0f;//z

            vTexCoord[(i+1)*2] = 0.5/tan(2*M_PI*(startlines-i)/(float)lines) + 0.5;
            vTexCoord[((i+1)*2)+1] =  0;

            DBG("90<startlines-i<135 - i = %d \n", i);
        }else if((startlines-i) == 90){
            vVertices[(i+1)*3]     = 0;
            vVertices[((i+1)*3)+1] = 1;
            vVertices[((i+1)*3)+2] = 0.0f;//z

            vTexCoord[(i+1)*2] =  0.5;
            vTexCoord[((i+1)*2)+1] =  0;

            DBG("(startlines-i) == 90 - i = %d \n", i);
        }else if((45<=(startlines-i)) && ((startlines-i) <90)){
            vVertices[(i+1)*3]     = 1/tan(2*M_PI*(startlines-i)/(float)lines);
            vVertices[((i+1)*3)+1] = 1;
            vVertices[((i+1)*3)+2] = 0.0f;//z

            vTexCoord[(i+1)*2] = 0.5/tan(2*M_PI*(startlines-i)/(float)lines) + 0.5;
            vTexCoord[((i+1)*2)+1] =  0;

            DBG("45<=startlines-i<90 - i = %d \n", i);
        }else if((-45<=(startlines-i)) && ((startlines-i) <45)){
            vVertices[(i+1)*3]     = 1;
            vVertices[((i+1)*3)+1] = tan(2*M_PI*(startlines-i)/(float)lines);
            vVertices[((i+1)*3)+2] = 0.0f;//z

            vTexCoord[(i+1)*2] = 1;
            vTexCoord[((i+1)*2)+1] =  -0.5*tan(2*M_PI*(startlines-i)/(float)lines)+0.5;

            DBG("-45<=startlines-i<45 - i = %d \n", i);
        }else if((-90<(startlines-i)) && ((startlines-i)<-45)){
            vVertices[(i+1)*3]     = -1/tan(2*M_PI*(startlines-i)/(float)lines);
            vVertices[((i+1)*3)+1] = -1;
            vVertices[((i+1)*3)+2] = 0.0f;//z

            vTexCoord[(i+1)*2] = -0.5/tan(2*M_PI*(startlines-i)/(float)lines) + 0.5;
            vTexCoord[((i+1)*2)+1] =  1;

            DBG("-90<startlines-i<-45 - i = %d \n", i);
        }
    }

#endif

    //indices[lines+1] = indices[1]; //closing part is same as for i=0


    PrintFan2file("Fan");
}






#if 0
///
// Load texture from disk
//
GLuint LoadTexture ( char *fileName )
{
    int width,
    height;
    char hasAlpha =0;
    char *buffer;

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

    glTexImage2D ( GL_TEXTURE_2D, 0, hasAlpha ? GL_RGBA : GL_RGB, width, height, 0,  hasAlpha ? GL_RGBA : GL_RGB , GL_UNSIGNED_BYTE, buffer );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    free ( buffer );
    return texId;
}
#endif

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
   userData->baseMapTexId = LoadTexture ( CYCLE_PATH );

    /*
   loadVertex(int lines, int startlines, int direc
               float radius_x, float radius_y,
               float center_x, float center_y)
   */
   //count = 90;
   loadVertex(numberlines, startlines, 1, 1, 1, 0, 0);



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

   count +=1;
   if(count > numberlines) count=0;

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
   glVertexAttribPointer ( userData->positionLoc, 3, GL_FLOAT, GL_FALSE, /*5 * sizeof(GLfloat)*/ 0, FanVerts );
   // Load the texture coordinate
   glVertexAttribPointer ( userData->texCoordLoc, 2, GL_FLOAT, GL_FALSE, /*5 * sizeof(GLfloat)*/ 0, FanTexCoords );

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

   glDrawElements ( GL_TRIANGLES, 90, GL_UNSIGNED_SHORT, indices );

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


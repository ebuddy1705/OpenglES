/*
 * components.c
 *
 *  Created on: Jul 28, 2014
 *      Author: ninhld
 */


#include <stdlib.h>
#include "esUtil.h"
#include "yuv_render.h"




typedef struct
{
   // Handle to a program object
   GLuint programObject;

   // Attribute locations
   GLint vPosition;
   GLint vTexCoord;

   // Sampler locations
   GLint samplerYUV[3];


   // Texture handle -- textId[0] = yTexId, textId[1] = uTexId, textId[2] = vTexId
   GLuint texIdYUV[3];

   //FBO -- RBO
   GLuint frame_buffer_object;
   GLuint render_buffer_object;

   //YUV420 CIF
   GLuint yuv_with;
   GLuint yuv_height;

} UserData;



//#define YUV420_QCIF
#ifdef YUV420_QCIF //qcif
    #define YUV_WIDTH 176
    #define YUV_HEIGHT 144
#else //cif
    #define YUV_WIDTH 352
    #define YUV_HEIGHT 288
#endif

static GLubyte yplane[YUV_HEIGHT][YUV_WIDTH];
static GLubyte uplane[YUV_HEIGHT/2][YUV_WIDTH/2];
static GLubyte vplane[YUV_HEIGHT/2][YUV_WIDTH/2];

static GLuint widths[3] = { YUV_WIDTH, YUV_WIDTH / 2, YUV_WIDTH / 2 };
static GLuint heights[3]  = { YUV_HEIGHT, YUV_HEIGHT / 2, YUV_HEIGHT / 2 };

static GLfloat vVertices[] =    { -1.0f,  1.0f, 0.0f,  // Position 0
                                    0.0f,  0.0f,        // TexCoord 0
                                    -1.0f, -1.0f, 0.0f,  // Position 1
                                    0.0f,  1.0f,        // TexCoord 1
                                    1.0f, -1.0f, 0.0f,  // Position 2
                                    1.0f,  1.0f,        // TexCoord 2
                                    1.0f,  1.0f, 0.0f,  // Position 3
                                    1.0f,  0.0f         // TexCoord 3
                                };

static GLushort indices[] = { 0, 1, 2, 0, 2, 3 };

static FILE *fd = NULL;
static GLint nread = 0;
//static GLint nLoopFrame = 0;


///
// Load texture from disk
//
static void LoadTexture(UserData *userData)
{
    int i = 0;


    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(3, userData->texIdYUV);

    for(i = 0; i < 3; i++) {
        glBindTexture(GL_TEXTURE_2D, userData->texIdYUV[i]);



#if 1
      glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, widths[i], heights[i], 0, GL_LUMINANCE,GL_UNSIGNED_BYTE, NULL);
#else
        if(i ==0){
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, widths[i], heights[i], 0, GL_LUMINANCE,GL_UNSIGNED_BYTE, yplane);
        }else if(i == 1){
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, widths[i], heights[i], 0, GL_LUMINANCE,GL_UNSIGNED_BYTE, uplane);
        }else {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, widths[i], heights[i], 0, GL_LUMINANCE,GL_UNSIGNED_BYTE, vplane);
        }
#endif
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        DBG("userData->texIdYUV[%d] = %d -- widths[i] = %d \n", i, userData->texIdYUV[i], widths[i]);
    }

}


///
// Load texture from disk
//
static void LoadFrameBuffer(UserData *userData)
{

    glGenFramebuffers(1, &(userData->frame_buffer_object));
    glBindFramebuffer(GL_FRAMEBUFFER, userData->frame_buffer_object);

    glGenRenderbuffers(1, &(userData->render_buffer_object));
    glBindRenderbuffer(GL_RENDERBUFFER, userData->render_buffer_object);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, userData->yuv_with, userData->yuv_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, userData->render_buffer_object);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, userData->texIdYUV[0], 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, userData->texIdYUV[1], 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, userData->texIdYUV[2], 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    GLint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            FATAL("%d\n", status);
        return;
    }
}



///
// Initialize the shader and program object
//
static int Init ( ESContext *esContext )
{
   UserData *userData = esContext->userData;

   GLbyte vShaderStr[] =
            "attribute vec4 vPosition;         \n"
            "attribute vec2 vTexCoord;         \n"
            "varying vec2 v_vTexCoord;         \n"
            "void main() {                     \n"
                "gl_Position = vPosition;       \n"
                "v_vTexCoord = vTexCoord;       \n"
            "}                                 \n";


   GLbyte fShaderStr[] =
            "precision mediump float;           \n"
            "varying vec2 v_vTexCoord;          \n"
            "uniform sampler2D ySampler;        \n"
            "uniform sampler2D uSampler;        \n"
            "uniform sampler2D vSampler;        \n"
            "void main() {                      \n"
                "float y=texture2D(ySampler, v_vTexCoord).r;        \n"
                "float u=texture2D(uSampler, v_vTexCoord).r - 0.5;  \n"
                "float v=texture2D(vSampler, v_vTexCoord).r - 0.5;  \n"
                "float r=y + 1.402 * v;\n"
                "float g=y - 0.344 * u - 0.714 * v;\n"
                "float b=y + 1.772 * u;\n"
                "gl_FragColor = vec4(r, g, b, 1.0);\n"
            "}\n";


   // Load the shaders and get a linked program object
   userData->programObject = esLoadProgram ( (char *)vShaderStr, (char *)fShaderStr );

   // Get the attribute locations
   userData->vPosition = glGetAttribLocation ( userData->programObject, "vPosition" );
   userData->vTexCoord = glGetAttribLocation ( userData->programObject, "vTexCoord" );

   // Get the sampler location
   userData->samplerYUV[0] = glGetUniformLocation ( userData->programObject, "ySampler" );
   userData->samplerYUV[1] = glGetUniformLocation ( userData->programObject, "uSampler" );
   userData->samplerYUV[2] = glGetUniformLocation ( userData->programObject, "vSampler" );

   // YUV420 CIF size
   userData->yuv_with = 352;
   userData->yuv_height = 288;

   //Load the textures
   LoadTexture(userData);

   //SetUp Frame Buffer Object (FBO) and Render Buffer Object (RBO)
   LoadFrameBuffer(userData);

   if ( userData->texIdYUV[0] == 0 ||  userData->texIdYUV[1] == 0 || userData->texIdYUV[2] == 0)
      return FALSE;

   glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f );

   return TRUE;
}


static void Update ( ESContext *esContext, float deltaTime )
{
    //get frame from camera or YUV file

    //load YUV data from file
     usleep(25000);
    if(fd == NULL){
    #ifdef BUILDTARGET
        fd = fopen("/mnt/testfile/stefan_cif_352x288.yuv", "rb");
        //fd = fopen("/mnt/testfile/bridge-close_cif_352x288.yuv", "rb");
    #else
        fd = fopen("/carmeter/src/bin/testfile/stefan_cif_352x288.yuv", "rb");
    #endif
    }

    if(fd){
        if((nread = fread(yplane, 1, sizeof(yplane), fd)) > 0){
            fread(uplane, 1, sizeof(uplane), fd);
            fread(vplane, 1, sizeof(vplane), fd);
            //nLoopFrame++;
        }else{
            fclose(fd);
            fd = NULL;
            DBG("End Of File \n");
            //nLoopFrame = 0;
        }
    }
}

///
// Draw a triangle using the shader pair created in Init()
//
static void Draw ( ESContext *esContext )
{


    int i = 0;

    UserData *userData = esContext->userData;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, 352, 288);
    glClearColor( 0.0f, 1.0f, 0.0f, 1.0f );
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(userData->programObject);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    //read frame from YUV file
    for(i = 0; i < 3; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, userData->texIdYUV[i]);
        glUniform1i(userData->samplerYUV[i], i);


        if(i ==0){
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, widths[i], heights[i], 0, GL_LUMINANCE,GL_UNSIGNED_BYTE, yplane);
        }else if(i == 1){
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, widths[i], heights[i], 0, GL_LUMINANCE,GL_UNSIGNED_BYTE, uplane);
        }else {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, widths[i], heights[i], 0, GL_LUMINANCE,GL_UNSIGNED_BYTE, vplane);
        }
    }


    // Load the vertex position
    glVertexAttribPointer ( userData->vPosition, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), vVertices );
    glVertexAttribPointer ( userData->vTexCoord, 2, GL_FLOAT,GL_FALSE, 5 * sizeof(GLfloat), &vVertices[3] );

    glEnableVertexAttribArray ( userData->vPosition );
    glEnableVertexAttribArray ( userData->vTexCoord );

    glBindRenderbuffer(GL_RENDERBUFFER, userData->render_buffer_object);
    glDrawElements ( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices );

    eglSwapBuffers ( esContext->eglDisplay, esContext->eglSurface );
}

///
// Cleanup
//
static void ShutDown ( ESContext *esContext )
{
   DBG("\n");
    UserData *userData = esContext->userData;

   // Delete texture object
   glDeleteTextures ( 1, &userData->texIdYUV[0] );
   glDeleteTextures ( 1, &userData->texIdYUV[1] );
   glDeleteTextures ( 1, &userData->texIdYUV[2] );

   // Delete program object
   glDeleteProgram ( userData->programObject );
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

    esCreateWindow ( &esContext, "Camera YUV420 Render", 640, 480, ES_WINDOW_ALPHA );

    if ( !Init ( &esContext ) )
        return 0;

    esRegisterDrawFunc ( &esContext, Draw );
    esRegisterUpdateFunc ( &esContext, Update );

    esMainLoop ( &esContext );

    ShutDown ( &esContext );
    return 1;
}







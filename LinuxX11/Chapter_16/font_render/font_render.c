/*
 * components.c
 *
 *  Created on: Jul 28, 2014
 *      Author: ninhld
 */


#include <stdlib.h>
#include "esUtil.h"
#include "font_render.h"




typedef struct
{
   // Handle to a program object
   GLuint programObject;

   // Attribute locations
   GLint  coord;

   // Uniform color
   GLint  color;

   // Sampler locations
   GLint sampler;

   // Texture handle
   GLuint textMapId;


   //other
   FT_Library ft;
   FT_Face face;
   FT_GlyphSlot g;

} UserData;






static void render_text(ESContext *esContext, const char *text, float x, float y, float sx, float sy)
{

   // DBG(" \n");
  const char *p;
  UserData *userData = esContext->userData;
  FT_GlyphSlot g = userData->g;

  for(p = text; *p; p++) {
    if(FT_Load_Char(userData->face, *p, FT_LOAD_RENDER))
        continue;

    //DBG("\n");

    glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_ALPHA,
      g->bitmap.width,
      g->bitmap.rows,
      0,
      GL_ALPHA,
      GL_UNSIGNED_BYTE,
      g->bitmap.buffer
    );

    float x2 = x + g->bitmap_left * sx;
    float y2 = -y - g->bitmap_top * sy;
    float w = g->bitmap.width * sx;
    float h = g->bitmap.rows * sy;

    GLfloat box[4][4] = {
        {x2,     -y2    , 0, 0},
        {x2 + w, -y2    , 1, 0},
        {x2,     -y2 - h, 0, 1},
        {x2 + w, -y2 - h, 1, 1},
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(box), box, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    x += (g->advance.x >> 6) * sx;
    y += (g->advance.y >> 6) * sy;
  }//end for
}


///
// Load texture from disk
//
static GLuint LoadTexture()
{

    GLuint tex;

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

   return tex;
}



///
// Initialize the shader and program object
//
static int Init ( ESContext *esContext )
{
   UserData *userData = esContext->userData;

   GLbyte vShaderStr[] =
           "attribute vec4 a_position;                      \n"
           "varying vec2 v_texCoord ;                       \n"
           "void main()                                     \n"
           "{                                               \n"
           "   gl_Position =  vec4(a_position.xy, 0, 1) ;   \n"
           "   v_texCoord =  a_position.zw;                 \n"
           "}                                               \n";


   GLbyte fShaderStr[] =
           "precision mediump float;                            \n"
           "varying vec2 v_texCoord;                            \n"
           "uniform sampler2D s_texMap;                         \n"
           "uniform vec4 u_color;                               \n"
           "void main()                                                                     \n"
           "{                                                                               \n"
           "	gl_FragColor = vec4(1, 1, 1, texture2D(s_texMap, v_texCoord).a) * u_color;  \n"
           "}                                                                               \n";



   // Load the shaders and get a linked program object
   userData->programObject = esLoadProgram ( (char *)vShaderStr, (char *)fShaderStr );

   // Get the attribute locations
   userData->coord = glGetAttribLocation ( userData->programObject, "a_position" );

   // Get the uniform color locations
   userData->color = glGetUniformLocation ( userData->programObject, "u_color" );

   // Get the sampler location
   userData->sampler = glGetUniformLocation ( userData->programObject, "s_texMap" );

   // Load the textures
   userData->textMapId = LoadTexture();


   if ( userData->textMapId == 0 )
      return FALSE;

   glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f );
   return TRUE;
}

///
// Draw a triangle using the shader pair created in Init()
//
static void Draw ( ESContext *esContext )
{

    UserData *userData = esContext->userData;
    GLuint vbo;

    glViewport ( 100, 0 , 320 /*esContext->width*/, 240/*esContext->height*/);
    glClearColor( 0.0f, 1.0f, 0.0f, 1.0f );
    glClear(GL_COLOR_BUFFER_BIT);


    glGenBuffers(1, &vbo);
    glUseProgram ( userData->programObject );

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(userData->coord, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(userData->coord);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture ( GL_TEXTURE_2D, userData->textMapId );
    glUniform1i(userData->sampler, 0);


    GLfloat mycolor[4] = {0, 0, 1, 1};
    glUniform4fv(userData->color, 1, mycolor);

    float sx = 2.0 / esContext->width;
    float sy = 2.0 / esContext->height;

    render_text(esContext, "I love you", -0.5 + 8 * sx,   1 - 50 * sy,    sx, sy);
    render_text(esContext, "Make me crazy, fuck fuck fap fap, fuck fuck fap fap", -1 + 8.5 * sx, 1 - 100.5 * sy, sx, sy);

    eglSwapBuffers ( esContext->eglDisplay, esContext->eglSurface );

}

///
// Cleanup
//
static void ShutDown ( ESContext *esContext )
{
   UserData *userData = esContext->userData;

   // Delete texture object
   glDeleteTextures ( 1, &userData->textMapId );


   // Delete program object
   glDeleteProgram ( userData->programObject );
}


int main(int argc, char *argv[])
{
    ESContext esContext;
    UserData  userData;


    //==========================================================
    // Init Font
    //==========================================================
    if(FT_Init_FreeType(&(userData.ft))) {
      fprintf(stderr, "Could not init freetype library\n");
      return 0;
    }

#ifdef BUILDTARGET
    if(FT_New_Face(userData.ft, "/mnt/Vbutlong.ttf", 0, &(userData.face))) {
      fprintf(stderr, "Could not open font\n");
      return 0;
    }
#else
    if(FT_New_Face(userData.ft, "./bin/Vbutlong.ttf", 0, &(userData.face))) {
      fprintf(stderr, "Could not open font\n");
      return 0;
    }
#endif

    FT_Set_Pixel_Sizes(userData.face, 0, 30);
    userData.g = userData.face->glyph;



    //==========================================================
    // Init GLES Context
    //==========================================================
    esInitContext ( &esContext );
    esContext.userData = &userData;

    esCreateWindow ( &esContext, "Font Render", 640, 480, ES_WINDOW_ALPHA );

    if ( !Init ( &esContext ) )
        return 0;

    esRegisterDrawFunc ( &esContext, Draw );

    esMainLoop ( &esContext );

    ShutDown ( &esContext );
    return 1;
}







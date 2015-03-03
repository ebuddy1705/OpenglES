//
// Book:      OpenGL(R) ES 2.0 Programming Guide
// Authors:   Aaftab Munshi, Dan Ginsburg, Dave Shreiner
// ISBN-10:   0321502795
// ISBN-13:   9780321502797
// Publisher: Addison-Wesley Professional
// URLs:      http://safari.informit.com/9780321563835
//            http://www.opengles-book.com
//

// ESUtil.c
//
//    A utility library for OpenGL ES.  This library provides a
//    basic common framework for the example applications in the
//    OpenGL ES 2.0 Programming Guide.
//

///
//  Includes
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/time.h>
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include "esUtil.h"
#include <png.h>
#ifdef USE_EGL_X11
#include  <X11/Xlib.h>
#include  <X11/Xatom.h>
#include  <X11/Xutil.h>
#elif (defined EGL_API_FB)
#endif

#include <Debug.h>

#ifdef USE_EGL_X11
// X11 related local variables
static Display *x_display = NULL;
#endif

#if 0
static EGLNativeWindowType fsl_createwindow(EGLDisplay egldisplay, EGLNativeDisplayType eglNativeDisplayType)
{
	EGLNativeWindowType native_window = (EGLNativeWindowType)0;

#if (defined EGL_USE_X11)
	Window window, rootwindow;
	int screen = DefaultScreen(eglNativeDisplayType);		
	rootwindow = RootWindow(eglNativeDisplayType,screen); 
	window = XCreateSimpleWindow(eglNativeDisplayType, rootwindow, 0, 0, 400, 533, 0, 0, WhitePixel (eglNativeDisplayType, screen));
	XMapWindow(eglNativeDisplayType, window);
	native_window = window;	
#else
	const char *vendor = eglQueryString(egldisplay, EGL_VENDOR);
	if (strstr(vendor, "Imagination Technologies"))
		native_window = (EGLNativeWindowType)0;
	else if (strstr(vendor, "AMD"))
		native_window = (EGLNativeWindowType)  open("/dev/fb0", O_RDWR);
	else if (strstr(vendor, "Vivante")) //NEEDS FIX - functs don't exist on other platforms
	{
#if (defined EGL_API_FB)
		native_window = fbCreateWindow(eglNativeDisplayType, 0, 0, SCREEN_W, SCREEN_H);
#endif		
	}
	else
	{
		printf("Unknown vendor [%s]\n", vendor);
		return 0;
	}	
#endif
	return native_window;

}
#endif
///
// CreateEGLContext()
//
//    Creates an EGL rendering context and all associated elements
//
EGLBoolean CreateEGLContext ( EGLNativeWindowType hWnd, EGLDisplay* eglDisplay,
                              EGLContext* eglContext, EGLSurface* eglSurface,
                              EGLint attribList[])
{
   EGLint numConfigs;
   EGLint majorVersion;
   EGLint minorVersion;
   EGLDisplay display;
   EGLContext context;
   EGLSurface surface;
   EGLConfig config;
   EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE, EGL_NONE };
#ifdef EGL_API_FB   
   EGLNativeWindowType eglNativeWindow = NULL;
   EGLNativeDisplayType eglNativeDisplayType = NULL;
#endif   
   
#ifdef USE_EGL_X11
   // Get Display
   display = eglGetDisplay((EGLNativeDisplayType)x_display);
   if ( display == EGL_NO_DISPLAY )
   {
      return EGL_FALSE;
   }
#else
   eglNativeDisplayType = fbGetDisplayByIndex(0); //Pass the argument as required to show the framebuffer	
   display = eglGetDisplay(eglNativeDisplayType);
   if ( display == EGL_NO_DISPLAY )
   {
      return EGL_FALSE;
   }
#endif
   // Initialize EGL
   if ( !eglInitialize(display, &majorVersion, &minorVersion) )
   {
      return EGL_FALSE;
   }

   // Get configs
   if ( !eglGetConfigs(display, NULL, 0, &numConfigs) )
   {
      return EGL_FALSE;
   }

   // Choose config
   if ( !eglChooseConfig(display, attribList, &config, 1, &numConfigs) )
   {
      return EGL_FALSE;
   }
#ifdef EGL_API_FB
   DBG("SCREEN_W = %d, SCREEN_H = %d \n", SCREEN_W, SCREEN_H);
    eglNativeWindow = fbCreateWindow(eglNativeDisplayType, 0, 0, SCREEN_W, SCREEN_H);
    surface = eglCreateWindowSurface(display, config, (EGLNativeWindowType)eglNativeWindow, NULL);
   if ( surface == EGL_NO_SURFACE )
   {
      return EGL_FALSE;
   }
//   esContext->hWnd = (EGLNativeWindowType) eglNativeWindow;

#else
   surface = eglCreateWindowSurface(display, config, (EGLNativeWindowType)hWnd, NULL);
   if ( surface == EGL_NO_SURFACE )
   {
      return EGL_FALSE;
   }
#endif
   // Create a surface

   // Create a GL context
   context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs );
   if ( context == EGL_NO_CONTEXT )
   {
      return EGL_FALSE;
   }   
   
   // Make the context current
   if ( !eglMakeCurrent(display, surface, surface, context) )
   {
      return EGL_FALSE;
   }
   
   *eglDisplay = display;
   *eglSurface = surface;
   *eglContext = context;
   return EGL_TRUE;
} 


///
//  WinCreate()
//
//      This function initialized the native X11 display and window for EGL
//
EGLBoolean WinCreate(ESContext *esContext, const char *title)
{
#ifdef USE_EGL_X11
    Window root;
    XSetWindowAttributes swa;
    XSetWindowAttributes  xattr;
    Atom wm_state;
    XWMHints hints;
    XEvent xev;
//     EGLConfig ecfg;
//     EGLint num_config;
    Window win;

    /*
     * X11 native display initialization
     */

    x_display = XOpenDisplay(NULL);
    if ( x_display == NULL )
    {
        return EGL_FALSE;
    }

    root = DefaultRootWindow(x_display);

    swa.event_mask  =  ExposureMask | PointerMotionMask | KeyPressMask;
    win = XCreateWindow(
               x_display, root,
               0, 0, esContext->width, esContext->height, 0,
               CopyFromParent, InputOutput,
               CopyFromParent, CWEventMask,
               &swa );

    xattr.override_redirect = FALSE;
    XChangeWindowAttributes ( x_display, win, CWOverrideRedirect, &xattr );

    hints.input = TRUE;
    hints.flags = InputHint;
    XSetWMHints(x_display, win, &hints);

    // make the window visible on the screen
    XMapWindow (x_display, win);
    XStoreName (x_display, win, title);

    // get identifiers for the provided atom name strings
    wm_state = XInternAtom (x_display, "_NET_WM_STATE", FALSE);

    memset ( &xev, 0, sizeof(xev) );
    xev.type                 = ClientMessage;
    xev.xclient.window       = win;
    xev.xclient.message_type = wm_state;
    xev.xclient.format       = 32;
    xev.xclient.data.l[0]    = 1;
    xev.xclient.data.l[1]    = FALSE;
    XSendEvent (
       x_display,
       DefaultRootWindow ( x_display ),
       FALSE,
       SubstructureNotifyMask,
       &xev );

    esContext->hWnd = (EGLNativeWindowType) win;
#endif
    return EGL_TRUE;
}

#include <unistd.h>
#include <termios.h>

char getchx() {
        char buf = 0;
        struct termios old = {0};
        if (tcgetattr(0, &old) < 0)
                perror("tcsetattr()");
        old.c_lflag &= ~ICANON;
        old.c_lflag &= ~ECHO;
#if !(defined FREESCALE)
        old.c_cc[VMIN] = 1;
#else
	old.c_cc[VMIN] = 0;
#endif
        old.c_cc[VTIME] = 0;
        if (tcsetattr(0, TCSANOW, &old) < 0)
                perror("tcsetattr ICANON");
         if (read(0, &buf, 1) < 0) ;
//                 perror ("read()");
        old.c_lflag |= ICANON;
        old.c_lflag |= ECHO;
        if (tcsetattr(0, TCSADRAIN, &old) < 0)
                perror ("tcsetattr ~ICANON");
        return (buf);
}

///
//  userInterrupt()
//
//      Reads from X11 event loop and interrupt program if there is a keypress, or
//      window close action.
//
GLboolean userInterrupt(ESContext *esContext)
{
//#if (defined USE_EGL_X11)
#if (defined USE_EGL_X11)
    XEvent xev;
    KeySym key;
    volatile GLboolean userinterrupt = GL_FALSE;
    char text;

    // Pump all messages from X server. Keypresses are directed to keyfunc (if defined)
    while ( XPending ( x_display ) )
    {
        XNextEvent( x_display, &xev );
        if ( xev.type == KeyPress )
        {
            if (XLookupString(&xev.xkey,&text,1,&key,0)==1)
            {
                if (esContext->keyFunc != NULL)
                    esContext->keyFunc(esContext, text, 0, 0);
            }
            if(text=='q') userinterrupt = GL_TRUE;
        }
        if ( xev.type == DestroyNotify )
            userinterrupt = GL_TRUE;
    }
    return userinterrupt;
#else
    char text;
    GLboolean userinterrupt = GL_FALSE;
    text=getchx();
    if (text !=0 && esContext->keyFunc != NULL)
      esContext->keyFunc(esContext, text, 0, 0);

    if(text=='q') userinterrupt = GL_TRUE;
    return userinterrupt;
#endif
}


//////////////////////////////////////////////////////////////////
//
//  Public Functions
//
//

///
//  esInitContext()
//
//      Initialize ES utility context.  This must be called before calling any other
//      functions.
//
void ESUTIL_API esInitContext ( ESContext *esContext )
{
   if ( esContext != NULL )
   {
      memset( esContext, 0, sizeof( ESContext) );
   }
}


///
//  esCreateWindow()
//
//      title - name for title bar of window
//      width - width of window to create
//      height - height of window to create
//      flags  - bitwise or of window creation flags 
//          ES_WINDOW_ALPHA       - specifies that the framebuffer should have alpha
//          ES_WINDOW_DEPTH       - specifies that a depth buffer should be created
//          ES_WINDOW_STENCIL     - specifies that a stencil buffer should be created
//          ES_WINDOW_MULTISAMPLE - specifies that a multi-sample buffer should be created
//
GLboolean ESUTIL_API esCreateWindow ( ESContext *esContext, const char* title, GLint width, GLint height, GLuint flags )
{
   EGLint attribList[] =
   {
       EGL_RED_SIZE,       5,
       EGL_GREEN_SIZE,     6,
       EGL_BLUE_SIZE,      5,
       EGL_ALPHA_SIZE,     (flags & ES_WINDOW_ALPHA) ? 8 : EGL_DONT_CARE,
       EGL_DEPTH_SIZE,     (flags & ES_WINDOW_DEPTH) ? 8 : EGL_DONT_CARE,
       EGL_STENCIL_SIZE,   (flags & ES_WINDOW_STENCIL) ? 8 : EGL_DONT_CARE,
       EGL_SAMPLE_BUFFERS, (flags & ES_WINDOW_MULTISAMPLE) ? 1 : 0,
       EGL_NONE
   };
   
   if ( esContext == NULL )
   {
      return GL_FALSE;
   }

   esContext->width = width;
   esContext->height = height;

   if ( !WinCreate ( esContext, title) )
   {
      return GL_FALSE;
   }

  
   if ( !CreateEGLContext ( esContext->hWnd,
                            &esContext->eglDisplay,
                            &esContext->eglContext,
                            &esContext->eglSurface,
                            attribList) )
   {
      return GL_FALSE;
   }
   

   return GL_TRUE;
}


///
//  esMainLoop()
//
//    Start the main loop for the OpenGL ES application
//
void ESUTIL_API esMainLoop ( ESContext *esContext )
{
    
    struct timeval t1, t2;
    struct timezone tz;
    float deltatime;
    float totaltime = 0.0f;
    unsigned int frames = 0;

    gettimeofday ( &t1 , &tz );
    while(userInterrupt(esContext) == GL_FALSE)
    {
        gettimeofday(&t2, &tz);
        deltatime = (float)(t2.tv_sec - t1.tv_sec + (t2.tv_usec - t1.tv_usec) * 1e-6);
        t1 = t2;
	
        if (esContext->updateFunc != NULL)
            esContext->updateFunc(esContext, deltatime);
	
        if (esContext->drawFunc != NULL)
            esContext->drawFunc(esContext);
	
        eglSwapBuffers(esContext->eglDisplay, esContext->eglSurface);
	
        totaltime += deltatime;
        frames++;
        if (totaltime >  2.0f)
        {
           // printf("%4d frames rendered in %1.4f seconds -> FPS=%3.4f\n", frames, totaltime, frames/totaltime);
            totaltime -= 2.0f;
            frames = 0;
        }
    }
}

#if 0
//  esRegisterDrawFunc()
//
void ESUTIL_API esRegisterKeyEventFunc ( ESContext *esContext, void (ESCALLBACK *keyEventFunc) (ESContext* ,int ,int) )
{
   esContext->keyEventFunc = keyEventFunc;
}
#endif
///
//  esRegisterDrawFunc()
//
void ESUTIL_API esRegisterDrawFunc ( ESContext *esContext, void (ESCALLBACK *drawFunc) (ESContext* ) )
{
   esContext->drawFunc = drawFunc;
}


///
//  esRegisterUpdateFunc()
//
void ESUTIL_API esRegisterUpdateFunc ( ESContext *esContext, void (ESCALLBACK *updateFunc) ( ESContext*, float ) )
{
   esContext->updateFunc = updateFunc;
}


///
//  esRegisterKeyFunc()
//
void ESUTIL_API esRegisterKeyFunc ( ESContext *esContext,
                                    void (ESCALLBACK *keyFunc) (ESContext*, unsigned char, int, int ) )
{
   esContext->keyFunc = keyFunc;
}


///
// esLogMessage()
//
//    Log an error message to the debug output for the platform
//
void ESUTIL_API esLogMessage ( const char *formatStr, ... )
{
    va_list params;
    char buf[BUFSIZ];

    va_start ( params, formatStr );
    vsprintf ( buf, formatStr, params );
    
    printf ( "%s", buf );
    
    va_end ( params );
}


///
// esLoadTGA()
//
//    Loads a 24-bit TGA image from a file. This is probably the simplest TGA loader ever.
//    Does not support loading of compressed TGAs nor TGAa with alpha channel. But for the
//    sake of the examples, this is sufficient.
//

char* ESUTIL_API esLoadTGA ( char *fileName, int *width, int *height )
{
    char *buffer = NULL;
    FILE *f;
    unsigned char tgaheader[12];
    unsigned char attributes[6];
    unsigned int imagesize;

    f = fopen(fileName, "rb");
    if(f == NULL) return NULL;

    if(fread(&tgaheader, sizeof(tgaheader), 1, f) == 0)
    {
        fclose(f);
        return NULL;
    }

    if(fread(attributes, sizeof(attributes), 1, f) == 0)
    {
        fclose(f);
        return 0;
    }

    *width = attributes[1] * 256 + attributes[0];
    *height = attributes[3] * 256 + attributes[2];
    imagesize = attributes[4] / 8 * *width * *height;
    buffer = malloc(imagesize);
    if (buffer == NULL)
    {
        fclose(f);
        return 0;
    }

    if(fread(buffer, 1, imagesize, f) != imagesize)
    {
        free(buffer);
        return NULL;
    }
    fclose(f);
    return buffer;
}


int esLoadBMP(char *filename, Image *image)
{
	printf("%s %d\n",__FUNCTION__,__LINE__);
	FILE *file;
	unsigned long size;                 // size of the image in bytes.
	unsigned long i;                    // standard counter.
	unsigned short int planes;          // number of planes in image (must be 1) 
	unsigned short int bpp;             // number of bits per pixel (must be 24)
	char temp;                          // temporary color storage for bgr-rgb conversion.

	// make sure the file is there.
	if ((file = fopen(filename, "rb"))==NULL)
	{
		printf("File Not Found : %s\n",filename);
		return 0;
	}

	// seek through the bmp header, up to the width/height:
	fseek(file, 18, SEEK_CUR);

	// read the width
	if ((i = fread(&image->sizeX, 4, 1, file)) != 1) 
	{
		printf("Error reading width from %s.\n", filename);
		return 0;
	}
	

	// read the height 
	if ((i = fread(&image->sizeY, 4, 1, file)) != 1) 
	{
		printf("Error reading height from %s.\n", filename);
		return 0;
	}	
	printf("%s %d\n",__FUNCTION__,__LINE__);

	// read the planes
	if ((fread(&planes, 2, 1, file)) != 1) 
	{
		printf("Error reading planes from %s.\n", filename);
		return 0;
	}
	if (planes != 1)
	{
		printf("Planes from %s is not 1: %u\n", filename, planes);
		return 0;
	}

	// read the bpp
	if ((i = fread(&bpp, 2, 1, file)) != 1)
	{
		printf("Error reading bpp from %s.\n", filename);
		return 0;
	}
		// calculate the size (assuming 24 bits or 3 bytes per pixel).
	size = image->sizeX * image->sizeY * bpp/8  ;
	image->bpp=bpp;
	//printf("%s %d\n",__FUNCTION__,__LINE__);
	//printf("size is %lu\n",size);
/*
	if (bpp != 24) 
	{
		printf("Bpp from %s is not 24: %u\n", filename, bpp);
		return 0;
	}
*/
	// seek past the rest of the bitmap header.
	fseek(file, 24, SEEK_CUR);

	// read the data. 
	image->data = (char *) malloc(size);
	if (image->data == NULL) 
	{
		printf("Error allocating memory for color-corrected image data");
		return 0;	
	}

	if ((i = fread(image->data, size, 1, file)) != 1) 
	{
		printf("Error reading image data from %s.\n", filename);
		return 0;
	}
#if 0
	for (i=0;i<size;i+=3) 
	{ // reverse all of the colors. (bgr -> rgb)
		temp = image->data[i];
		image->data[i] = image->data[i+2];
		image->data[i+2] = temp;
	}
#else
	if(image->bpp == 24)
	{
	  //printf("%s %d  24bits\n",__FUNCTION__,__LINE__);
	  for (i=0;i<size;i+=3) 
	  { // reverse all of the colors. (bgr -> rgb)
		  temp = image->data[i];
		  image->data[i] = image->data[i+2];
		  image->data[i+2] = temp;
	  }
	}
	else if(image->bpp == 32)
	{
	  
	}
#endif
     // printf("%s %d\n",__FUNCTION__,__LINE__);
	// we're done.
	return 1;
}

void * esLoadPNG(char *name, int *outWidth, int *outHeight, char *outHasAlpha)
{
    char*outData=NULL;
    png_structp png_ptr;
    png_infop info_ptr;
    unsigned int sig_read = 0;
    int color_type, interlace_type;
    FILE *fp;
 
    if ((fp = fopen(name, "rb")) == NULL)
        return NULL;
 
    /* Create and initialize the png_struct
     * with the desired error handler
     * functions.  If you want to use the
     * default stderr and longjump method,
     * you can supply NULL for the last
     * three parameters.  We also supply the
     * the compiler header file version, so
     * that we know if the application
     * was compiled with a compatible version
     * of the library.  REQUIRED
     */
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                     NULL, NULL, NULL);
 
    if (png_ptr == NULL) {
        fclose(fp);
        return NULL;
    }
 
    /* Allocate/initialize the memory
     * for image information.  REQUIRED. */
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        fclose(fp);
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return NULL;
    }
 
    /* Set error handling if you are
     * using the setjmp/longjmp method
     * (this is the normal method of
     * doing things with libpng).
     * REQUIRED unless you  set up
     * your own error handlers in
     * the png_create_read_struct()
     * earlier.
     */
    if (setjmp(png_jmpbuf(png_ptr))) {
        /* Free all of the memory associated
         * with the png_ptr and info_ptr */
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fp);
        /* If we get here, we had a
         * problem reading the file */
	return NULL;
      
    }
 
    /* Set up the output control if
     * you are using standard C streams */
    png_init_io(png_ptr, fp);
 
    /* If we have already
     * read some of the signature */
    png_set_sig_bytes(png_ptr, sig_read);
 
    /*
     * If you have enough memory to read
     * in the entire image at once, and
     * you need to specify only
     * transforms that can be controlled
     * with one of the PNG_TRANSFORM_*
     * bits (this presently excludes
     * dithering, filling, setting
     * background, and doing gamma
     * adjustment), then you can read the
     * entire image (including pixels)
     * into the info structure with this
     * call
     *
     * PNG_TRANSFORM_STRIP_16 |
     * PNG_TRANSFORM_PACKING  forces 8 bit
     * PNG_TRANSFORM_EXPAND forces to
     *  expand a palette into RGB
     */
    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);
 
    png_uint_32 width, height;
    int bit_depth;
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
                 &interlace_type, NULL, NULL);
    if(outWidth)*outWidth = width;
    if(outHeight)*outHeight = height;
    if(outHasAlpha)*outHasAlpha=0;
    if (color_type & PNG_COLOR_MASK_ALPHA) {      
      if(outHasAlpha)*outHasAlpha=1;
     // printf("%s\n",*outHasAlpha? "has alpha": "not alpha channel");
    }
    unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
    //printf("%dx%d %d\n",(int)width,(int)height,(int)row_bytes);
    outData = (char*) malloc(row_bytes * height);
     png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);
    int i;
    for ( i = 0; i < height; i++) {
        // note that png is ordered top to
        // bottom, but OpenGL expect it bottom to top
        // so the order or swapped
        //memcpy(outData+(row_bytes * (height-1-i)), row_pointers[i], row_bytes);
      memcpy(outData+(row_bytes * (i)), row_pointers[i], row_bytes);
    }
 
    /* Clean up after the read,
     * and free any memory allocated */
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
 
    /* Close the file */
    fclose(fp);
 
    /* That's it */
    return outData;
}


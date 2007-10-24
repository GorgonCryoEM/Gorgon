
/* shadowcast/glstuff.h - OpenGL extension header for shadowcast */

/* Copyright NVIDIA Corporation, 2000, 2001. */

/* $Id$ */

#ifndef __GLEXT_PROC_DEFS_H__
#define __GLEXT_PROC_DEFS_H__

#include <GL/glExt.h>

/* Macintosh has ARB_multitexture entry points.  No need for
   these function pointers for Macintosh. */
# if !defined(macintosh) && !defined(__linux__)
/* ARB_multitexture command function pointers */
static PFNGLMULTITEXCOORD2IARBPROC glMultiTexCoord2iARB;
static PFNGLMULTITEXCOORD2FVARBPROC glMultiTexCoord2fvARB;
static PFNGLMULTITEXCOORD2DVARBPROC glMultiTexCoord2dvARB;
static PFNGLMULTITEXCOORD3FARBPROC glMultiTexCoord3fARB;
static PFNGLMULTITEXCOORD3FVARBPROC glMultiTexCoord3fvARB;
static PFNGLMULTITEXCOORD4FARBPROC glMultiTexCoord4fARB;
static PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB;
static PFNGLMULTITEXCOORD1FARBPROC glMultiTexCoord1fARB;
static PFNGLMULTITEXCOORD1DARBPROC glMultiTexCoord1dARB;
static PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;
# endif

# ifdef _WIN32
/* WGL_EXT_swap_control command function pointers */
static PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
static PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT;
# endif

/* NV_register_combiners command function pointers */
static PFNGLCOMBINERPARAMETERFVNVPROC glCombinerParameterfvNV;
static PFNGLCOMBINERPARAMETERIVNVPROC glCombinerParameterivNV;
static PFNGLCOMBINERPARAMETERFNVPROC glCombinerParameterfNV;
static PFNGLCOMBINERPARAMETERINVPROC glCombinerParameteriNV;
static PFNGLCOMBINERINPUTNVPROC glCombinerInputNV;
static PFNGLCOMBINEROUTPUTNVPROC glCombinerOutputNV;
static PFNGLFINALCOMBINERINPUTNVPROC glFinalCombinerInputNV;
static PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC glGetCombinerInputParameterfvNV;
static PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC glGetCombinerInputParameterivNV;
static PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC glGetCombinerOutputParameterfvNV;
static PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC glGetCombinerOutputParameterivNV;
static PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC glGetFinalCombinerInputParameterfvNV;
static PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC glGetFinalCombinerInputParameterivNV;


#ifdef _WIN32
# define GET_PROC_ADDRESS(x) wglGetProcAddress(x)
#else
# ifdef macintosh
#  ifndef GL_MAC_GET_PROC_ADDRESS_NV
#   define GL_MAC_GET_PROC_ADDRESS_NV 0x84FC
#  endif
#  define GET_PROC_ADDRESS(x) macGetProcAddress(x)
void *(*macGetProcAddress)(char *string);
# else
#  define GET_PROC_ADDRESS(x) glXGetProcAddress(x)
# endif
#endif

static void InitGLExtProcs()
{
# if defined(macintosh)
  glGetPointerv(GL_MAC_GET_PROC_ADDRESS_NV, (GLvoid*) &macGetProcAddress);
# endif

# if !defined(macintosh) && !defined(__linux__)
  /* Retrieve some ARB_multitexture routines. */
  glMultiTexCoord2iARB =
    (PFNGLMULTITEXCOORD2IARBPROC)
    GET_PROC_ADDRESS("glMultiTexCoord2iARB");
  glMultiTexCoord2fvARB =
    (PFNGLMULTITEXCOORD2FVARBPROC)
    GET_PROC_ADDRESS("glMultiTexCoord2fvARB");
  glMultiTexCoord2dvARB =
    (PFNGLMULTITEXCOORD2DVARBPROC)
    GET_PROC_ADDRESS("glMultiTexCoord2dvARB");
  glMultiTexCoord3fARB =
    (PFNGLMULTITEXCOORD3FARBPROC)
    GET_PROC_ADDRESS("glMultiTexCoord3fARB");
  glMultiTexCoord3fvARB =
    (PFNGLMULTITEXCOORD3FVARBPROC)
    GET_PROC_ADDRESS("glMultiTexCoord3fvARB");
 glMultiTexCoord4fARB =
    (PFNGLMULTITEXCOORD4FARBPROC)
    GET_PROC_ADDRESS("glMultiTexCoord4fARB");  
 glMultiTexCoord2fARB =
    (PFNGLMULTITEXCOORD2FARBPROC)
    GET_PROC_ADDRESS("glMultiTexCoord2fARB");  
 glMultiTexCoord1fARB =
    (PFNGLMULTITEXCOORD1FARBPROC)
    GET_PROC_ADDRESS("glMultiTexCoord1fARB");  
  glMultiTexCoord1dARB =
    (PFNGLMULTITEXCOORD1DARBPROC)
    GET_PROC_ADDRESS("glMultiTexCoord1dARB");  
  glActiveTextureARB =
    (PFNGLACTIVETEXTUREARBPROC)
    GET_PROC_ADDRESS("glActiveTextureARB");
# endif

# ifdef _WIN32
#define PROC_ARG (LPCSTR)
# else
#define PROC_ARG (const GLubyte *)
#endif

  if (g_OGLState.HasRegisterCombiners() ) {
    /* Retrieve all NV_register_combiners routines. */
    glCombinerParameterfvNV =
      (PFNGLCOMBINERPARAMETERFVNVPROC)
      GET_PROC_ADDRESS( PROC_ARG "glCombinerParameterfvNV");
    glCombinerParameterivNV =
      (PFNGLCOMBINERPARAMETERIVNVPROC)
      GET_PROC_ADDRESS( PROC_ARG "glCombinerParameterivNV");
    glCombinerParameterfNV =
      (PFNGLCOMBINERPARAMETERFNVPROC)
      GET_PROC_ADDRESS( PROC_ARG "glCombinerParameterfNV");
    glCombinerParameteriNV =
      (PFNGLCOMBINERPARAMETERINVPROC)
      GET_PROC_ADDRESS( PROC_ARG "glCombinerParameteriNV");
    glCombinerInputNV =
      (PFNGLCOMBINERINPUTNVPROC)
      GET_PROC_ADDRESS( PROC_ARG "glCombinerInputNV");
    glCombinerOutputNV =
      (PFNGLCOMBINEROUTPUTNVPROC)
      GET_PROC_ADDRESS( PROC_ARG "glCombinerOutputNV");
    glFinalCombinerInputNV =
      (PFNGLFINALCOMBINERINPUTNVPROC)
      GET_PROC_ADDRESS( PROC_ARG "glFinalCombinerInputNV");
    glGetCombinerInputParameterfvNV =
      (PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC)
      GET_PROC_ADDRESS( PROC_ARG "glGetCombinerInputParameterfvNV");
    glGetCombinerInputParameterivNV =
      (PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC)
      GET_PROC_ADDRESS( PROC_ARG "glGetCombinerInputParameterivNV");
    glGetCombinerOutputParameterfvNV =
      (PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC)
      GET_PROC_ADDRESS( PROC_ARG "glGetCombinerOutputParameterfvNV");
    glGetCombinerOutputParameterivNV =
      (PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC)
      GET_PROC_ADDRESS( PROC_ARG "glGetCombinerOutputParameterivNV");
    glGetFinalCombinerInputParameterfvNV =
      (PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC)
      GET_PROC_ADDRESS( PROC_ARG "glGetFinalCombinerInputParameterfvNV");
    glGetFinalCombinerInputParameterivNV =
      (PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC)
      GET_PROC_ADDRESS( PROC_ARG "glGetFinalCombinerInputParameterivNV");
  }
}

#endif /* __GLSTUFF_H__ */

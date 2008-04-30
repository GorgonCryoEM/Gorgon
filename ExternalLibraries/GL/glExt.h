
/* shadowcast/glstuff.h - OpenGL extension header for shadowcast */

/* Copyright NVIDIA Corporation, 2000, 2001. */

/* $Id$ */

#ifndef __GLEXT_DEFS_H__
#define __GLEXT_DEFS_H__

/* This file contains all the OpenGL extension defines required by the
   shadowcast example (and no other extensions).

   The NV_texture_rectangle extension is supported for GeForce, Quadro,
   and future GPUs with the Release 10 drivers (early 2001).

   The SGIX_shadow and SGIX_depth_texture extensions are not supported
   on GeForce 256, GeForce2, or Quadro GPUs but is implemented on future
   GPUs (early 2001). */

/* GL_GLEXT_VERSION is defined if <GL/gl.h> supports the Linux OpenGL ABI. */
#if defined(GL_GLEXT_VERSION) || defined(_WIN32) || defined(macintosh)
#define NEEDS_GET_PROC_ADDRESS
#endif

#ifdef _WIN32
# include <windows.h>  /* for wglGetProcAddress */
#else
# define APIENTRY
#endif

/* ARB_multitexture defines and prototypes from <GL/gl.h> */
#ifndef GL_ARB_multitexture
#define GL_ACTIVE_TEXTURE_ARB               0x84E0
#define GL_CLIENT_ACTIVE_TEXTURE_ARB        0x84E1
#define GL_MAX_TEXTURE_UNITS_ARB            0x84E2
#define GL_TEXTURE0_ARB                     0x84C0
#define GL_TEXTURE1_ARB                     0x84C1
#define GL_TEXTURE2_ARB                     0x84C2
#define GL_TEXTURE3_ARB                     0x84C3
typedef void (APIENTRY * PFNGLMULTITEXCOORD1DARBPROC) (GLenum target, GLdouble s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1DVARBPROC) (GLenum target, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1FARBPROC) (GLenum target, GLfloat s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1FVARBPROC) (GLenum target, const GLfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1IARBPROC) (GLenum target, GLint s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1IVARBPROC) (GLenum target, const GLint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1SARBPROC) (GLenum target, GLshort s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1SVARBPROC) (GLenum target, const GLshort *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2DARBPROC) (GLenum target, GLdouble s, GLdouble t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2DVARBPROC) (GLenum target, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2FARBPROC) (GLenum target, GLfloat s, GLfloat t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2FVARBPROC) (GLenum target, const GLfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2IARBPROC) (GLenum target, GLint s, GLint t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2IVARBPROC) (GLenum target, const GLint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2SARBPROC) (GLenum target, GLshort s, GLshort t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2SVARBPROC) (GLenum target, const GLshort *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3DARBPROC) (GLenum target, GLdouble s, GLdouble t, GLdouble r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3DVARBPROC) (GLenum target, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3FARBPROC) (GLenum target, GLfloat s, GLfloat t, GLfloat r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3FVARBPROC) (GLenum target, const GLfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3IARBPROC) (GLenum target, GLint s, GLint t, GLint r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3IVARBPROC) (GLenum target, const GLint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3SARBPROC) (GLenum target, GLshort s, GLshort t, GLshort r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3SVARBPROC) (GLenum target, const GLshort *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4DARBPROC) (GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4DVARBPROC) (GLenum target, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4FARBPROC) (GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4FVARBPROC) (GLenum target, const GLfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4IARBPROC) (GLenum target, GLint s, GLint t, GLint r, GLint q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4IVARBPROC) (GLenum target, const GLint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4SARBPROC) (GLenum target, GLshort s, GLshort t, GLshort r, GLshort q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4SVARBPROC) (GLenum target, const GLshort *v);
typedef void (APIENTRY * PFNGLACTIVETEXTUREARBPROC) (GLenum target);
typedef void (APIENTRY * PFNGLCLIENTACTIVETEXTUREARBPROC) (GLenum target);
#endif

/* NV_register_combiners defines and prototypes from <GL/gl.h> */
#ifndef GL_NV_register_combiners
#define GL_REGISTER_COMBINERS_NV            0x8522
#define GL_COMBINER0_NV                     0x8550
#define GL_COMBINER1_NV                     0x8551
#define GL_COMBINER2_NV                     0x8552
#define GL_COMBINER3_NV                     0x8553
#define GL_COMBINER4_NV                     0x8554
#define GL_COMBINER5_NV                     0x8555
#define GL_COMBINER6_NV                     0x8556
#define GL_COMBINER7_NV                     0x8557
#define GL_VARIABLE_A_NV                    0x8523
#define GL_VARIABLE_B_NV                    0x8524
#define GL_VARIABLE_C_NV                    0x8525
#define GL_VARIABLE_D_NV                    0x8526
#define GL_VARIABLE_E_NV                    0x8527
#define GL_VARIABLE_F_NV                    0x8528
#define GL_VARIABLE_G_NV                    0x8529
/*      GL_ZERO */
#define GL_CONSTANT_COLOR0_NV               0x852A
#define GL_CONSTANT_COLOR1_NV               0x852B
/*      GL_FOG */
#define GL_PRIMARY_COLOR_NV                 0x852C
#define GL_SECONDARY_COLOR_NV               0x852D
#define GL_SPARE0_NV                        0x852E
#define GL_SPARE1_NV                        0x852F
/*      GL_TEXTURE0_ARB */
/*      GL_TEXTURE1_ARB */
#define GL_UNSIGNED_IDENTITY_NV             0x8536
#define GL_UNSIGNED_INVERT_NV               0x8537
#define GL_EXPAND_NORMAL_NV                 0x8538
#define GL_EXPAND_NEGATE_NV                 0x8539
#define GL_HALF_BIAS_NORMAL_NV              0x853A
#define GL_HALF_BIAS_NEGATE_NV              0x853B
#define GL_SIGNED_IDENTITY_NV               0x853C
#define GL_SIGNED_NEGATE_NV                 0x853D
#define GL_E_TIMES_F_NV                     0x8531
#define GL_SPARE0_PLUS_SECONDARY_COLOR_NV   0x8532
/*      GL_NONE */
#define GL_SCALE_BY_TWO_NV                  0x853E
#define GL_SCALE_BY_FOUR_NV                 0x853F
#define GL_SCALE_BY_ONE_HALF_NV             0x8540
#define GL_BIAS_BY_NEGATIVE_ONE_HALF_NV     0x8541
#define GL_DISCARD_NV                       0x8530
#define GL_COMBINER_INPUT_NV                0x8542
#define GL_COMBINER_MAPPING_NV              0x8543
#define GL_COMBINER_COMPONENT_USAGE_NV      0x8544
#define GL_COMBINER_AB_DOT_PRODUCT_NV       0x8545
#define GL_COMBINER_CD_DOT_PRODUCT_NV       0x8546
#define GL_COMBINER_MUX_SUM_NV              0x8547
#define GL_COMBINER_SCALE_NV                0x8548
#define GL_COMBINER_BIAS_NV                 0x8549
#define GL_COMBINER_AB_OUTPUT_NV            0x854a
#define GL_COMBINER_CD_OUTPUT_NV            0x854b
#define GL_COMBINER_SUM_OUTPUT_NV           0x854c
#define GL_MAX_GENERAL_COMBINERS_NV         0x854d
#define GL_NUM_GENERAL_COMBINERS_NV         0x854e
#define GL_COLOR_SUM_CLAMP_NV               0x854f
typedef void (APIENTRY * PFNGLCOMBINERPARAMETERFVNVPROC) (GLenum pname, const GLfloat *params);
typedef void (APIENTRY * PFNGLCOMBINERPARAMETERFNVPROC) (GLenum pname, GLfloat param);
typedef void (APIENTRY * PFNGLCOMBINERPARAMETERIVNVPROC) (GLenum pname, const GLint *params);
typedef void (APIENTRY * PFNGLCOMBINERPARAMETERINVPROC) (GLenum pname, GLint param);
typedef void (APIENTRY * PFNGLCOMBINERINPUTNVPROC) (GLenum stage, GLenum portion, GLenum variable, GLenum input, GLenum mapping, GLenum componentUsage);
typedef void (APIENTRY * PFNGLCOMBINEROUTPUTNVPROC) (GLenum stage, GLenum portion, GLenum abOutput, GLenum cdOutput, GLenum sumOutput, GLenum scale, GLenum bias, GLboolean abDotProduct, GLboolean cdDotProduct, GLboolean muxSum);
typedef void (APIENTRY * PFNGLFINALCOMBINERINPUTNVPROC) (GLenum variable, GLenum input, GLenum mapping, GLenum componentUsage);
typedef void (APIENTRY * PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC) (GLenum stage, GLenum portion, GLenum variable, GLenum pname, GLfloat *params);
typedef void (APIENTRY * PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC) (GLenum stage, GLenum portion, GLenum variable, GLenum pname, GLint *params);
typedef void (APIENTRY * PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC) (GLenum stage, GLenum portion, GLenum pname, GLfloat *params);
typedef void (APIENTRY * PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC) (GLenum stage, GLenum portion, GLenum pname, GLint *params);
typedef void (APIENTRY * PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC) (GLenum variable, GLenum pname, GLfloat *params);
typedef void (APIENTRY * PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC) (GLenum variable, GLenum pname, GLint *params);
#endif

/* OpenGL 1.2 defines and prototypes from <GL/gl.h> */
#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE                    0x812F
#endif
#ifndef GL_LIGHT_MODEL_COLOR_CONTROL
#define GL_LIGHT_MODEL_COLOR_CONTROL        0x81F8
#define GL_SINGLE_COLOR                     0x81F9
#define GL_SEPARATE_SPECULAR_COLOR          0x81FA
#endif

/* EXT_texture_env_combine */
#ifndef GL_EXT_texture_env_combine
#define GL_COMBINE_EXT                      0x8570
#define GL_COMBINE_RGB_EXT                  0x8571
#define GL_COMBINE_ALPHA_EXT                0x8572
#define GL_RGB_SCALE_EXT                    0x8573
#define GL_ADD_SIGNED_EXT                   0x8574
#define GL_INTERPOLATE_EXT                  0x8575
#define GL_CONSTANT_EXT                     0x8576
#define GL_PRIMARY_COLOR_EXT                0x8577
#define GL_PREVIOUS_EXT                     0x8578
#define GL_SOURCE0_RGB_EXT                  0x8580
#define GL_SOURCE1_RGB_EXT                  0x8581
#define GL_SOURCE2_RGB_EXT                  0x8582
#define GL_SOURCE0_ALPHA_EXT                0x8588
#define GL_SOURCE1_ALPHA_EXT                0x8589
#define GL_SOURCE2_ALPHA_EXT                0x858A
#define GL_OPERAND0_RGB_EXT                 0x8590
#define GL_OPERAND1_RGB_EXT                 0x8591
#define GL_OPERAND2_RGB_EXT                 0x8592
#define GL_OPERAND0_ALPHA_EXT               0x8598
#define GL_OPERAND1_ALPHA_EXT               0x8599
#define GL_OPERAND2_ALPHA_EXT               0x859A
#endif

/* SGIX_shadow */
#ifndef GL_SGIX_shadow
#define GL_TEXTURE_COMPARE_SGIX             0x819A
#define GL_TEXTURE_COMPARE_OPERATOR_SGIX    0x819B
#define GL_TEXTURE_LEQUAL_R_SGIX            0x819C
#define GL_TEXTURE_GEQUAL_R_SGIX            0x819D
#endif

/* SGIX_depth_texture */
#ifndef GL_SGIX_depth_texture
#define GL_DEPTH_COMPONENT16_SGIX           0x81A5
#define GL_DEPTH_COMPONENT24_SGIX           0x81A6
#define GL_DEPTH_COMPONENT32_SGIX           0x81A7
#endif

/* ARB_texture_border_clamp */
#ifndef GL_ARB_texture_border_clamp
#define GL_CLAMP_TO_BORDER_ARB              0x812D
#endif

/* NV_texture_rectangle */
#ifndef GL_NV_texture_rectangle
#define GL_TEXTURE_RECTANGLE_NV           0x84F5
#define GL_TEXTURE_BINDING_RECTANGLE_NV   0x84F6
#define GL_PROXY_TEXTURE_RECTANGLE_NV     0x84F7
#define GL_MAX_RECTANGLE_TEXTURE_SIZE_NV  0x84F8
#endif

/* EXT_texture_lod_bias */
#ifndef GL_EXT_texture_lod_bias
#define GL_MAX_TEXTURE_LOD_BIAS_EXT       0x84FD
#define GL_TEXTURE_FILTER_CONTROL_EXT     0x8500
#define GL_TEXTURE_LOD_BIAS_EXT           0x8501
#endif

/* WGL_EXT_swap_control defines and prototypes from <GL/gl.h> */
#if defined(_WIN32)
#ifndef WGL_EXT_swap_control
typedef int (APIENTRY * PFNWGLSWAPINTERVALEXTPROC) (int);
typedef int (APIENTRY * PFNWGLGETSWAPINTERVALEXTPROC) (void);
#endif
#endif


#endif /* __GLSTUFF_H__ */

/*
 * Copyright (c) 2017 Rob Clark <rclark@redhat.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sub license,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/**
 * @file common.h
 * @brief Common structures, macros, and function declarations for GBM/EGL/OpenGL ES rendering.
 *
 * This header provides utility macros, type definitions, and function prototypes
 * for initializing and managing GBM (Generic Buffer Management), EGL (Embedded-System Graphics Library),
 * and OpenGL ES resources. It is intended to be used as a shared interface for rendering
 * with DRM/KMS and hardware-accelerated graphics on embedded or desktop Linux systems.
 *
 * The file defines:
 *   - Macros for array sizing and platform compatibility
 *   - Structures for GBM, EGL, and framebuffer management
 *   - Function prototypes for initialization and shader/program utilities
 *   - Mode enumerations for different rendering strategies
 *   - Optional GStreamer video decoding support
 */

#ifndef _COMMON_H
#define _COMMON_H

#ifndef GL_ES_VERSION_2_0
#include <GLES2/gl2.h>
#endif
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <gbm.h>
#include <drm/drm_fourcc.h>
#include <stdbool.h>

/**
 * @def ARRAY_SIZE(arr)
 * @brief Returns the number of elements in a static array.
 */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/* from mesa's util/macros.h: 
#define MIN2( A, B )   ( (A)<(B) ? (A) : (B) )
#define MAX2( A, B )   ( (A)>(B) ? (A) : (B) )
#define MIN3( A, B, C ) ((A) < (B) ? MIN2(A, C) : MIN2(B, C))
#define MAX3( A, B, C ) ((A) > (B) ? MAX2(A, C) : MAX2(B, C))

static inline unsigned
u_minify(unsigned value, unsigned levels)
{
	return MAX2(1, value >> levels);
}
*/

#ifndef DRM_FORMAT_MOD_LINEAR
#define DRM_FORMAT_MOD_LINEAR 0
#endif

#ifndef DRM_FORMAT_MOD_INVALID
#define DRM_FORMAT_MOD_INVALID ((((__u64)0) << 56) | ((1ULL << 56) - 1))
#endif

#ifndef EGL_KHR_platform_gbm
#define EGL_KHR_platform_gbm 1
#define EGL_PLATFORM_GBM_KHR              0x31D7
#endif /* EGL_KHR_platform_gbm */

#ifndef EGL_EXT_platform_base
#define EGL_EXT_platform_base 1
typedef EGLDisplay (EGLAPIENTRYP PFNEGLGETPLATFORMDISPLAYEXTPROC) (EGLenum platform, void *native_display, const EGLint *attrib_list);
typedef EGLSurface (EGLAPIENTRYP PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC) (EGLDisplay dpy, EGLConfig config, void *native_window, const EGLint *attrib_list);
typedef EGLSurface (EGLAPIENTRYP PFNEGLCREATEPLATFORMPIXMAPSURFACEEXTPROC) (EGLDisplay dpy, EGLConfig config, void *native_pixmap, const EGLint *attrib_list);
#ifdef EGL_EGLEXT_PROTOTYPES
EGLAPI EGLDisplay EGLAPIENTRY eglGetPlatformDisplayEXT (EGLenum platform, void *native_display, const EGLint *attrib_list);
EGLAPI EGLSurface EGLAPIENTRY eglCreatePlatformWindowSurfaceEXT (EGLDisplay dpy, EGLConfig config, void *native_window, const EGLint *attrib_list);
EGLAPI EGLSurface EGLAPIENTRY eglCreatePlatformPixmapSurfaceEXT (EGLDisplay dpy, EGLConfig config, void *native_pixmap, const EGLint *attrib_list);
#endif
#endif /* EGL_EXT_platform_base */

#ifndef EGL_VERSION_1_5
#define EGLImage EGLImageKHR
#endif /* EGL_VERSION_1_5 */

#define WEAK __attribute__((weak))

/* Define tokens from EGL_EXT_image_dma_buf_import_modifiers */
#ifndef EGL_EXT_image_dma_buf_import_modifiers
#define EGL_EXT_image_dma_buf_import_modifiers 1
#define EGL_DMA_BUF_PLANE3_FD_EXT         0x3440
#define EGL_DMA_BUF_PLANE3_OFFSET_EXT     0x3441
#define EGL_DMA_BUF_PLANE3_PITCH_EXT      0x3442
#define EGL_DMA_BUF_PLANE0_MODIFIER_LO_EXT 0x3443
#define EGL_DMA_BUF_PLANE0_MODIFIER_HI_EXT 0x3444
#define EGL_DMA_BUF_PLANE1_MODIFIER_LO_EXT 0x3445
#define EGL_DMA_BUF_PLANE1_MODIFIER_HI_EXT 0x3446
#define EGL_DMA_BUF_PLANE2_MODIFIER_LO_EXT 0x3447
#define EGL_DMA_BUF_PLANE2_MODIFIER_HI_EXT 0x3448
#define EGL_DMA_BUF_PLANE3_MODIFIER_LO_EXT 0x3449
#define EGL_DMA_BUF_PLANE3_MODIFIER_HI_EXT 0x344A
#endif

#define NUM_BUFFERS 2

/**
 * @struct gbm
 * @brief Holds GBM device, surface, and buffer objects for rendering.
 *
 * Used to manage the GBM device, window surface, and buffer objects (for surface-less rendering).
 * The format, width, and height describe the pixel format and dimensions.
 */
struct gbm {
	struct gbm_device *dev;              /**< GBM device handle */
	struct gbm_surface *surface;          /**< GBM window surface, or NULL for surface-less */
	struct gbm_bo *bos[NUM_BUFFERS];      /**< Buffer objects for surface-less rendering */
	uint32_t format;                      /**< Pixel format (DRM FourCC) */
	int width, height;                    /**< Dimensions */
};

/**
 * @brief Initialize the GBM device and resources.
 *
 * @param drm_fd DRM file descriptor
 * @param w Width in pixels
 * @param h Height in pixels
 * @param format DRM FourCC pixel format
 * @param modifier Buffer modifier (tiling/compression)
 * @param surfaceless If true, use surface-less mode (no window surface)
 * @return Pointer to initialized gbm struct, or NULL on failure
 */
const struct gbm * init_gbm(int drm_fd, int w, int h, uint32_t format, uint64_t modifier, bool surfaceless);

/**
 * @struct framebuffer
 * @brief Represents an OpenGL framebuffer backed by a GBM buffer object.
 *
 * Contains the EGLImage, OpenGL texture, and framebuffer object handles.
 */
struct framebuffer {
	EGLImageKHR image;   /**< EGLImage created from GBM buffer */
	GLuint tex;          /**< OpenGL texture handle */
	GLuint fb;           /**< OpenGL framebuffer object handle */
};

/**
 * @struct egl
 * @brief Holds EGL display, context, surface, and extension function pointers.
 *
 * Used to manage the EGL context, configuration, and surface, as well as extension
 * function pointers and framebuffer objects for surface-less rendering.
 */
struct egl {
	EGLDisplay display;  /**< EGL display connection */
	EGLConfig config;    /**< EGL configuration */
	EGLContext context;  /**< EGL rendering context */
	EGLSurface surface;  /**< EGL drawing surface */
	struct framebuffer fbs[NUM_BUFFERS];    /**< Framebuffers for surface-less rendering */

	PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT; /**< Extension: platform display */
	PFNEGLCREATEIMAGEKHRPROC eglCreateImageKHR;               /**< Extension: create EGLImage */
	PFNEGLDESTROYIMAGEKHRPROC eglDestroyImageKHR;             /**< Extension: destroy EGLImage */
	PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES; /**< Extension: bind EGLImage to texture */
	PFNEGLCREATESYNCKHRPROC eglCreateSyncKHR;                 /**< Extension: create sync object */
	PFNEGLDESTROYSYNCKHRPROC eglDestroySyncKHR;               /**< Extension: destroy sync object */
	PFNEGLWAITSYNCKHRPROC eglWaitSyncKHR;                     /**< Extension: wait for sync */
	PFNEGLCLIENTWAITSYNCKHRPROC eglClientWaitSyncKHR;         /**< Extension: client wait for sync */
	PFNEGLDUPNATIVEFENCEFDANDROIDPROC eglDupNativeFenceFDANDROID; /**< Extension: duplicate native fence fd */

	/* AMD_performance_monitor extension function pointers (optional) */
/*	PFNGLGETPERFMONITORGROUPSAMDPROC         glGetPerfMonitorGroupsAMD;
	PFNGLGETPERFMONITORCOUNTERSAMDPROC       glGetPerfMonitorCountersAMD;
	PFNGLGETPERFMONITORGROUPSTRINGAMDPROC    glGetPerfMonitorGroupStringAMD;
	PFNGLGETPERFMONITORCOUNTERSTRINGAMDPROC  glGetPerfMonitorCounterStringAMD;
	PFNGLGETPERFMONITORCOUNTERINFOAMDPROC    glGetPerfMonitorCounterInfoAMD;
	PFNGLGENPERFMONITORSAMDPROC              glGenPerfMonitorsAMD;
	PFNGLDELETEPERFMONITORSAMDPROC           glDeletePerfMonitorsAMD;
	PFNGLSELECTPERFMONITORCOUNTERSAMDPROC    glSelectPerfMonitorCountersAMD;
	PFNGLBEGINPERFMONITORAMDPROC             glBeginPerfMonitorAMD;
	PFNGLENDPERFMONITORAMDPROC               glEndPerfMonitorAMD;
	PFNGLGETPERFMONITORCOUNTERDATAAMDPROC    glGetPerfMonitorCounterDataAMD;*/

	bool modifiers_supported; /**< True if EGL_EXT_image_dma_buf_import_modifiers is supported */

	void (*draw)(unsigned i); /**< Optional draw callback for rendering */
};

/**
 * @brief Check if an EGL function pointer is valid (not NULL).
 * @param ptr Function pointer
 * @param name Name of the function
 * @return 0 if valid, -1 if NULL
 */
static inline int __egl_check(void *ptr, const char *name)
{
	if (!ptr) {
		printf("no %s\n", name);
		return -1;
	}
	return 0;
}

#define egl_check(egl, name) __egl_check((egl)->name, #name)

/**
 * @brief Initialize EGL context, surface, and extensions.
 * @param egl Pointer to egl struct to initialize
 * @param gbm Pointer to initialized gbm struct
 * @param samples Number of MSAA samples
 * @return 0 on success, -1 on failure
 */
int init_egl(struct egl *egl, const struct gbm *gbm, int samples);

/**
 * @brief Compile vertex and fragment shaders and create a GL program.
 * @param vs_src Vertex shader source code
 * @param fs_src Fragment shader source code
 * @return GL program handle, or -1 on failure
 */
int create_program(const char *vs_src, const char *fs_src);

/**
 * @brief Link a GL program and check for errors.
 * @param program GL program handle
 * @return 0 on success, -1 on failure
 */
int link_program(unsigned program);

/**
 * @enum mode
 * @brief Rendering modes for different texture and shading strategies.
 */
enum mode {
	SMOOTH,        /**< smooth-shaded */
	RGBA,          /**< single-plane RGBA */
	NV12_2IMG,     /**< NV12, handled as two textures and converted to RGB in shader */
	NV12_1IMG,     /**< NV12, imported as planar YUV EGLImage */
	VIDEO,         /**< video textured cube */
/*	SHADERTOY,        display shadertoy shader */
};

/**
 * @brief Initialize a smooth-shaded cube renderer.
 * @param gbm Pointer to initialized gbm struct
 * @param samples Number of MSAA samples
 * @return Pointer to initialized egl struct, or NULL on failure
 */
const struct egl * init_cube_smooth(const struct gbm *gbm, int samples);

/**
 * @brief Initialize a textured cube renderer.
 * @param gbm Pointer to initialized gbm struct
 * @param mode Rendering mode
 * @param samples Number of MSAA samples
 * @return Pointer to initialized egl struct, or NULL on failure
 */
const struct egl * init_cube_tex(const struct gbm *gbm, enum mode mode, int samples);
/*const struct egl * init_cube_shadertoy(const struct gbm *gbm, const char *shadertoy, int samples);*/

#ifdef HAVE_GST

struct decoder;

/**
 * @brief Initialize a video decoder for use with EGL and GBM.
 * @param egl Pointer to initialized egl struct
 * @param gbm Pointer to initialized gbm struct
 * @param filename Path to video file
 * @return Pointer to decoder struct, or NULL on failure
 */
struct decoder * video_init(const struct egl *egl, const struct gbm *gbm, const char *filename);

/**
 * @brief Get the next video frame as an EGLImage.
 * @param dec Pointer to decoder struct
 * @return EGLImage for the current frame
 */
EGLImage video_frame(struct decoder *dec);

/**
 * @brief Deinitialize and free the video decoder.
 * @param dec Pointer to decoder struct
 */
void video_deinit(struct decoder *dec);

/**
 * @brief Initialize a video-textured cube renderer.
 * @param gbm Pointer to initialized gbm struct
 * @param video Path to video file
 * @param samples Number of MSAA samples
 * @return Pointer to initialized egl struct, or NULL on failure
 */
const struct egl * init_cube_video(const struct gbm *gbm, const char *video, int samples);

#else
static inline const struct egl *
init_cube_video(const struct gbm *gbm, const char *video, int samples)
{
	(void)gbm; (void)video; (void)samples;
	printf("no GStreamer support!\n");
	return NULL;
}
#endif

/*
void init_perfcntrs(const struct egl *egl, const char *perfcntrs);
void start_perfcntrs(void);
void end_perfcntrs(void);
void finish_perfcntrs(void);
void dump_perfcntrs(unsigned nframes, uint64_t elapsed_time_ns);
*/

/**
 * @def NSEC_PER_SEC
 * @brief Number of nanoseconds per second.
 */
#define NSEC_PER_SEC (INT64_C(1000) * USEC_PER_SEC)
#define USEC_PER_SEC (INT64_C(1000) * MSEC_PER_SEC)
#define MSEC_PER_SEC INT64_C(1000)

/**
 * @brief Get the current monotonic time in nanoseconds.
 * @return Time in nanoseconds
 */
int64_t get_time_ns(void);

#endif /* _COMMON_H */

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
 * @file drm-common.h
 * @brief Common DRM/KMS structures and function declarations for display management.
 *
 * This header provides structures and function prototypes for managing DRM (Direct Rendering Manager)
 * resources such as planes, CRTCs, connectors, and framebuffers. It is used to abstract and simplify
 * the process of setting display modes, page flipping, and buffer management for both legacy and atomic
 * DRM APIs.
 */

#ifndef _DRM_COMMON_H
#define _DRM_COMMON_H

#include <xf86drm.h>
#include <xf86drmMode.h>

struct gbm;  ///< Forward declaration for GBM structure
struct egl;  ///< Forward declaration for EGL structure

/**
 * @struct plane
 * @brief Represents a DRM plane (overlay, primary, or cursor plane).
 */
struct plane {
	drmModePlane *plane;                        ///< Pointer to DRM plane object
	drmModeObjectProperties *props;              ///< Properties for the plane
	drmModePropertyRes **props_info;             ///< Array of property info pointers
};

/**
 * @struct crtc
 * @brief Represents a DRM CRTC (display controller).
 */
struct crtc {
	drmModeCrtc *crtc;                          ///< Pointer to DRM CRTC object
	drmModeObjectProperties *props;              ///< Properties for the CRTC
	drmModePropertyRes **props_info;             ///< Array of property info pointers
};

/**
 * @struct connector
 * @brief Represents a DRM connector (output such as HDMI, VGA, etc.).
 */
struct connector {
	drmModeConnector *connector;                ///< Pointer to DRM connector object
	drmModeObjectProperties *props;              ///< Properties for the connector
	drmModePropertyRes **props_info;             ///< Array of property info pointers
};

/**
 * @struct drm
 * @brief Holds DRM device state and configuration for display management.
 *
 * This structure is used to manage the DRM device file descriptor, associated
 * planes, CRTCs, connectors, and mode settings. It also stores framebuffer and
 * synchronization information for rendering and page flipping.
 */
struct drm {
	int fd;                                    ///< DRM device file descriptor

	/* only used for atomic: */
	struct plane *plane;                       ///< Pointer to DRM plane (atomic only)
	struct crtc *crtc;                         ///< Pointer to DRM CRTC (atomic only)
	struct connector *connector;               ///< Pointer to DRM connector (atomic only)
	int crtc_index;                            ///< Index of the CRTC in the resources array
	int kms_in_fence_fd;                       ///< Input fence file descriptor for KMS (atomic only)
	int kms_out_fence_fd;                      ///< Output fence file descriptor for KMS (atomic only)

	drmModeModeInfo *mode;                     ///< Pointer to selected display mode
	uint32_t crtc_id;                          ///< CRTC object ID
	uint32_t connector_id;                     ///< Connector object ID

	/* number of frames to run for: */
	unsigned int count;                        ///< Number of frames to render

	/**
	 * @brief Main rendering loop function pointer.
	 *
	 * This function is called to run the main rendering loop, using the provided
	 * GBM and EGL resources.
	 * @param gbm Pointer to GBM resources
	 * @param egl Pointer to EGL resources
	 * @return 0 on success, negative on error
	 */
	int (*run)(const struct gbm *gbm, const struct egl *egl);
};

/**
 * @struct drm_fb
 * @brief Represents a DRM framebuffer object associated with a GBM buffer object.
 */
struct drm_fb {
	struct gbm_bo *bo;                         ///< Pointer to GBM buffer object
	uint32_t fb_id;                            ///< DRM framebuffer object ID
};

/**
 * @brief Get or create a DRM framebuffer from a GBM buffer object.
 * @param bo Pointer to GBM buffer object
 * @return Pointer to DRM framebuffer struct, or NULL on failure
 */
struct drm_fb * drm_fb_get_from_bo(struct gbm_bo *bo);

/**
 * @brief Initialize DRM device and select mode/connector.
 * @param drm Pointer to DRM device struct to initialize
 * @param device DRM device path (e.g., "/dev/dri/card0"), or NULL for auto-detect
 * @param mode_str Desired mode string (e.g., "1920x1080"), or NULL for default
 * @param vrefresh Desired vertical refresh rate, or 0 for default
 * @param count Number of frames to render
 * @return 0 on success, negative on error
 */
int init_drm(struct drm *drm, const char *device, const char *mode_str, unsigned int vrefresh, unsigned int count);

/**
 * @brief Initialize DRM in legacy (non-atomic) mode.
 * @param device DRM device path
 * @param mode_str Desired mode string
 * @param vrefresh Desired vertical refresh rate
 * @param count Number of frames to render
 * @return Pointer to initialized DRM device struct, or NULL on failure
 */
const struct drm * init_drm_legacy(const char *device, const char *mode_str, unsigned int vrefresh, unsigned int count);

/**
 * @brief Initialize DRM in atomic mode.
 * @param device DRM device path
 * @param mode_str Desired mode string
 * @param vrefresh Desired vertical refresh rate
 * @param count Number of frames to render
 * @return Pointer to initialized DRM device struct, or NULL on failure
 */
const struct drm * init_drm_atomic(const char *device, const char *mode_str, unsigned int vrefresh, unsigned int count);

/*
build a single request (an atomic commit) that describes all 
the changes you want to make: new framebuffers,
plane positions, CRTC settings, etc.
The kernel checks if the entire set of changes is valid and can be applied together. 
If so, all changes are applied simultaneously (atomically) at the next vertical blanking interval (vsync).
If not, nothing is changed and you get an error.
*/

#endif /* _DRM_COMMON_H */

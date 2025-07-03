## [Replaced by this repository](https://github.com/robertkirkman/sm64ex-bbb-doc)

kmscube
=======
Fork of freedesktop kmscube with the bare minimum stripped out (OpenGL ES 3.0+ 
dependent features) to get it to compile and run on the BeagleBone Black's 
PowerVR SGX530 GPU with version 1.17.4948957 of the proprietary userspace libraries.
**many features untested, use at your own risk**.

---

kmscube is a little demonstration program for how to drive bare metal graphics
without a compositor like X11, wayland or similar, using DRM/KMS (kernel mode
setting), GBM (graphics buffer manager) and EGL for rendering content using
OpenGL or OpenGL ES.

The upstream of kmscube is available at https://gitlab.freedesktop.org/mesa/kmscube/

## Key Options

| Short | Long         | Argument         | Description                                                                 |
|-------|--------------|------------------|-----------------------------------------------------------------------------|
| -A    | --atomic     | (none)           | Use atomic mode setting and fencing (modern, robust)                         |
| -c    | --count      | <number>         | Run for the specified number of frames                                      |
| -D    | --device     | <device>         | Use the given DRM device (e.g., `/dev/dri/card0`)                           |
| -f    | --format     | <FOURCC>         | Framebuffer format (e.g., `XRGB`, `ARGB`, `NV12`, etc.)                     |
| -M    | --mode       | <mode>           | Rendering mode: `smooth`, `rgba`, `nv12-2img`, `nv12-1img`                  |
| -m    | --modifier   | <modifier>       | Hardcode the selected buffer modifier (tiling/compression)                  |
| -s    | --samples    | <N>              | Use MSAA (multi-sample anti-aliasing) with N samples                        |
| -V    | --video      | <file>           | Use a video file as a texture on the cube                                   |
| -v    | --vmode      | <mode>[-<freq>]  | Specify the video mode (resolution and optional refresh rate)               |
| -x    | --surfaceless| (none)           | Use surfaceless mode (no GBM surface, direct buffer rendering)              |

## Example Usage Summary

| Command Example                                 | What it does                                 |
|-------------------------------------------------|----------------------------------------------|
| `./kmscube`                                     | Default spinning cube                        |
| `./kmscube --atomic`                            | Use atomic mode setting                       |
| `./kmscube --device=/dev/dri/card1`             | Use a specific DRM device                    |
| `./kmscube --mode=rgba`                         | Use RGBA textured cube                       |
| `./kmscube --video=movie.mp4`                   | Use video as texture (needs GStreamer)       |
| `./kmscube --surfaceless`                       | Use surfaceless rendering mode               |
| `./kmscube --count=60`                          | Run for 60 frames and exit                   |

Notes
You need to run this as a user with access to the DRM device (often root or with appropriate group permissions, e.g., video group).
If you use the --video option, GStreamer support must be enabled at build time.
The application is intended for direct rendering to a display, not for running inside X11/Wayland.
If you see errors about "failed to initialize DRM" or "failed to initialize GBM", check your device path and permissions.
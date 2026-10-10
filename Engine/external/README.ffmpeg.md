# FFmpeg

FFmpeg is an upstream Git submodule in `Engine/external/ffmpeg`, pinned to
`n9.0.2` (`946fcce07b6dcd0331c8cc609192aeff5e1924f8`).
The source checkout is not modified by the SciVibe build.

## Build

After cloning SciVibe:

```sh
git submodule update --init --recursive
```

Build the engine normally with CMake. `Engine/cmake/ScivibeFFmpeg.cmake` drives
FFmpeg's configure/make build and installs its headers, import libraries and
shared libraries under `build/Engine/ffmpeg/install`. No global FFmpeg install
and no `ffmpeg.exe` are needed. The first build takes longer; subsequent builds
reuse the compiled objects. Completed FFmpeg builds are skipped until their
configuration changes. Removing `build` also removes the compiled FFmpeg cache
and requires another full build. Compiler progress is printed during that build.

On Windows, use the project's MinGW UCRT64 preset with MSYS2 `bash` and GNU
`make` installed. CMake locates them relative to the selected compiler.
The cache variables `SCIVIBE_FFMPEG_BASH`, `SCIVIBE_FFMPEG_MAKE` and
`SCIVIBE_FFMPEG_JOBS` allow overriding the build tools and parallelism.
NASM is optional: if found through `SCIVIBE_FFMPEG_NASM` or PATH, optimized
x86 assembly is enabled; otherwise the portable implementations are compiled.
MSVC builds are not currently supported by this integration.

The seven libraries `avcodec`, `avformat`, `avutil`, `avdevice`, `avfilter`,
`swscale` and `swresample` are available through imported CMake targets and
linked to `scivibe`. `scivibe_add_executable()` automatically copies their DLLs
beside Windows applications, including the sandbox.

## Use from C++

```cpp
#include "media/ffmpeg.hpp"

const char* version = av_version_info();
const AVCodec* encoder = avcodec_find_encoder(AV_CODEC_ID_MPEG4);
```

The wrapper supplies `extern "C"` for the FFmpeg headers. Include it explicitly
in the implementation files that need FFmpeg; it is not added to the global PCH.

The build enables the built-in codecs and formats and disables automatic
discovery of third-party libraries, CLI programs and documentation. For example,
the built-in MPEG-4 encoder and MP4 muxer can be used immediately. External
encoders such as `libx264` are not bundled by this change. Consult the upstream
`LICENSE.md` for the license of this configuration.

## Integration check

Configure with `-DSCIVIBE_BUILD_TESTS=ON`, build, then run:

```sh
ctest --test-dir build --output-on-failure
```

`scivibe_ffmpeg_smoke` checks the seven runtime library versions, converts RGBA
pixels to YUV, encodes twelve frames with the built-in MPEG-4 encoder, and writes
`build/tests/ffmpeg-smoke.mp4`. It verifies headers, linking, DLL deployment,
pixel conversion, encoding and MP4 finalization without opening an OpenGL window.
Window capture itself remains a separate engine feature.

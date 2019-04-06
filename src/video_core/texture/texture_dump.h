// Copyright 2019 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once
#include "common/math_util.h"
#include "video_core/renderer_opengl/gl_rasterizer_cache.h"
#include "video_core/texture/texture_decode.h"

namespace Pica::Texture {

/**
 * Dumps a SurfaceType::Texture texture to a PNG file
 * @param source Source pointer to read data from
 * @param info TextureInfo object describing the texture setup
 * @param rect Region to dump
 */
void DumpTextureToPNG(const u8* source, const TextureInfo& info,
                      const Common::Rectangle<u32>& rect);

/**
 * Dumps a SurfaceType::Color texture to a PNG file
 * @param source Source pointer to read data from
 * @param start_offset Offset from source to start dumping from
 * @param pixel_format Pixel format of the texture
 * @param stride Stride of the texture
 * @param width Width of the texture
 * @param height Height of the texture
 * @param flip Texture vertically flipped or not
 */
void DumpColorToPNG(const u8* source, const u32 start_offset,
                    const OpenGL::SurfaceParams::PixelFormat pixel_format, const u32 stride,
                    const u32 width, const u32 height, bool flip = false);

} // namespace Pica::Texture
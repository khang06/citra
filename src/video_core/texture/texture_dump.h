// Copyright 2019 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once
#include "common/math_util.h"
#include "video_core/texture/texture_decode.h"

namespace Pica::Texture {

/**
 * Dumps a texture to a PNG file
 * @param source Source pointer to read data from
 * @param info TextureInfo object describing the texture setup
 * @param rect The region to dump
 */
void DumpTextureToPNG(const u8* source, const TextureInfo& info,
                      const Common::Rectangle<u32>& rect);

} // namespace Pica::Texture
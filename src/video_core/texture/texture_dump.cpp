// Copyright 2019 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include <lodepng.h>
#include "common/file_util.h"
#include "common/hash.h"
#include "core/core.h"
#include "core/hle/kernel/process.h"
#include "video_core/texture/texture_dump.h"

namespace Pica::Texture {

void DumpTextureToPNG(const u8* source, const TextureInfo& info) {
    // Hash the encoded texture
    const u64 tex_hash = Common::ComputeHash64(source, info.stride * info.height);

    // Make destination folder if it doesn't already exist
    const std::string dump_dir =
        fmt::format("{}/textures/{:016X}/", FileUtil::GetUserPath(FileUtil::UserPath::DumpDir),
                    Core::System::GetInstance().Kernel().GetCurrentProcess()->codeset->program_id);
    if (!FileUtil::CreateFullPath(dump_dir)) {
        LOG_ERROR(Render, "Unable to create {}", dump_dir);
        return;
    }

    // Check if the texture was already dumped
    const std::string dump_path = fmt::format("{}/tex1_{}x{}_{:016X}_{}.png", dump_dir, info.width,
                                              info.height, tex_hash, static_cast<u32>(info.format));
    if (FileUtil::Exists(dump_path))
        return;

    // If not, try dumping it
    LOG_INFO(Render, "Dumping texture to {}", dump_path);
    std::vector<u8> rgba8_tex;

    rgba8_tex.resize(info.width * info.height * 4);
    for (unsigned y = 0; y < info.height; ++y) {
        for (unsigned x = 0; x < info.width; ++x) {
            auto vec4 = Pica::Texture::LookupTexture(source, x, y, info);
            const std::size_t offset = (x + (info.width * y)) * 4;
            std::memcpy(&rgba8_tex[offset], vec4.AsArray(), 4);
        }
    }
    unsigned png_error = lodepng::encode(dump_path, rgba8_tex, info.width, info.height);
    if (png_error)
        LOG_ERROR(Render, "Failed to write dumped texture!");
}

} // namespace Pica::Texture
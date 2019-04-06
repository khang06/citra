// Copyright 2019 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include <lodepng.h>
#include "common/color.h"
#include "common/file_util.h"
#include "common/hash.h"
#include "common/math_util.h"
#include "core/core.h"
#include "core/hle/kernel/process.h"
#include "video_core/texture/texture_dump.h"

namespace Pica::Texture {

using PixelFormat = OpenGL::SurfaceParams::PixelFormat;

const std::string MakeDumpDirectory() {
    const std::string dump_dir =
        fmt::format("{}/textures/{:016X}/", FileUtil::GetUserPath(FileUtil::UserPath::DumpDir),
                    Core::System::GetInstance().Kernel().GetCurrentProcess()->codeset->program_id);
    if (!FileUtil::CreateFullPath(dump_dir)) {
        LOG_ERROR(Render, "Unable to create {}", dump_dir);
        return "";
    }
    return dump_dir;
}

const std::string BuildDumpPath(const std::string& dump_dir, const u32 width, const u32 height,
                                const u64 hash, const u32 format) {
    return fmt::format("{}/tex1_{}x{}_{:016X}_{}.png", dump_dir, width, height, hash, format);
}

void DumpTextureToPNG(const u8* source, const TextureInfo& info,
                      const Common::Rectangle<u32>& rect) {
    const std::string dump_dir = MakeDumpDirectory();
    if (dump_dir == "")
        return;

    // Hash the encoded texture
    const u64 tex_hash = Common::ComputeHash64(source, info.stride * info.height);

    // Check if the texture was already dumped
    const std::string dump_path =
        BuildDumpPath(dump_dir, info.width, info.height, tex_hash, static_cast<u32>(info.format));
    if (FileUtil::Exists(dump_path))
        return;

    // If not, try dumping it
    LOG_INFO(Render, "Dumping texture to {}", dump_path);
    std::vector<u8> rgba8_tex;

    rgba8_tex.resize((rect.top - rect.bottom) * (rect.right - rect.left) * 4);
    for (unsigned y = rect.bottom; y < rect.top; ++y) {
        for (unsigned x = rect.left; x < rect.right; ++x) {
            auto vec4 = Pica::Texture::LookupTexture(source, x, y, info);
            const std::size_t offset = (x + (info.width * y)) * 4;
            std::memcpy(&rgba8_tex[offset], vec4.AsArray(), 4);
        }
    }
    unsigned png_error = lodepng::encode(dump_path, rgba8_tex, info.width, info.height);
    if (png_error)
        LOG_ERROR(Render, "Failed to write dumped texture!");
}

void DumpColorToPNG(const u8* source, const u32 start_offset, const PixelFormat pixel_format,
                    const u32 stride, const u32 width, const u32 height, bool flip) {
    const std::string dump_dir = MakeDumpDirectory();
    if (dump_dir == "")
        return;

    // Hash the encoded texture
    const u64 tex_hash = Common::ComputeHash64(source, stride * height);

    // Check if the texture was already dumped
    const std::string dump_path =
        BuildDumpPath(dump_dir, width, height, tex_hash, static_cast<u32>(pixel_format));
    if (FileUtil::Exists(dump_path))
        return;

    // If not, try dumping it
    LOG_INFO(Render, "Dumping texture to {}", dump_path);
    std::vector<u8> rgba8_tex;
    const u32 bpp = OpenGL::CachedSurface::GetFormatBpp(pixel_format);

    rgba8_tex.reserve(width * height * 4);
    for (unsigned y = 0; y < height; ++y) {
        for (unsigned x = 0; x < width; ++x) {
            // TODO: Better variable name
            unsigned y_flip = y;
            if (flip)
                y_flip = height - 1 - y;
            const u8* pixel_ptr = source + start_offset + ((x + y_flip * stride) * (bpp / 8));
            Common::Vec4<u8> color;
            switch (pixel_format) {
            case PixelFormat::RGBA8:
                color = Color::DecodeRGBA8(pixel_ptr);
                rgba8_tex.push_back(color.r());
                rgba8_tex.push_back(color.g());
                rgba8_tex.push_back(color.b());
                rgba8_tex.push_back(color.a());
                break;
            case PixelFormat::RGB8:
                color = Color::DecodeRGB8(pixel_ptr);
                rgba8_tex.push_back(color.r());
                rgba8_tex.push_back(color.g());
                rgba8_tex.push_back(color.b());
                rgba8_tex.push_back(color.a());
                break;
            case PixelFormat::RGB5A1:
                color = Color::DecodeRGB5A1(pixel_ptr);
                rgba8_tex.push_back(color.r());
                rgba8_tex.push_back(color.g());
                rgba8_tex.push_back(color.b());
                rgba8_tex.push_back(color.a());
                break;
            case PixelFormat::RGB565:
                color = Color::DecodeRGB565(pixel_ptr);
                rgba8_tex.push_back(color.r());
                rgba8_tex.push_back(color.g());
                rgba8_tex.push_back(color.b());
                rgba8_tex.push_back(color.a());
                break;
            case PixelFormat::RGBA4:
                color = Color::DecodeRGBA4(pixel_ptr);
                rgba8_tex.push_back(color.r());
                rgba8_tex.push_back(color.g());
                rgba8_tex.push_back(color.b());
                rgba8_tex.push_back(color.a());
                break;
            default:
                LOG_CRITICAL(Render, "Unsupported pixel format {}", static_cast<u32>(pixel_format));
                UNIMPLEMENTED();
            }
        }
    }
    unsigned png_error = lodepng::encode(dump_path, rgba8_tex, width, height);
    if (png_error)
        LOG_ERROR(Render, "Failed to write dumped texture!");
}

} // namespace Pica::Texture
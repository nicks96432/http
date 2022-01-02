#pragma once

#include <filesystem>
#include <string>

namespace phase2
{

    /**
     * @brief Get the MIME type of the file. This function uses
     * libmagic functions to query the MIME type first, and
     * derive the MIME type from the file's extension if libmagic
     * returns generic type like "text/plain" and
     * "application/octet-stream".
     *
     * @param path path to the file.
     * @return The MIME type of the file in string.
     */
    std::string get_mime(const std::filesystem::path &path);

} // namespace phase2

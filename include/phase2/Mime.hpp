#pragma once

#include <filesystem>
#include <string>

namespace phase2
{

	/**
	 * @brief Get the MIME type of the file.
	 * 
	 * @param path path to the file.
	 * @return The MIME type of the file in string.
	 */
    std::string get_mime(const std::filesystem::path &path);

} // namespace phase2

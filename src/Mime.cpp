#include <filesystem>
#include <string>
#include <string_view>

#include <magic.h>

#include <phase2/Mime.hpp>
#ifndef NDEBUG
#include <phase2/utils/Log.hpp>
#endif

namespace phase2
{

	// clang-format off
	/**
	 * @brief Convert the binary file extension to its MIME type.
	 * 
	 * @param ext the extension of the file, without the dot.
	 * @return The MIME type of the file extension.
	 */
    std::string _check_binary_mime(std::string_view ext)
    {
        switch (ext.size())
        {
        case 2:
            switch (ext.front())
            {
			case '7':
				if (ext[1] == 'z')                  return "application/x-7z-compressed";
				break; 
			case 'r':
				if (ext[1] == 'a')                  return "audio/x-realaudio";
				break;
            case 't':
                if (ext[1] == 's')                  return "video/mp2t";
                break;
            default:
                break;
            }
			break;
        case 3:
            switch (ext.front())
            {
			case '3':
				if (ext[1] == 'g')
				{
					if (ext[2] == 'p')              return "video/3gpp";
					if (ext[2] == '2')              return "video/3gpp2";
				}
				break;
            case 'a':
                if (ext[1] == 'a' && ext[2] == 'c') return "audio/aac";
                break;
            case 'b':
				if (ext[1] == 'b' && ext[2] == 'p') return "image/bmp";
                if (ext[1] == 'z' && ext[2] == '2') return "application/x-bzip2";
				break;
			case 'f':
				if (ext[1] == 'l' && ext[2] == 'v') return "video/x-flv";
				break;
            case 'g':
                if (ext[1] == 'i' && ext[2] == 'f') return "image/gif";
				break;
			case 'i':
				if (ext[1] == 'c' && ext[2] == 'o') return "image/x-icon";
				break;
            case 'j':
				if (ext[1] == 'a' && ext[2] == 'r') return "application/java-archive";
                if (ext[1] == 'p' && ext[2] == 'g') return "image/jpeg";
				break;
            case 'm':
				if (ext[1] == '4' && ext[2] == 'a') return "audio/mp4";
                if (ext[1] == 'i' && ext[2] == 'd') return "audio/midi";
				if (ext[1] == 'k' && ext[2] == 'v') return "video/x-matroska";
				if (ext[1] == 'o' && ext[2] == 'v') return "video/quicktime";
                if (ext[1] == 'p')
                {
                    if (ext[2] == '3')              return "audio/mpeg";
                    if (ext[2] == '4')              return "video/mp4";
                }
                break;
            case 'o':
                if (ext[1] == 'g')
                {
                    if (ext[2] == 'a')              return "audio/ogg";
                    if (ext[2] == 'g')              return "vidio/ogg";
                    if (ext[2] == 'v')              return "vidio/ogg";
                }
				break;
			case 'p':
				if (ext[1] == 'd' && ext[2] == 'f') return "application/pdf";
				break;
			case 'r':
				if (ext[1] == 'a' && ext[2] == 'r') return "application/vnd.rar";
				break;
			case 's':
				if (ext[1] == 'w' && ext[2] == 'f') return "application/x-shockwave-flash";
				break;
			case 't':
				if (ext[1] == 'a' && ext[2] == 'r') return "application/x-tar";
				if (ext[1] == 'i' && ext[2] == 'f') return "image/tiff";
				if (ext[1] == 't' && ext[2] == 'f') return "font/ttf";
				break;
			case 'w':
				if (ext[1] == 'a' && ext[2] == 'v') return "audio/wav";
				if (ext[1] == 'm' && ext[2] == 'v') return "video/x-ms-wmv";
				break;
			case 'x':
				if (ext[1] == 'b' && ext[2] == 'm') return "image/xbm";
				break;
			case 'z':
				if (ext[1] == 'i' && ext[2] == 'p') return "application/zip";
				break;
			default:
				break;
            }
			break;
		case 4:
			switch (ext.front())
			{
			case 'a':
				if (ext.substr(1) == "png")         return "image/apng";
				if (ext.substr(1) == "vif")         return "image/avif";
				break;
			case 'e':
				if (ext.substr(1) == "pub")         return "application/epub+zip";
				break;
			case 'j':
				if (ext.substr(1) == "peg")         return "image/jpeg";
				break;
			case 'm':
				if (ext.substr(1) == "idi")         return "audio/midi";
				if (ext.substr(1) == "peg")         return "video/mpeg";
				if (ext.substr(1) == "pkg")         return "application/vnd.apple.installer+xml";
				break;
			case 'o':
				if (ext.substr(1) == "pus")         return "audio/opus";
				break;
			case 't':
				if (ext.substr(1) == "iff")         return "image/tiff";
				break;
			case 'w':
				if (ext[1] == 'e' && ext[2] == 'b')
				{
					if (ext[3] =='a')               return "audio/webm";
					if (ext[3] =='m')               return "video/webm";
					if (ext[3] =='p')               return "audio/webp";
				}
				if (ext.substr(1) == "off")         return "font/woff";
				break;
			default:
				break;
			}
			break;
		case 5:
			if (ext == "woff2")                     return "font/woff2";
			break;
		default:
			break;
        }

		return "";
    }

    /**
     * @brief Convert the text file extension to its MIME type.
     *
     * @param ext the extension of the file, without the dot.
     * @return The MIME type of the file extension.
     */
    std::string _check_text_mime(std::string_view ext)
    {
        switch (ext.size())
        {
		case 2:
			switch(ext.front())
			{
			case 'j':
				if (ext[1] == 's')                  return "application/javascript";
				break;
			case 's':
				if (ext[1] == 'h')                  return "application/x-sh";
				break;
			case 't':
				if (ext[1] == 's')                  return "application/typescript";
				break;
			default:
				break;
			}
			break;
		case 3:
			switch(ext.front())
			{
			case 'c':
				if (ext[1] == 's')
				{
					if (ext[2] == 's')              return "text/css";
					if (ext[2] == 'v')              return "text/csv";
				}
				break;
			case 'h':
				if (ext[1] == 't' && ext[2] == 'm') return "text/html";
				break;
			case 'm':
				if (ext[1] == 'j' && ext[2] == 's') return "application/javascript";
				break;
			case 's':
				if (ext[1] == 'v' && ext[2] == 'v') return "image/svg+xml";
				break;
			case 't':
				if (ext[1] == 'x' && ext[2] == 't') return "text/plain";
				break;
			case 'x':
				if (ext[1] == 'm' && ext[2] == 'l') return "application/xml";
				break;
			}
			break;
		case 4:
			if (ext == "json")                      return "application/json";
			break;
        default:
            break;
        }

		return "";
    }
	// clang-format on

	std::string get_mime(const std::filesystem::path &path)
	{
		::magic_t cookie = ::magic_open(MAGIC_MIME_TYPE);
		if (cookie == nullptr)
		{
#ifndef NDEBUG
			log_error << "get_mime: unable to initialize magic library";
#endif
			return "";
		}

		if (::magic_load(cookie, nullptr) < 0)
		{
#ifndef NDEBUG
			log_error << "get_mime: unable to load default magic database";
#endif
			::magic_close(cookie);
			return "";
		}

		const char *result = ::magic_file(cookie, path.c_str());
		if (result == nullptr)
		{
#ifndef NDEBUG
			log_error << "get_mime: result is NULL";
#endif
			::magic_close(cookie);
			return "";
		}

		std::string result_str = result;
		::magic_close(cookie);

		if (path.has_extension())
		{
			std::string_view extension{path.extension().string()};
			extension.remove_prefix(1);
			std::string result2;
			if (result_str == "application/octet-stream")
			{
#ifndef NDEBUG
				log_debug << "get_mime: result is unknown binary form, start second decision by extension";
#endif
				result2 = _check_binary_mime(extension);
			}
			else if (result_str == "text/plain")
			{
#ifndef NDEBUG
				log_debug << "get_mime: result is unknown text form, start second decision by extension";
#endif
				result2 = _check_text_mime(extension);
			}
			if (!result2.empty())
				result_str = std::move(result2);
		}

		return result_str;
	}

} // namespace phase2

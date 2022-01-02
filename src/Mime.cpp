#include <magic.h>

#include <phase2/Mime.hpp>
#ifndef NDEBUG
#include <phase2/utils/Log.hpp>
#endif

namespace phase2
{

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
        return result_str;
    }

} // namespace phase2

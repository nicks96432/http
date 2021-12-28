#include <cctype>
#include <charconv>
#include <cstdint>
#include <cstring>
#include <functional>
#include <list>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#ifndef NDEBUG
#include <phase2/utils/Log.hpp>
#endif

#include <phase2/Http.hpp>

namespace phase2
{

    HttpHeader::HttpHeader() noexcept : _Headers{}, _version{-1, -1}, _valid{false} {}

    HttpHeader::HttpHeader(std::string_view str, std::optional<std::reference_wrapper<std::size_t>> body_start) noexcept
        : _version{-1, -1}
    {
        const std::string_view::size_type header_end = str.find("\r\n\r\n");
        if (header_end == str.npos)
        {
            this->_valid = false;
            return;
        }
        if (body_start)
            body_start->get() = header_end + 4;
        str.remove_suffix(str.size() - header_end - 2);

        const std::string_view::size_type firstline_end = str.find("\r\n");
        if (firstline_end == str.npos)
        {
            this->_valid = false;
            return;
        }
        str.remove_prefix(firstline_end + 2);

        std::string_view::size_type line_end;
        while ((line_end = str.find("\r\n")) != str.npos)
        {
            std::string_view line             = {str.begin(), line_end};
            std::string_view::size_type colon = line.find(':');
            if (colon == line.npos)
            {
                this->_valid = false;
                return;
            }
            std::string_view::size_type value_begin = line.find_first_not_of(" \t", colon + 1);
            std::string_view::size_type value_end   = line.find_last_not_of(" \t");
            std::string field{line.begin(), colon};
            this->_Headers.emplace(field, std::list<std::string>());
            this->_Headers.at(field).emplace_back(line.substr(value_begin, value_end));
            str.remove_prefix(line_end + 2);
        }

        this->_valid = true;
    }

    HttpHeader::HttpHeader(const std::vector<std::uint8_t> &buf,
                           std::optional<std::reference_wrapper<std::size_t>> body_start) noexcept
        : HttpHeader{std::string_view{reinterpret_cast<const char *>(buf.data()), buf.size()}, body_start} {}

    HttpHeader::HttpHeader(const std::uint8_t *buf, std::size_t size,
                           std::optional<std::reference_wrapper<std::size_t>> body_start) noexcept
        : HttpHeader{std::string_view{reinterpret_cast<const char *>(buf), size}, body_start} {}

    void HttpHeader::addHeader(std::string_view field, std::string_view value)
    {
        std::string field_str{field};
        this->_Headers.emplace(field_str, std::list<std::string>());
        this->_Headers.at(field_str).emplace_back(value);
    }

    std::list<std::string> HttpHeader::getHeader(std::string_view field) const
    {
        std::string field_str{field};
        if (this->_Headers.find(field_str) == this->_Headers.end())
        {
#ifndef NDEBUG
            log_debug << "field " << field << "not found, return empty list";
#endif
            return {};
        }

        return this->_Headers.at(field_str);
    }

    HttpHeader::HttpVersionType HttpHeader::getHttpVersion() const noexcept
    {
        return this->_version;
    }

    bool HttpHeader::isValid() const noexcept
    {
        return this->_valid;
    }

    void HttpHeader::removeHeader(std::string_view field) noexcept
    {
        this->_Headers.erase(std::string{field});
    }

    void HttpHeader::setHttpVersion(const HttpHeader::HttpVersionType &version) noexcept
    {
        this->_version = version;
    }

    void HttpHeader::setHttpVersion(HttpHeader::HttpVersionType &&version) noexcept
    {
        this->_version = std::move(version);
    }

    void HttpHeader::setHttpVersion(int major, int minor) noexcept
    {
        this->_version.first  = major;
        this->_version.second = minor;
    }

    HttpHeader::BufferType HttpHeader::serialize() const noexcept
    {
        if (!*this)
            return HttpRequestHeader::BufferType();
        BufferType buf;
        buf.reserve(256);
        for (const auto &pair : this->_Headers)
            for (std::string_view value : pair.second)
            {
                buf.insert(buf.end(), pair.first.cbegin(), pair.first.cend());
                buf.push_back(':');
                buf.push_back(' ');
                buf.insert(buf.end(), value.begin(), value.end());
                buf.push_back('\r');
                buf.push_back('\n');
            }
        buf.push_back('\r');
        buf.push_back('\n');

        return buf;
    }

    HttpHeader::operator bool() const noexcept
    {
        return this->isValid();
    }

    bool HttpHeader::operator!() const noexcept
    {
        return !this->isValid();
    }

    std::ostream &operator<<(std::ostream &os, std::vector<std::uint8_t> buffer)
    {
        for (const auto &c : buffer)
            os << static_cast<char>(c);
        return os;
    }

    HttpRequestHeader::HttpRequestHeader() noexcept
        : HttpHeader(), _type(HttpRequestHeader::RequestType::UNKNOWN) {}

    HttpRequestHeader::HttpRequestHeader(std::string_view str,
                                         std::optional<std::reference_wrapper<std::size_t>> body_start) noexcept
        : HttpHeader{str, body_start}
    {
        std::string_view::size_type firstline_end = str.find("\r\n");
        if (firstline_end == str.npos)
        {
#ifndef NDEBUG
            log_debug << "invalid HTTP request: cannot find \\r\\n";
#endif
            this->_valid = false;
            return;
        }
        str.remove_suffix(str.size() - firstline_end);

        std::string_view::size_type first_space = str.find(' ');
        if (first_space == str.npos)
        {
#ifndef NDEBUG
            log_debug << "invalid HTTP request: cannot find request type";
#endif
            this->_valid = false;
            return;
        }
        this->_type = phase2::to_type(str.substr(0, first_space));
        str.remove_prefix(first_space + 1);

        std::string_view::size_type second_space = str.find(' ');
        if (second_space == str.npos)
        {
#ifndef NDEBUG
            log_debug << "invalid HTTP request: cannot find url";
#endif
            this->_valid = false;
            return;
        }
        this->setUrl(str.substr(0, second_space));
        str.remove_prefix(second_space + 1);

        this->setHttpVersion(phase2::to_version(str));

        this->_valid = true;
    }

    HttpRequestHeader::HttpRequestHeader(const BufferType &buf,
                                         std::optional<std::reference_wrapper<std::size_t>> body_start) noexcept
        : HttpRequestHeader{std::string_view{reinterpret_cast<const char *>(buf.data()), buf.size()}, body_start} {}

    HttpRequestHeader::HttpRequestHeader(const std::uint8_t *buf, std::size_t size,
                                         std::optional<std::reference_wrapper<std::size_t>> body_start) noexcept
        : HttpRequestHeader{std::string_view{reinterpret_cast<const char *>(buf), size}, body_start} {}

    HttpRequestHeader::RequestType HttpRequestHeader::getType() const noexcept
    {
        return this->_type;
    }

    std::string HttpRequestHeader::getUrl() const noexcept
    {
        return this->_url;
    }

    void HttpRequestHeader::setType(HttpRequestHeader::RequestType type) noexcept
    {
        this->_type = type;
    }

    void HttpRequestHeader::setUrl(std::string_view url) noexcept
    {
        this->_url = url;
    }

    std::ostream &operator<<(std::ostream &os, const HttpRequestHeader &req)
    {
        return os << req.serialize();
    }

    HttpRequestHeader::BufferType HttpRequestHeader::serialize() const noexcept
    {
        if (!*this)
            return HttpRequestHeader::BufferType();
        std::string str{phase2::to_string(this->_type)};
        str += ' ';
        str += this->_url;
        str += ' ';
        str += phase2::to_string(this->_version);
        str += "\r\n";

        HttpRequestHeader::BufferType buf;
        buf.reserve(512);
        buf.insert(buf.end(), str.cbegin(), str.cend());
        HttpRequestHeader::BufferType &&buf2 = HttpHeader::serialize();
        buf.insert(buf.end(), buf2.cbegin(), buf2.cend());
        return buf;
    }

    HttpResponseHeader::HttpResponseHeader() noexcept
        : HttpHeader{}, _status{HttpResponseHeader::StatusCode::unknown} {}

    HttpResponseHeader::HttpResponseHeader(std::string_view str,
                                           std::optional<std::reference_wrapper<std::size_t>> body_start) noexcept
        : HttpHeader{str, body_start}
    {
        std::string_view::size_type firstline_end = str.find("\r\n");
        if (firstline_end == str.npos)
        {
#ifndef NDEBUG
            log_debug << "invalid HTTP request: cannot find \\r\\n";
#endif
            this->_valid = false;
            return;
        }
        str.remove_suffix(str.size() - firstline_end);

        std::string_view::size_type first_space = str.find(' ');
        if (first_space == str.npos)
        {
#ifndef NDEBUG
            log_debug << "invalid HTTP response: cannot find HTTP version";
#endif
            this->_valid = false;
            return;
        }
        this->setHttpVersion(phase2::to_version(str.substr(0, first_space)));
        str.remove_prefix(first_space + 1);

        std::string_view::size_type second_space = str.find(' ');
        if (second_space == str.npos)
        {
#ifndef NDEBUG
            log_debug << "invalid HTTP response: cannot find status code";
#endif
            this->_valid = false;
            return;
        }

        unsigned short status;
        std::from_chars_result result = std::from_chars(str.begin(), str.begin() + second_space, status);
        if (result.ec == std::errc::result_out_of_range)
        {
#ifndef NDEBUG
            log_debug << "invalid HTTP response: statuc code too big";
#endif
            this->_valid = false;
            return;
        }
        if (result.ptr != str.begin() + second_space)
        {
#ifndef NDEBUG
            log_debug << "invalid HTTP response: invalid statuc code";
#endif
            this->_valid = false;
            return;
        }
        this->setStatus(static_cast<HttpResponseHeader::StatusCode>(status));

        this->_valid = true;
    }

    HttpResponseHeader::HttpResponseHeader(const BufferType &buf,
                                           std::optional<std::reference_wrapper<std::size_t>> body_start) noexcept
        : HttpResponseHeader{std::string_view{reinterpret_cast<const char *>(buf.data()), buf.size()}, body_start} {}

    HttpResponseHeader::HttpResponseHeader(const std::uint8_t *buf, std::size_t size,
                                           std::optional<std::reference_wrapper<std::size_t>> body_start) noexcept
        : HttpResponseHeader{std::string_view{reinterpret_cast<const char *>(buf), size}, body_start} {}

    HttpResponseHeader::StatusCode HttpResponseHeader::getStatus() const noexcept
    {
        return this->_status;
    }

    void HttpResponseHeader::setStatus(HttpResponseHeader::StatusCode status) noexcept
    {
        this->_status = status;
    }

    HttpResponseHeader::BufferType HttpResponseHeader::serialize() const noexcept
    {
        if (!*this)
            return HttpRequestHeader::BufferType();
        std::string str = phase2::to_string(this->_version);
        str += ' ';
        str += std::to_string(static_cast<int>(this->getStatus()));
        str += ' ';
        str += phase2::to_string(this->getStatus());
        str += "\r\n";

        HttpResponseHeader::BufferType buf;
        buf.reserve(512);
        buf.insert(buf.end(), str.cbegin(), str.cend());
        HttpResponseHeader::BufferType &&buf2 = HttpHeader::serialize();
        buf.insert(buf.end(), buf2.cbegin(), buf2.cend());
        return buf;
    }

    std::ostream &operator<<(std::ostream &os, const HttpResponseHeader &res)
    {
        return os << res.serialize();
    }

    std::string to_string(const std::pair<int, int> &version)
    {
        std::string str = "HTTP/";
        str += std::to_string(version.first);
        str += '.';
        str += std::to_string(version.second);
        return str;
    }

    // clang-format off
    std::string_view to_string(HttpRequestHeader::RequestType type)
    {
        switch (type)
        {
        case HttpRequestHeader::RequestType::GET:     return "GET";
        case HttpRequestHeader::RequestType::HEAD:    return "HEAD";
        case HttpRequestHeader::RequestType::POST:    return "POST";
        case HttpRequestHeader::RequestType::PUT:     return "PUT";
        case HttpRequestHeader::RequestType::DELETE:  return "DELETE";
        case HttpRequestHeader::RequestType::CONNECT: return "CONNECT";
        case HttpRequestHeader::RequestType::OPTIONS: return "OPTIONS";
        case HttpRequestHeader::RequestType::TRACE:   return "TRACE";
        case HttpRequestHeader::RequestType::PATCH:   return "PATCH";

        default:
            return "UNKNOWN";
        }
    }

    std::string_view to_string(HttpResponseHeader::StatusCode status)
    {
		using StatusCode = HttpResponseHeader::StatusCode;

        switch (status)
        {
		// 1XX
        case StatusCode::continue_:                       return "Continue";
        case StatusCode::switching_protocols:             return "Switching Protocols";
        case StatusCode::early_hints:                     return "Early Hints";

		// 2XX
        case StatusCode::ok:                              return "OK";
        case StatusCode::created:                         return "Created";
        case StatusCode::accepted:                        return "Accepted";
        case StatusCode::non_authoritative_information:   return "Non-Authoritative Information";
        case StatusCode::no_content:                      return "No Content";
        case StatusCode::reset_content:                   return "Reset Content";
        case StatusCode::partial_content:                 return "Partial Content";

		// 3XX
        case StatusCode::multiple_choices:                return "Multiple Choices";
        case StatusCode::moved_permanently:               return "Moved Permanently";
        case StatusCode::found:                           return "Found";
        case StatusCode::see_other:                       return "See Other";
        case StatusCode::not_modified:                    return "not Modified";
        case StatusCode::temporary_redirect:              return "Temporary Redirect";
        case StatusCode::permanent_redirect:              return "Permanent Redirect";

		// 4XX
        case StatusCode::bad_request:                     return "Bad Request";
        case StatusCode::unauthorized:                    return "Unauthorized";
        case StatusCode::payment_required:                return "Payment Required";
        case StatusCode::forbidden:                       return "Forbidden";
        case StatusCode::not_found:                       return "Not Found";
        case StatusCode::method_not_allowed:              return "Method Not Allowed";
        case StatusCode::not_acceptable:                  return "Not Acceptable";
        case StatusCode::proxy_authentication_required:   return "Proxy Authentication Required";
        case StatusCode::request_timeout:                 return "Request Timeout";
        case StatusCode::conflict:                        return "Conflict";
        case StatusCode::gone:                            return "Gone";
        case StatusCode::length_required:                 return "Length Required";
        case StatusCode::precondition_failed:             return "Precondition Failed";
        case StatusCode::payload_too_large:               return "Payload Too Large";
        case StatusCode::uri_too_long:                    return "URI Too Long";
        case StatusCode::unsupported_media_type:          return "Unsupported Media Type";
        case StatusCode::range_not_satisfiable:           return "Range Not Satisfiable";
        case StatusCode::expectation_failed:              return "Expection Failed";
        case StatusCode::unprocessable_entity:            return "Unprocessable Entity";
        case StatusCode::too_early:                       return "Too Early";
        case StatusCode::upgrade_required:                return "Upgrade Required";
        case StatusCode::precondition_required:           return "Precondition Required";
        case StatusCode::too_many_requests:               return "Too Many Requests";
        case StatusCode::request_header_fields_too_karge: return "Request Header Fields Too Large";
        case StatusCode::unavailable_for_legal_reasons:   return "Unavailable For Legal Reasons";

		// 5XX
        case StatusCode::internal_server_error:           return "Internal Server Error";
        case StatusCode::not_implemented:                 return "Not Implemented";
        case StatusCode::bad_gateway:                     return "Bad Gateway";
        case StatusCode::service_unavailable:             return "Service Unavailable";
        case StatusCode::gateway_timeout:                 return "Gateway Timeout";
        case StatusCode::http_version_not_supported:      return "HTTP Version Not Supported";
        case StatusCode::variant_also_negotiates:         return "Variant Also Negotiates";
        case StatusCode::insufficient_storage:            return "Insufficient Storage";
        case StatusCode::loop_detected:                   return "Loop Detected";
        case StatusCode::not_extended:                    return "Not Extended";
        case StatusCode::network_authentication_required: return "Network Authentication Required";

		default:
            return "unknown";
		}
    }
    // clang-format on

    HttpRequestHeader::RequestType to_type(std::string_view str)
    {
        using RequestType = HttpRequestHeader::RequestType;
        if (str.length() < 3)
            return RequestType::UNKNOWN;

        switch (str.at(0))
        {
        case 'G':
            str.remove_prefix(1);
            return str == "ET" ? RequestType::GET : RequestType::UNKNOWN;
        case 'H':
            str.remove_prefix(1);
            return str == "EAD" ? RequestType::HEAD : RequestType::UNKNOWN;
        case 'P':
            str.remove_prefix(1);
            if (str == "OST")
                return RequestType::POST;
            if (str == "UT")
                return RequestType::PUT;
            if (str == "ATCH")
                return RequestType::PATCH;
            return RequestType::UNKNOWN;
        case 'D':
            str.remove_prefix(1);
            return str == "ELETE" ? RequestType::DELETE : RequestType::UNKNOWN;
        case 'C':
            str.remove_prefix(1);
            return str == "ONNECT" ? RequestType::CONNECT : RequestType::UNKNOWN;
        case 'O':
            str.remove_prefix(1);
            return str == "PTIONS" ? RequestType::OPTIONS : RequestType::UNKNOWN;
        case 'T':
            str.remove_prefix(1);
            return str == "RACE" ? RequestType::TRACE : RequestType::UNKNOWN;
        }

        return RequestType::UNKNOWN;
    }

    std::pair<int, int> to_version(std::string_view str)
    {
        std::string tmp;
        tmp.reserve(str.size());
        for (const char &c : str)
            tmp.push_back(std::isalpha(static_cast<unsigned char>(c)) ? std::toupper(static_cast<unsigned char>(c)) : c);
        if (tmp.compare(0, 5, "HTTP/") != 0)
        {
#ifndef NDEBUG
            log_debug << "to_version: invalid HTTP version, default HTTP/1.0";
#endif
            return std::make_pair(-1, -1);
        }

        std::istringstream ss{tmp};
        ss.ignore(5);
        int major, minor;
        if (!(ss >> major))
        {
#ifndef NDEBUG
            log_debug << "to_version: invalid HTTP version";
#endif
            return std::make_pair(-1, -1);
        }
        ss.ignore();
        if (!(ss >> minor))
        {
#ifndef NDEBUG
            log_debug << "to_version: invalid HTTP version";
#endif
            return std::make_pair(-1, -1);
        }

        return std::make_pair(major, minor);
    }

} // namespace phase2

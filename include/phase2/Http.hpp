#pragma once

#include <cstdint>
#include <filesystem>
#include <functional>
#include <optional>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <phase2/Url.hpp>
#include <phase2/utils/HeaderMap.hpp>

namespace phase2
{

	/**
	 * @brief The base object of a HTTP request/response.
	 */
	class HttpHeader
	{
	protected:
		using HttpVersionType = std::pair<int, int>;
		using BufferType      = std::vector<std::uint8_t>;

	public:
		/**
		 * @brief Construct an empty HTTP header.
		 */
		HttpHeader() noexcept;

		/**
		 * @brief Construct a new HTTP header from a string. This constructor
		 * will skip the first line and parse the header fields.
		 *
		 * @param str the string.
		 * @param body_start an optional parameter that stores where the body starts.
		 */
		HttpHeader(std::string_view str,
				   std::optional<std::reference_wrapper<std::size_t>> body_start = {}) noexcept;

		/**
		 * @brief Construct a new HTTP header from a vector buffer. This constructor
		 * will skip the first line and parse the header fields.
		 *
		 * @param buf the buffer.
		 * @param body_start an optional parameter that stores where the body starts.
		 */
		HttpHeader(const BufferType &buf,
				   std::optional<std::reference_wrapper<std::size_t>> body_start = {}) noexcept;

		/**
		 * @brief Construct a new HTTP header from a buffer with the specified size.
		 * This constructor will skip the first line and parse the header fields.
		 * @param buf the buffer.
		 * @param size size of the buffer.
		 * @param body_start an optional parameter that stores where the body starts.
		 */
		HttpHeader(const std::uint8_t *buf, std::size_t size,
				   std::optional<std::reference_wrapper<std::size_t>> body_start = {}) noexcept;

		/**
		 * @brief Add a header to the request/response. If the header exists,
		 * it will replace the old header.
		 *
		 * @param field the header field to add.
		 * @param value the value.
		 */
		void addHeader(std::string_view field, std::string_view value);

		/**
		 * @brief Get values of a field of the request/response header.
		 *
		 * @param field the header field to get.
		 * @return the header strings.
		 */
		std::list<std::string> getHeader(std::string_view field) const;

		/**
		 * @brief Get the HTTP version of the request/response.
		 *
		 * @return a version pair.
		 */
		HttpVersionType getHttpVersion() const noexcept;

		/**
		 * @brief Check whether the request/response is valid or not.
		 *
		 * @return the request/response is valid or not.
		 */
		bool isValid() const noexcept;

		/**
		 * @brief Remove a header from the request/response.
		 *
		 * @param field the header field to remove.
		 */
		void removeHeader(std::string_view field) noexcept;

		/**
		 * @brief Set the HTTP version of the request/response.
		 *
		 * @param version the version pair
		 */
		void setHttpVersion(const HttpVersionType &version) noexcept;

		/**
		 * @brief Set the HTTP version of the request/response.
		 *
		 * @param version the version pair
		 */
		void setHttpVersion(HttpVersionType &&version) noexcept;

		/**
		 * @brief Set the HTTP version of the request/response.
		 *
		 * @param major the major version number.
		 * @param minor the minor version number.
		 */
		void setHttpVersion(int major, int minor) noexcept;

		/**
		 * @brief Convert the headers and the body to a buffer.
		 *
		 * @return the converted buffer.
		 */
		virtual BufferType serialize() const;

		/**
		 * @brief Check whether the request/response is valid or not.
		 *
		 * @return the request/response is valid or not.
		 */
		operator bool() const noexcept;

		/**
		 * @brief Check whether the request/response is invalid or not.
		 *
		 * @return the request/response is invalid or not.
		 */
		bool operator!() const noexcept;

	protected:
		HeaderMap _Headers;
		HttpVersionType _version;
		bool _valid;
	};

	/**
	 * @brief print a vector buffer.
	 *
	 * @param os the output stream.
	 * @param req the buffer.
	 * @return reference of the output stream.
	 */
	std::ostream &operator<<(std::ostream &os, std::vector<std::uint8_t> buffer);

	/**
	 * @brief The HTTP Request object.
	 */
	class HttpRequestHeader : public HttpHeader
	{
	public:
		/**
		 * @brief Request types of a HTTP Request.
		 */
		enum class RequestType
		{
			UNKNOWN = 0,

			GET,
			HEAD,
			POST,
			PUT,
			DELETE,
			CONNECT,
			OPTIONS,
			TRACE,
			PATCH,
		};

		/**
		 * @brief Construct an empty HTTP header.
		 */
		HttpRequestHeader() noexcept;

		/**
		 * @brief Construct a new HTTP request header from a string.
		 *
		 * @param str the string.
		 * @param body_start an optional parameter that stores where the body starts.
		 */
		HttpRequestHeader(std::string_view str,
						  std::optional<std::reference_wrapper<std::size_t>> body_start = {}) noexcept;

		/**
		 * @brief Construct a new HTTP request header from a vector buffer.
		 *
		 * @param buf the buffer.
		 * @param body_start an optional parameter that stores where the body starts.
		 */
		HttpRequestHeader(const BufferType &buf,
						  std::optional<std::reference_wrapper<std::size_t>> body_start = {}) noexcept;

		/**
		 * @brief Construct a new HTTP request header from a buffer with the specified size.
		 *
		 * @param buf the buffer.
		 * @param size size of the buffer.
		 * @param body_start an optional parameter that stores where the body starts.
		 */
		HttpRequestHeader(const std::uint8_t *buf, std::size_t size,
						  std::optional<std::reference_wrapper<std::size_t>> body_start = {}) noexcept;

		/**
		 * @brief Get the type of the HTTP request.
		 *
		 * @return the type of the HTTP request.
		 */
		RequestType getType() const noexcept;

		/**
		 * @brief Get the URL of the HTTP request.
		 *
		 * @return the URL of the HTTP request.
		 */
		Url getUrl() const noexcept;

		/**
		 * @brief Set the type of the request.
		 *
		 * @param type the new request type.
		 */
		void setType(RequestType type) noexcept;

		/**
		 * @brief Set the URL of the HTTP request.
		 *
		 * @param url the new URL.
		 */
		void setUrl(Url url) noexcept;

		/**
		 * @brief Convert the request to a buffer.
		 *
		 * @return the converted buffer.
		 */
		BufferType serialize() const override;

	protected:
		RequestType _type;
		Url _url;
	};

	/**
	 * @brief print a HTTP request header.
	 *
	 * @param os the output stream.
	 * @param req the request header.
	 * @return reference of the output stream.
	 */
	std::ostream &operator<<(std::ostream &os, const HttpRequestHeader &req);

	/**
	 * @brief The HTTP response object.
	 */
	class HttpResponseHeader : public HttpHeader
	{
	public:
		// clang-format off
		/**
		 * @brief Status codes of a HTTP response.
		 */
		enum class StatusCode : unsigned short
		{
			unknown = 0,

			continue_                       = 100,
			switching_protocols             = 101,
			early_hints                     = 103,

			ok                              = 200,
			created                         = 201,
			accepted                        = 202,
			non_authoritative_information   = 203,
			no_content                      = 204,
			reset_content                   = 205,
			partial_content                 = 206,

			multiple_choices                = 300,
			moved_permanently               = 301,
			found                           = 302,
			see_other                       = 303,
			not_modified                    = 304,
			temporary_redirect              = 307,
			permanent_redirect              = 308,

			bad_request                     = 400,
			unauthorized                    = 401,
			payment_required                = 402,
			forbidden                       = 403,
			not_found                       = 404,
			method_not_allowed              = 405,
			not_acceptable                  = 406,
			proxy_authentication_required   = 407,
			request_timeout                 = 408,
			conflict                        = 409,
			gone                            = 410,
			length_required                 = 411,
			precondition_failed             = 412,
			payload_too_large               = 413,
			uri_too_long                    = 414,
			unsupported_media_type          = 415,
			range_not_satisfiable           = 416,
			expectation_failed              = 417,
			unprocessable_entity            = 422,
			too_early                       = 425,
			upgrade_required                = 426,
			precondition_required           = 428,
			too_many_requests               = 429,
			request_header_fields_too_karge = 431,
			unavailable_for_legal_reasons   = 451,

			internal_server_error           = 500,
			not_implemented                 = 501,
			bad_gateway                     = 502,
			service_unavailable             = 503,
			gateway_timeout                 = 504,
			http_version_not_supported      = 505,
			variant_also_negotiates         = 506,
			insufficient_storage            = 507,
			loop_detected                   = 508,
			not_extended                    = 510,
			network_authentication_required = 511,
		};
		// clang-format on

		/**
		 * @brief Construct a new Http Response object.
		 */
		HttpResponseHeader() noexcept;

		/**
		 * @brief Construct a new HTTP response header from a string.
		 *
		 * @param str the string.
		 * @param body_start an optional parameter that stores where the body starts.
		 */
		HttpResponseHeader(std::string_view str,
						   std::optional<std::reference_wrapper<std::size_t>> body_start = {}) noexcept;

		/**
		 * @brief Construct a new HTTP response header from a vector buffer.
		 *
		 * @param buf the buffer.
		 * @param body_start an optional parameter that stores where the body starts.
		 */
		HttpResponseHeader(const BufferType &buf,
						   std::optional<std::reference_wrapper<std::size_t>> body_start = {}) noexcept;

		/**
		 * @brief Construct a new HTTP response header from a buffer with the specified size.
		 *
		 * @param buf the buffer.
		 * @param size size of the buffer.
		 * @param body_start an optional parameter that stores where the body starts.
		 */
		HttpResponseHeader(const std::uint8_t *buf, std::size_t size,
						   std::optional<std::reference_wrapper<std::size_t>> body_start = {}) noexcept;

		/**
		 * @brief Get the status code of the response.
		 */
		StatusCode getStatus() const noexcept;

		/**
		 * @brief Set the status code of the response.
		 *
		 * @param status the new status code.
		 */
		void setStatus(StatusCode status) noexcept;

		/**
		 * @brief Convert the response to a buffer.
		 *
		 * @return the converted buffer.
		 */
		BufferType serialize() const override;

	protected:
		StatusCode _status;
	};

	/**
	 * @brief print a HTTP response header.
	 *
	 * @param os the output stream.
	 * @param res the response header.
	 * @return reference of the output stream.
	 */
	std::ostream &operator<<(std::ostream &os, const HttpResponseHeader &res);

	/**
	 * @brief Convert the HTTP version pair to a string.
	 *
	 * @param version the pair.
	 * @return the converted string.
	 */
	std::string to_string(const std::pair<int, int> &version);

	/**
	 * @brief Convert the HTTP request type to a string.
	 *
	 * @param type the request type.
	 * @return the converted string.
	 */
	std::string_view to_string(HttpRequestHeader::RequestType type);

	/**
	 * @brief Convert the HTTP response status code to a string.
	 *
	 * @param status the status code.
	 * @return the converted string.
	 */
	std::string_view to_string(HttpResponseHeader::StatusCode status);

	/**
	 * @brief Convert the string to a HTTP request type.
	 *
	 * @param str the string.
	 * @return the converted request type.
	 */
	HttpRequestHeader::RequestType to_type(std::string_view str);

	/**
	 * @brief Convert the string to a HTTP version pair.
	 *
	 * @param str the string.
	 * @return the converted version pair.
	 */
	std::pair<int, int> to_version(std::string_view str);

} // namespace phase2

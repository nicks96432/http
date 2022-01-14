#pragma once

#include <filesystem>
#include <string>
#include <string_view>
#include <unordered_map>

namespace phase2
{

	class Url
	{
	public:
		Url() noexcept;

		Url(std::string_view raw);

		void clearParams() noexcept;

		std::string getParam(std::string_view param) const;
		
		bool isValid() const;
		
		void removeParam(std::string_view param);

		void setParam(std::string_view param, std::string_view value);

		std::filesystem::path path() const noexcept;

		void path(const std::filesystem::path &p);

		std::string string() const;

	private:
		bool _valid;
		std::filesystem::path _path;
		std::unordered_map<std::string, std::string> _params;
	};

} // namespace phase2

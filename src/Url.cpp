#include <filesystem>
#include <string>
#include <utility>

#include <phase2/Url.hpp>

#ifndef NDEBUG
#include <phase2/utils/Log.hpp>
#endif

namespace phase2
{

	Url::Url() noexcept : _valid(false) {}

	Url::Url(std::string_view raw)
	{
		std::size_t pos = raw.find('?');
		if (pos == raw.npos)
		{
			this->_path  = raw;
			this->_valid = true;
			return;
		}

		this->_path = raw.substr(0, pos);
		raw.remove_prefix(pos + 1);

		while ((pos = raw.find('&')) != raw.npos)
		{
			std::string_view param = raw.substr(0, pos);
			std::size_t equal      = param.find('=');
			if (equal == param.npos)
			{
				this->_valid = false;
				return;
			}
			this->setParam(param.substr(0, equal), param.substr(equal + 1));
			raw.remove_prefix(pos + 1);
		}

		std::size_t equal = raw.find('=');
		if (equal == raw.npos)
		{
			this->_valid = false;
			return;
		}
		this->setParam(raw.substr(0, equal), raw.substr(equal + 1));
		this->_valid = true;
	}

	void Url::clearParams() noexcept
	{
		this->_params.clear();
	}

	std::string Url::getParam(std::string_view param) const
	{
		auto result = this->_params.find(std::string{param});
		if (result == this->_params.cend())
			return "";

		return result->second;
	}

	bool Url::isValid() const
	{
		return this->_valid;
	}

	void Url::removeParam(std::string_view param)
	{
		auto result = this->_params.find(std::string{param});
		if (result != this->_params.cend())
			this->_params.erase(result);
	}

	void Url::setParam(std::string_view param, std::string_view value)
	{
		this->_params[std::string{param}] = value;
	}

	std::filesystem::path Url::path() const noexcept
	{
		return this->_path;
	}

	void Url::path(const std::filesystem::path &p)
	{
		this->_valid = p.string().find('&') == std::string::npos;
		this->_path  = p;
	}

	std::string Url::string() const
	{
		std::string result = this->_path;
		if (!this->_params.empty())
			result.push_back('?');
		for (const auto &param : this->_params)
		{
			result += param.first;
			result.push_back('=');
			result += param.second;
			result.push_back('&');
		}
		if (this->_params.size() > 1)
			result.pop_back();

		return result;
	}

} // namespace phase2

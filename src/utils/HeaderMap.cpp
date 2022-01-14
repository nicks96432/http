#include <algorithm>
#include <cctype>
#include <functional>
#include <string>

#include <phase2/utils/HeaderMap.hpp>

namespace phase2
{

	bool CaseInsensitiveEqual::operator()(std::string_view lhs, std::string_view rhs) const
	{
		if (lhs.size() != rhs.size())
			return false;
		for (std::size_t i = 0; i < lhs.size(); ++i)
			if (std::tolower(static_cast<unsigned char>(lhs.at(i))) !=
				std::tolower(static_cast<unsigned char>(rhs.at(i))))
				return false;
		return true;
	}

	std::size_t CaseInsensitiveHash::operator()(std::string_view str) const
	{
		std::string tmp;
		tmp.reserve(str.size());
		for (const char &c : str)
			tmp.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
		return std::hash<std::string>()(tmp);
	}

} // namespace phase2

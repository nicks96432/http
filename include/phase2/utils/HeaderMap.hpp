#pragma once

#include <list>
#include <string>
#include <string_view>
#include <unordered_map>

namespace phase2
{

	struct CaseInsensitiveEqual
	{
		bool operator()(std::string_view lhs, std::string_view rhs) const;
	};

	struct CaseInsensitiveHash
	{
		std::size_t operator()(std::string_view str) const;
	};

	using HeaderMap = std::unordered_map<std::string, std::list<std::string>,
										 CaseInsensitiveHash, CaseInsensitiveEqual>;

} // namespace phase2

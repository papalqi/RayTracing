#pragma once
#include <string>

namespace igl
{
	
	inline std::string dirname(const std::string & path);
}


#include <algorithm>

inline std::string igl::dirname(const std::string & path)
{
	if (path == "")
	{
		return std::string("");
	}
	size_t found = path.find_last_of("/\\");
	if (found == std::string::npos)
	{
		return std::string(".");
	}
	else if (found == 0)
	{
		return std::string(path.begin(), path.begin() + 1);
	}
	else if (found == path.length() - 1)
	{
		std::string redo = std::string(path.begin(), path.end() - 1);
		return igl::dirname(redo);
	}
	return std::string(path.begin(), path.begin() + found);
}

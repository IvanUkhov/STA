#ifndef __UTILS_H__
#define __UTILS_H__

#include <string>

#define WHITE_SPACES " \t\f\v\n\r"

class Utils
{
	public:

	static std::string strip(const std::string &line)
	{
		std::string out;

		size_t found;

		found = line.find_first_not_of(WHITE_SPACES);
		if (found != std::string::npos) out = line.substr(found);

		found = out.find_last_not_of(WHITE_SPACES);
		if (found != std::string::npos) out.erase(found + 1);
		else out.clear();

		return out;
	}
};

#endif

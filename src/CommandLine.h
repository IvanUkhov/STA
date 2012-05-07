#ifndef __COMMAND_LINE_H__

#include <string>
#include <stdexcept>

class CommandLine
{
	public:

	CommandLine()
	{
	}

	void parse(int argc, const char **argv)
	{
		std::string name;
		bool found = false;

		for (size_t i = 1; i < argc; i++) {
			std::string token(argv[i]);

			bool empty = token.empty();

			if (!empty && token[0] == '-') {
				if (found) process(name, "");

				name = token.substr(1);

				if (name.empty())
					throw std::runtime_error("The name of one of the arguments is empty.");

				found = true;
			}
			else if (empty && !found) continue;
			else {
				process(name, token);

				found = false;
			}
		}

		if (found) process(name, "");

		verify();
	}

	virtual void usage() const
	{
	}

	protected:

	virtual void process(const std::string &name, const std::string &value) = 0;

	virtual void verify() const
	{
	}
};

#endif

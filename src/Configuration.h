#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include <sstream>
#include <string>
#include <vector>

#include "matrix.h"

struct Parameter
{
	std::string name;
	std::string value;

	Parameter() {}
	Parameter(const std::string &_name, const std::string &_value) :
		name(_name), value(_value) {}

	template<class T>
	T convert() const
	{
		std::stringstream stream(value);

		T _value;
		stream >> _value;

		return _value;
	}

	double to_double() const
	{
		return convert<double>();
	}

	int to_int() const
	{
		return convert<int>();
	}

	bool to_bool() const
	{
		return convert<bool>();
	}

	matrix_t to_matrix() const;
	void to_matrix(matrix_t &matrix) const;
	vector_t to_vector() const;
	void to_vector(vector_t &vector) const;
};

class Configuration: public std::vector<Parameter>
{
	public:

	Configuration() {}
	Configuration(const std::string &filename);

	void append(const std::string &name, const std::string &value);
	void update(const std::string &filename);
	void update(std::istream &main_stream);
};

std::ostream &operator<< (std::ostream &o, const Parameter &parameter);
std::ostream &operator<< (std::ostream &o, const Configuration &parameters);

#endif

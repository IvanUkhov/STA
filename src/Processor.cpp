#include <iomanip>

#include "Processor.h"

std::ostream &operator<< (std::ostream &o, const Processor *processor)
{
	o	<< std::setw(4) << processor->get_id()
		<< " ( " << std::setw(8) << processor->get_size() << " )" << std::endl;

	return o;
}

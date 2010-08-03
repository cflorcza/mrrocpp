/*!
 * \file vsp_error.h
 * \brief File containing class representing VSP errors.
 * \date 30.11.2006
 * \author tkornuta <tkornuta@ia.pw.edu.pl>, Warsaw University of Technology
 * \defgroup VSP -- Virtual Sensor Process
 */

#if !defined(_VSP_ERROR_H)
#define _VSP_ERROR_H

#include <exception>

namespace mrrocpp {
namespace vsp {
namespace common {


/*!
 * \class vsp_error
 * \brief Class representing the exceptions thrown and handled in VSP.
 * \author tkornuta
 */
class vsp_error : public std::exception
{
public:
	/** Error class. */
	const lib::error_class_t error_class;

	/** Error number. */
	const uint64_t error_no;

	/*!
	 * Constructor.
	 * @param err_cl Error class.
	 * @param err_no Error number.
	 */
	vsp_error(lib::error_class_t err_cl, uint64_t err_no) :
		std::exception(), error_class(err_cl), error_no(err_no)
	{
	}
};

} // namespace common
} // namespace vsp
} // namespace mrrocpp

#endif
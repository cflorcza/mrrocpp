// -------------------------------------------------------------------------
//                            ecp.h dla QNX6
// Deklaracje struktur danych i metod dla procesow ECP z wykrozsytsaniem sily
// 
// -------------------------------------------------------------------------

#if !defined(_ECP_GEN_TEST_CONV_H)
#define _ECP_GEN_TEST_CONV_H

#include "common/com_buf.h"		// trajectory_description

#include "ecp/common/ecp_generator.h"

namespace mrrocpp {
namespace ecp {
namespace conveyor {

// --------------------------------------------------------------------------
// Generator trajektorii prostoliniowej dla zadan yoyka z wodzeniem za nos
class y_simple_generator : public common::ecp_generator {

protected:

	
public:	
	trajectory_description td;
	int step_no;
	double delta[6];
	
	// konstruktor
	y_simple_generator(common::ecp_task& _ecp_task, int step=0);  
	
	virtual bool first_step ();

	virtual bool next_step ();

}; // end:
// --------------------------------------------------------------------------

} // namespace conveyor
} // namespace ecp
} // namespace mrrocpp

#endif

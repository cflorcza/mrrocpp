#if !defined(_ECP_T_SMB_SWARM_DEMO_SINGLE_AGENT_H)
#define _ECP_T_SMB_SWARM_DEMO_SINGLE_AGENT_H

#include "base/ecp/ecp_task.h"

namespace mrrocpp {
namespace ecp {
namespace smb {
namespace task {

class swarmitfix : public common::task::_task <ecp::smb::robot>
{
public:
	// KONSTRUKTORY
	swarmitfix(lib::configurator &_config);

	// methods for ECP template to redefine in concrete classes
	void mp_2_ecp_next_state_string_handler(void);
};

}
} // namespace irp6p
} // namespace ecp
} // namespace mrrocpp

#endif
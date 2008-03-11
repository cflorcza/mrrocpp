#include "common/impconst.h"
#include "common/com_buf.h"

#include "lib/mis_fun.h"
#include "lib/srlib.h"
#include "mp/mp_r_conveyor.h"

mp_conveyor_robot::mp_conveyor_robot(mp_task &mp_object_l) :
	mp_irp6s_and_conv_robot(ROBOT_CONVEYOR, "[ecp_conveyor]", mp_object_l)
{
}

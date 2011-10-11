/*!
 * \file festo_and_inputs.cc
 * \brief File containing the festo and inputs class methods
 *
 * \author yoyek
 * \date 2011
 *
 */

#include "base/edp/edp_e_motor_driven.h"
#include "const_smb.h"
#include "edp_e_smb.h"
#include "../canopen/gateway_epos_usb.h"
#include "../canopen/gateway_socketcan.h"
#include "../festo/cpv.h"
#include "../maxon/epos.h"
#include "festo_and_inputs.h"
#include "exceptions.h"

namespace mrrocpp {
namespace edp {
namespace smb {

festo_and_inputs::festo_and_inputs(effector &_master) :
		master(_master), epos_di_node(master.epos_di_node), cpv10(master.cpv10), robot_test_mode(master.robot_test_mode)
{
	if (!(master.robot_test_mode)) {
		festo::U32 DeviceType = cpv10->readDeviceType();
		printf("Device type = 0x%08X\n", DeviceType);

		festo::U8 ErrorRegister = cpv10->readErrorRegister();
		printf("Error register = 0x%02X\n", ErrorRegister);

		festo::U32 ManufacturerStatusRegister = cpv10->readManufacturerStatusRegister();
		printf("Manufacturer status register = 0x%08X\n", ManufacturerStatusRegister);

		uint8_t NumberOfOutputGroups = cpv10->readNumberOf8OutputGroups();
		printf("Number of 8-output groups = %d\n", NumberOfOutputGroups);

		uint8_t Outputs07 = cpv10->readOutputs(1);
		printf("Status of outputs 0..7 = 0x%02x\n", Outputs07);

		master.gateway->SendNMTService(10, canopen::gateway::Start_Remote_Node);

		read_state();
	}
}

festo_and_inputs::~festo_and_inputs()
{

}

bool festo_and_inputs::is_upper_halotron_avtive(int leg_number)
{
	return epos_inputs[2 * leg_number + 11];
}

bool festo_and_inputs::is_lower_halotron_avtive(int leg_number)
{
	return epos_inputs[2 * leg_number + 10];
}

bool festo_and_inputs::is_attached(int leg_number)
{
	// to be implemented
	return false;
}

void festo_and_inputs::set_detach(int leg_number, bool value)
{
	switch (leg_number)
	{
		case 1: {
			//	group_one_desired_output[2] = value;
		}

			break;
		case 2: {

		}

			break;
		case 3: {

		}

			break;

		default:
			throw NonFatal_error_2(INVALID_MOTION_PARAMETERS);
			break;

	}
}

void festo_and_inputs::set_move_up(int leg_number, bool value)
{
	switch (leg_number)
	{
		case 1: {
			desired_output[FESTO_CY11_GROUP][FESTO_CY11_BIT_TO_SET] = value;
		}

			break;
		case 2: {
			desired_output[FESTO_CY21_GROUP][FESTO_CY21_BIT_TO_SET] = value;
		}

			break;
		case 3: {
			desired_output[FESTO_CY31_GROUP][FESTO_CY31_BIT_TO_SET] = value;
		}

			break;

		default:
			throw NonFatal_error_2(INVALID_MOTION_PARAMETERS);
			break;

	}
}

void festo_and_inputs::set_move_down(int leg_number, bool value)
{
	switch (leg_number)
	{
		case 1: {
			desired_output[FESTO_CY12_GROUP][FESTO_CY12_BIT_TO_SET] = value;
		}

			break;
		case 2: {
			desired_output[FESTO_CY22_GROUP][FESTO_CY22_BIT_TO_SET] = value;
		}

			break;
		case 3: {
			desired_output[FESTO_CY32_GROUP][FESTO_CY32_BIT_TO_SET] = value;
		}

			break;

		default:
			throw NonFatal_error_2(INVALID_MOTION_PARAMETERS);
			break;

	}
}

void festo_and_inputs::set_clean(int leg_number, bool value)
{
	switch (leg_number)
	{
		case 1: {
			//	group_one_desired_output[2] = value;
		}

			break;
		case 2: {

		}

			break;
		case 3: {

		}

			break;

		default:
			throw NonFatal_error_2(INVALID_MOTION_PARAMETERS);
			break;

	}
}

void festo_and_inputs::determine_legs_state()
{
	if (!(master.robot_test_mode)) {

		int number_of_legs_up = 0;

		for (int i = 0; i < lib::smb::LEG_CLAMP_NUMBER; i++) {

			if (is_upper_halotron_avtive(i)) {
				number_of_legs_up++;
			}
		}

		switch (number_of_legs_up)
		{
			case 0:

				current_legs_state = lib::smb::ALL_DOWN;
				break;
			case 1:
				current_legs_state = lib::smb::ONE_UP_TWO_DOWN;
				break;
			case 2:
				current_legs_state = lib::smb::TWO_UP_ONE_DOWN;
				break;
			case 3:
				current_legs_state = lib::smb::ALL_UP;
				break;
			default:
				break;

		}

	}
}

/*--------------------------------------------------------------------------*/

void festo_and_inputs::festo_command()
{
	std::stringstream ss(std::stringstream::in | std::stringstream::out);

	master.msg->message("FESTO");
	lib::smb::festo_command_td festo_command;

	memcpy(&festo_command, &(master.ecp_edp_cbuffer.festo_command), sizeof(festo_command));

	if (master.robot_test_mode) {
		ss << festo_command.leg[2];

		master.msg->message(ss.str().c_str());
	}

	// determine next_legs_state by counting numebr of legs to be up
	int number_of_legs_up = 0;
	for (int i = 0; i < lib::smb::LEG_CLAMP_NUMBER; i++) {
		if (festo_command.leg[i] == lib::smb::UP) {
			number_of_legs_up++;
		}

	}

	switch (number_of_legs_up)
	{
		case 0:

			next_legs_state = lib::smb::ALL_DOWN;
			break;
		case 1:
			next_legs_state = lib::smb::ONE_UP_TWO_DOWN;
			break;
		case 2:
			next_legs_state = lib::smb::TWO_UP_ONE_DOWN;
			break;
		case 3:
			next_legs_state = lib::smb::ALL_UP;
			break;
		default:
			break;

	}

	// checks if the next_legs_state is valid taking into account current_legs_state
	// and prepares detailed command for festo hardware

	switch (next_legs_state)
	{
		case lib::smb::ALL_DOWN:
			festo_command_all_down(festo_command);
			break;
		case lib::smb::ONE_UP_TWO_DOWN:
			festo_command_one_up_two_down(festo_command);
			break;
		case lib::smb::TWO_UP_ONE_DOWN:
			festo_command_two_up_one_down(festo_command);
			break;
		case lib::smb::ALL_UP:
			festo_command_all_up(festo_command);
			break;
		default:
			break;

	}
	if (!master.robot_test_mode) {
		execute_command();
	}
}

void festo_and_inputs::festo_command_all_down(lib::smb::festo_command_td& festo_command)
{
	switch (current_legs_state)
	{
		case lib::smb::ALL_DOWN:
			BOOST_THROW_EXCEPTION(mrrocpp::edp::smb::nfe_invalid_command_in_given_state() << current_state(current_legs_state) << retrieved_festo_command(lib::smb::ALL_DOWN));
			break;
		case lib::smb::ONE_UP_TWO_DOWN:
			festo_test_mode_set_reply(festo_command);

			for (int i = 0; i < lib::smb::LEG_CLAMP_NUMBER; i++) {
				set_move_down(i + 1, true);
				set_move_up(i + 1, false);
			}

			break;
		case lib::smb::TWO_UP_ONE_DOWN:
			festo_test_mode_set_reply(festo_command);
			for (int i = 0; i < lib::smb::LEG_CLAMP_NUMBER; i++) {
				set_move_down(i + 1, true);
				set_move_up(i + 1, false);
			}
			break;
		case lib::smb::ALL_UP:
			festo_test_mode_set_reply(festo_command);
			for (int i = 0; i < lib::smb::LEG_CLAMP_NUMBER; i++) {
				set_move_down(i + 1, true);
				set_move_up(i + 1, false);
			}
			break;
		default:
			break;

	}
}

void festo_and_inputs::festo_command_one_up_two_down(lib::smb::festo_command_td& festo_command)
{
	BOOST_THROW_EXCEPTION(mrrocpp::edp::smb::nfe_invalid_command_in_given_state()<<current_state(current_legs_state) << retrieved_festo_command(lib::smb::ONE_UP_TWO_DOWN));
}

void festo_and_inputs::festo_command_two_up_one_down(lib::smb::festo_command_td& festo_command)
{
	switch (current_legs_state)
	{
		case lib::smb::ALL_DOWN: {
			festo_test_mode_set_reply(festo_command);
		}

			break;
		case lib::smb::ONE_UP_TWO_DOWN:
		case lib::smb::TWO_UP_ONE_DOWN:
		case lib::smb::ALL_UP:
			BOOST_THROW_EXCEPTION(mrrocpp::edp::smb::nfe_invalid_command_in_given_state()<<current_state(current_legs_state) << retrieved_festo_command(lib::smb::TWO_UP_ONE_DOWN));
			break;
		default:
			break;

	}
}

void festo_and_inputs::festo_command_all_up(lib::smb::festo_command_td& festo_command)
{
	switch (current_legs_state)
	{
		case lib::smb::ALL_DOWN: {
			festo_test_mode_set_reply(festo_command);
			for (int i = 0; i < lib::smb::LEG_CLAMP_NUMBER; i++) {
				set_move_down(i + 1, false);
				set_move_up(i + 1, true);
			}
		}

			break;
		case lib::smb::ONE_UP_TWO_DOWN:
		case lib::smb::TWO_UP_ONE_DOWN:
		case lib::smb::ALL_UP:
			BOOST_THROW_EXCEPTION(mrrocpp::edp::smb::nfe_invalid_command_in_given_state()<<current_state(current_legs_state) << retrieved_festo_command(lib::smb::ALL_UP));
			break;
		default:
			break;

	}
}

void festo_and_inputs::festo_test_mode_set_reply(lib::smb::festo_command_td& festo_command)
{
	if (robot_test_mode) {
		for (int i = 0; i < lib::smb::LEG_CLAMP_NUMBER; i++) {
			if (festo_command.leg[i] == lib::smb::UP) {
				master.edp_ecp_rbuffer.multi_leg_reply.leg[i].is_up = true;
				master.edp_ecp_rbuffer.multi_leg_reply.leg[i].is_down = false;
			} else {
				master.edp_ecp_rbuffer.multi_leg_reply.leg[i].is_up = false;
				master.edp_ecp_rbuffer.multi_leg_reply.leg[i].is_down = true;
			}

		}
	}
}

void festo_and_inputs::read_state()
{
	if (!(master.robot_test_mode)) {
		epos_inputs = epos_di_node->readDInput();

		current_output[1] = cpv10->readOutputs(1);
		desired_output[1] = current_output[1];
		//	std::cout << "group_one_desired_output 1= " << group_one_desired_output << std::endl;
		current_output[2] = cpv10->readOutputs(2);
		//	std::cout << "group_two_current_output 1= " << group_two_current_output << std::endl;
		desired_output[2] = current_output[2];
	}
}

void festo_and_inputs::execute_command()
{
	cpv10->writeOutputs(1, (uint8_t) desired_output[1].to_ulong());
	cpv10->writeOutputs(2, (uint8_t) desired_output[2].to_ulong());
}

} // namespace smb
} // namespace edp
} // namespace mrrocpp


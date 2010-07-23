#include "robot/sarkofag/edp_e_sarkofag.h"
#include "robot/sarkofag/hi_sarkofag.h"

//#include "hi_sarkofag.h"

#include <exception>
#include <stdexcept>
#include <string.h>
#include <iostream>

namespace mrrocpp {
namespace edp {
namespace sarkofag {

HI_moxa::HI_moxa() {
#ifdef T_INFO_FUNC
	std::cout << "[func] Hi, Moxa!" << std::endl;
#endif
}

HI_moxa::~HI_moxa() {
#ifdef T_INFO_FUNC
	std::cout << "[func] Bye, Moxa!" << std::endl;
#endif
	for (unsigned int i = 0; i < 8; i++) {
		if (fd[i] > 0) {
			tcsetattr(fd[i], TCSANOW, &oldtio[i]);
			close(fd[i]);
		}
	}
}

void HI_moxa::init() {

	std::string port = PORT;

#ifdef T_INFO_FUNC
	std::cout << "[func] HI_moxa::init()" << std::endl;
#endif

	for (unsigned int i = 0; i < 8; i++) {
		std::cout << "[info] opening port : "
				<< (port + (char) (i + 50)).c_str();
		fd[i] = open((port + (char) (i + 50)).c_str(), O_RDWR | O_NOCTTY
				| O_NDELAY);
		if (fd[i] < 0) {
			//	throw(std::runtime_error("unable to open device!!!"));
			std::cout << std::endl << "[error] fd == " << (int) fd[i]
					<< std::endl;
		} else
			std::cout << "...OK (" << (int) fd[i] << ")" << std::endl;

		tcgetattr(fd[i], &oldtio[i]);

		// set up new settings
		struct termios newtio;
		memset(&newtio, 0, sizeof(newtio));
		newtio.c_cflag = CS8 | CLOCAL | CREAD | CSTOPB;
		newtio.c_iflag = INPCK; //IGNPAR;
		newtio.c_oflag = 0;
		newtio.c_lflag = 0;
		if (cfsetispeed(&newtio, BAUD) < 0 || cfsetospeed(&newtio, BAUD) < 0) {
			tcsetattr(fd[i], TCSANOW, &oldtio[i]);
			close(fd[i]);
			fd[i] = -1;
			throw(std::runtime_error("unable to set baudrate !!!"));
			return;
		}
		// activate new settings
		tcflush(fd[i], TCIFLUSH);
		tcsetattr(fd[i], TCSANOW, &newtio);
	}

	clock_gettime(CLOCK_MONOTONIC, &wake_time);
}

void HI_moxa::insert_set_value(int drive_number, double set_value) {
#ifdef T_INFO_FUNC
	std::cout << "[func] HI_moxa::insert_set_value(" << drive_number << ", " << set_value << ")" << std::endl;
#endif

	buf[0] = 0x00;
	buf[1] = 0x00;
	buf[2] = 0x00;
	buf[3] = 0x00;
	buf[4] = START_BYTE;
	buf[5] = SET_PWM;
	struct pwm_St* temp = (pwm_St*) &buf[6];
	temp->pwm = set_value / 0.190;

#ifdef T_INFO_CALC
	std::cout << "[calc] pwm: (" << temp->pwm << ")" << std::endl;
#endif
}

int HI_moxa::get_current(int drive_number) {
	int ret;

	ret = sarkofag_status.current;

#ifdef T_INFO_FUNC
	std::cout << "[func] HI_moxa::get_current(" << drive_number << ") = " << ret << std::endl;
#endif
	return ret;
	//return 0;
}

double HI_moxa::get_increment(int drive_number) {
	static int32_t oldPosition;
	double ret;

	ret = (double) (sarkofag_status.position - oldPosition);
	oldPosition = sarkofag_status.position;

#ifdef T_INFO_FUNC
	std::cout << "[func] HI_moxa::get_increment(" << drive_number << ") = " << ret << std::endl;
#endif
	return ret;
}

long int HI_moxa::get_position(int drive_number) {
	int ret;

	ret = sarkofag_status.position;

#ifdef T_INFO_FUNC
	std::cout << "[func] HI_moxa::get_position(" << drive_number << ") = " << ret << std::endl;
#endif
	return ret;
}

uint64_t HI_moxa::read_write_hardware(void) {
	static int64_t receive_attempts = 0, receive_timeouts = 0;
	unsigned int dlen = 0;
	fd_set rfds;

	write(fd[0], buf, WRITE_BYTES);

	receive_attempts++;
	for (int i = 0; (i < READ_BYTES && dlen < READ_BYTES); i++) {
		FD_ZERO(&rfds);
		FD_SET(fd[0], &rfds);

		// timeout
		struct timeval timeout;
		timeout.tv_sec = (time_t) 0;
		timeout.tv_usec = 500;

		int select_retval = select(fd[0] + 1, &rfds, NULL, NULL, &timeout);

		if (select_retval == 0) {
			//throw(std::runtime_error("communication timeout !!!"));
			std::cout << "[error] communication timeout ("
					<< ++receive_timeouts << "/" << receive_attempts << "="
					<< (float) (receive_timeouts / receive_attempts) << ")"
					<< std::endl;
			break;
		} else {
			dlen += read(fd[0], (char*) (&sarkofag_status) + dlen, READ_BYTES
					- dlen);
		}
	}

	//usleep(1000);

	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &wake_time, NULL);

	while ((wake_time.tv_nsec += COMMCYCLE_TIME_NS) > 1000000000) {
		wake_time.tv_sec += 1;
		wake_time.tv_nsec -= 1000000000;
	}

}

void HI_moxa::reset_counters(void) {
#ifdef T_INFO_FUNC
	std::cout << "[func] HI_moxa::reset_counters" << std::endl;
#endif
}

void HI_moxa::start_synchro(int drive_number) {
#ifdef T_INFO_FUNC
	std::cout << "[func] HI_moxa::start_synchro(" << drive_number << ")" << std::endl;
#endif
}

void HI_moxa::finish_synchro(int drive_number) {
#ifdef T_INFO_FUNC
	std::cout << "[func] HI_moxa::finish_synchro(" << drive_number << ")" << std::endl;
#endif
}

bool HI_moxa::is_impulse_zero(int drive_number) {
#ifdef T_INFO_FUNC
	std::cout << "[func] HI_moxa::is_impulse_zero(" << drive_number << ")" << std::endl;
#endif
	return true;
}

void HI_moxa::reset_position(int i) {
#ifdef T_INFO_FUNC
	std::cout << "[func] HI_moxa::reset_position(" << i << ")" << std::endl;
#endif
}

} // namespace sarkofag
} // namespace edp
} // namespace mrrocpp


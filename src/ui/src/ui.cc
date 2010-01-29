/* Y o u r   D e s c r i p t i o n                       */
/*                            AppBuilder Photon Code Lib */
/*                                         Version 2.01  */

/* Standard headers */
#include <semaphore.h>
#include <pthread.h>

#include "ui/ui.h"

ui_sr_buffer::ui_sr_buffer() :
	cb(UI_SR_BUFFER_LENGHT) {

}

void ui_sr_buffer::put_one_msg(const lib::sr_package_t& new_msg) {

	boost::mutex::scoped_lock lock(mtx);
	cb.push_back(new_msg);

	return;
}

void ui_sr_buffer::get_one_msg(lib::sr_package_t& new_msg) {
	boost::mutex::scoped_lock lock(mtx);
	new_msg = cb.front();
	cb.pop_front();

	return;
}

bool ui_sr_buffer::buffer_empty() // sprawdza czy bufor jest pusty
{
	boost::mutex::scoped_lock lock(mtx);
	return cb.empty();
}

ui_ecp_buffer::ui_ecp_buffer() {
	sem_init(&sem, 0, 0);
	communication_state = UI_ECP_AFTER_REPLY;
}

int ui_ecp_buffer::post_sem() // podniesienie semafora
{
	return sem_post(&sem); // odwieszenie watku edp_master
}

int ui_ecp_buffer::take_sem() // oczekiwanie na semafor
{
	return sem_wait(&sem);
}

int ui_ecp_buffer::trywait_sem() // oczekiwanie na semafor
{
	return sem_trywait(&sem);
}

busy_flagger::busy_flagger(busy_flag & _flag) :
	flag(_flag) {
	flag.increment();
}

busy_flagger::~busy_flagger() {
	flag.decrement();
}

busy_flag::busy_flag() :
	counter(0) {
}

void busy_flag::increment(void) {
	boost::mutex::scoped_lock lock(m_mutex);
	counter++;
}

void busy_flag::decrement(void) {
	boost::mutex::scoped_lock lock(m_mutex);
	counter--;
}

bool busy_flag::is_busy() const {
	//	boost::mutex::scoped_lock lock(m_mutex);
	return (counter);
}

void function_execution_buffer::command(command_function_t _com_fun) {
	boost::unique_lock<boost::mutex> lock(mtx);

	// assign command for execution
	com_fun = _com_fun;
	has_command = true;

	cond.notify_one();

	return;
}

int function_execution_buffer::wait_and_execute() {
	command_function_t popped_command;

	{
		boost::unique_lock<boost::mutex> lock(mtx);

		while (!has_command) {
			cond.wait(lock);
		}

		has_command = false;
		popped_command = com_fun;
	}

	busy_flagger flagger(communication_flag);
	set_ui_state_notification(UI_N_BUSY);
	return popped_command();
}


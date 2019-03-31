#include "lux_wrapper.h"
#include "lux.h"
#include <assert.h>
#include "common.h"
#include <stdio.h>

int file_des_lux;
uint8_t register_data;
int ret_status;

int main()
{
	printf("Begin\n");

	/*CONTROL_REGISTER*/
	ret_status = byte_access_lux_register(file_des_lux, CONTROL_REGISTER,COMMAND , &register_data, CLEAR_PENDING_INTERUPTS );
	assert(ret_status == SUCCESS);

	register_data = 0x03;
	ret_status = byte_access_lux_register(file_des_lux, CONTROL_REGISTER,WRITE , &register_data, NONE );
	assert(ret_status == SUCCESS);

	register_data = 0;
	ret_status = byte_access_lux_register(file_des_lux, CONTROL_REGISTER,READ , &register_data, NONE );
	assert(register_data == 0x03);

	printf("End\n");
	return SUCCESS;

}
#include "temp.h"
#include <assert.h>
#include "common.h"
#include <stdio.h>

int ret_status;

int main()
{

	printf("Begin\n");
	uint16_t configuration;

	/******************************************************/
	/* Testing temperature init */
	ret_status = temp_sensor_init();
	assert(ret_status == SUCCESS);

	/******************************************************/
	/* Writing default value 0x60A0 to the control register */
	ret_status = config_reg_write_default();
	assert(ret_status == SUCCESS);

	ret_status = config_reg_read(&configuration);
	assert(ret_status == SUCCESS);

	assert(configuration == 0x60A0);

	tlow_reg_write(10);

	assert
	printf("tlow = %f\n",tlow_reg_read() * 0.0625);
	thigh_reg_write(100);

	printf("thigh = %f\n",thigh_reg_read() *  0.0625);

	

	// config_read_conversion_rate();

	// config_reg_read(&configuration);

	config_sd();

	config_reg_read(&configuration);

	// config_sd_continuous();

	// config_reg_read(&configuration);

	// i2c_setup(&file_des_lux,2,0x39);

	// /*CONTROL_REGISTER*/
	// ret_status = byte_access_lux_register(file_des_lux, CONTROL_REGISTER,COMMAND , &register_data, NONE );
	// assert(ret_status == SUCCESS);

	// register_data = 0x03;
	// ret_status = byte_access_lux_register(file_des_lux, NONE,WRITE , &register_data, NONE );
	// assert(ret_status == SUCCESS);
	

	// ret_status = byte_access_lux_register(file_des_lux, CONTROL_REGISTER,COMMAND , &register_data, NONE );
	// assert(ret_status == SUCCESS);

	// register_data = 0;
	// ret_status = byte_access_lux_register(file_des_lux, NONE,READ , &register_data, NONE );
	// printf("REGISTER DATA after write%x\n",register_data );
	// assert(register_data ==  0x33);//reserved set as 3
	
	// /******************************************************/
	// ret_status =  byte_access_lux_register(file_des_lux, TIMING_REGISTER,COMMAND , &register_data, NONE );
	// assert(ret_status == SUCCESS);	
	
	// register_data = 0x02;
	// ret_status = byte_access_lux_register(file_des_lux, NONE,WRITE , &register_data, NONE );
	// assert(ret_status == SUCCESS);	

	// ret_status = byte_access_lux_register(file_des_lux, TIMING_REGISTER,COMMAND , &register_data, NONE );
	// assert(ret_status == SUCCESS);

	// register_data = 0;
	// ret_status = byte_access_lux_register(file_des_lux, NONE,READ , &register_data, NONE );
	// printf("REGISTER DATA after write%x\n",register_data );
	// assert(register_data ==  0x02);


	// /******************************************************/
	// ret_status =  byte_access_lux_register(file_des_lux, INTERRUPT,COMMAND , &register_data, NONE );
	// assert(ret_status == SUCCESS);	
	
	// register_data = 0x11;
	// ret_status = byte_access_lux_register(file_des_lux, NONE,WRITE , &register_data, NONE );
	// assert(ret_status == SUCCESS);	

	// ret_status = byte_access_lux_register(file_des_lux, INTERRUPT,COMMAND , &register_data, NONE );
	// assert(ret_status == SUCCESS);

	// register_data = 0;
	// ret_status = byte_access_lux_register(file_des_lux, NONE,READ , &register_data, NONE );
	// printf("REGISTER DATA after write%x\n",register_data );
	// assert(ret_status == SUCCESS);
	// assert(register_data ==  0x11);

	
	// /******************************************************/

	// ret_status =  byte_access_lux_register(file_des_lux, INDICATION_REGISTER,COMMAND , &register_data, NONE );
	// assert(ret_status == SUCCESS);	
	

	// register_data = 0;
	// ret_status = byte_access_lux_register(file_des_lux, NONE,READ , &register_data, NONE );
	// printf("REGISTER DATA - Indication register -%x\n",register_data );
	// assert(ret_status == SUCCESS);

	
	// /******************************************************/
	// ret_status =  byte_access_lux_register(file_des_lux, DATA0LOW_REGISTER,COMMAND , &register_data, NONE );
	// assert(ret_status == SUCCESS);	
	

	// register_data = 0;
	// ret_status = byte_access_lux_register(file_des_lux, NONE,READ , &register_data, NONE );
	// printf("REGISTER DATA - DATA0LOW_REGISTER- %x\n",register_data );
	// assert(ret_status == SUCCESS);

	
	// /******************************************************/
	// ret_status =  byte_access_lux_register(file_des_lux, DATA0HIGH_REGISTER,COMMAND , &register_data, NONE );
	// assert(ret_status == SUCCESS);	
	

	// register_data = 0;
	// ret_status = byte_access_lux_register(file_des_lux, NONE,READ , &register_data, NONE );
	// printf("REGISTER DATA - DATA0HIGH_REGISTER %x\n",register_data );
	// assert(ret_status == SUCCESS);

	
	// /******************************************************/
	// ret_status =  byte_access_lux_register(file_des_lux, DATA1LOW_REGISTER,COMMAND , &register_data, NONE );
	// assert(ret_status == SUCCESS);	
	

	// register_data = 0;
	// ret_status = byte_access_lux_register(file_des_lux, NONE,READ , &register_data, NONE );
	// printf("REGISTER DATA - DATA1LOW_REGISTER %x\n",register_data );
	// assert(ret_status == SUCCESS);

	
	// /******************************************************/
	// ret_status =  byte_access_lux_register(file_des_lux, DATA1HIGH_REGISTER,COMMAND , &register_data, NONE );
	// assert(ret_status == SUCCESS);	
	

	// register_data = 0;
	// ret_status = byte_access_lux_register(file_des_lux, NONE,READ , &register_data, NONE );
	// printf("REGISTER DATA - DATA1HIGH_REGISTER %x\n",register_data );
	// assert(ret_status == SUCCESS);

	
	// /******************************************************/
	// uint16_t word;
	// /*command to write as a word for high threshold register */
	// ret_status = word_access_lux_register(file_des_lux, THRESHHIGHLOW,COMMAND , &word, NONE );
	// assert(ret_status == SUCCESS);

	// /*Writing to threshold register*/
	// word = 0x0BB8; //set to 3000
	// ret_status = word_access_lux_register(file_des_lux, NONE,WRITE , &word, NONE );
	// assert(ret_status == SUCCESS);

	// ret_status = word_access_lux_register(file_des_lux, THRESHHIGHLOW,COMMAND , &word, NONE );
	// assert(ret_status == SUCCESS);

	// /*Writing to threshold register*/
	// ret_status = word_access_lux_register(file_des_lux, NONE,READ , &word, NONE );
	// assert(ret_status == SUCCESS);
	// printf("REGISTER DATA - THRESHHIGHLOW %x\n",word );
	// assert(word == 0x0BB8);
	// /******************************************************/
	// /*command to write as a word for high threshold register */
	// ret_status = word_access_lux_register(file_des_lux, THRESHLOWLOW,COMMAND , &word, NONE );
	// assert(ret_status == SUCCESS);

	// /*Writing to threshold register*/
	// word = 0x0101; //set to 3000
	// ret_status = word_access_lux_register(file_des_lux, NONE,WRITE , &word, NONE );
	// assert(ret_status == SUCCESS);

	// ret_status = word_access_lux_register(file_des_lux, THRESHLOWLOW,COMMAND , &word, NONE );
	// assert(ret_status == SUCCESS);

	// /*Writing to threshold register*/
	// ret_status = word_access_lux_register(file_des_lux, NONE,READ , &word, NONE );
	// assert(ret_status == SUCCESS);
	// printf("REGISTER DATA - THRESHLOWLOW %x\n",word );
	// assert(word == 0x0101);




	printf("End\n");
	return SUCCESS;

}
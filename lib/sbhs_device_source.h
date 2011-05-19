
#ifndef INCLUDED_SBHS_DEVICE_SOURCE_H
#define INCLUDED_SBHS_DEVICE_SOURCE_H

#include<gr_block.h>
#define FAN 253
#define HEATER 254
#define TEMPERATURE 255

class sbhs_device_source;

typedef boost::shared_ptr<sbhs_device_source> sbhs_device_source_sptr; 

sbhs_device_source_sptr
sbhs_make_device_source (int fan_speed,int heater_temperature);

class sbhs_device_source : public gr_block
{
	private:

	friend sbhs_device_source_sptr sbhs_make_device_source (int fan_speed,int heater_temperature);			

	sbhs_device_source (int fan_speed,int heater_temperature);
	
 	public:
	
	char d_tty[50];
	int d_sbhs;
	int d_fan_speed,d_heater_temperature;
		
	~sbhs_device_source ();
	

  	int general_work (int noutput_items,
		    gr_vector_int &ninput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items);

	void search_device();
	void python_sleep();
	void create_file();
	void initialize_device();
	void set_fan_speed(int fan_speed);
	void set_heater_temperature(int heater_temperature);
};


#endif /*  INCLUDED_SBHS_DEVICE_SOURCE_H */

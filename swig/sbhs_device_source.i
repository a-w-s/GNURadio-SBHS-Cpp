/* -*- c++ -*- */

GR_SWIG_BLOCK_MAGIC(sbhs,device_source);

sbhs_device_source_sptr
sbhs_make_device_source (int fan_speed,int heater_temperature);

class sbhs_device_source : public gr_block
{
	private:
	friend sbhs_device_source_sptr sbhs_make_device_source (int fan_speed,int heater_temperature);			
	sbhs_device_source (int fan_speed,int heater_temperature);
	char d_tty[50];
	int d_sbhs;
	int d_fan_speed,d_heater_temperature;
	
 	public:	
	~sbhs_device_source ();

	void search_device();
	void python_sleep();
	void create_file();
	void initialize_device();	
	void set_fan_speed(int fan_speed);
	void set_heater_temperature(int heater_temperature);
};

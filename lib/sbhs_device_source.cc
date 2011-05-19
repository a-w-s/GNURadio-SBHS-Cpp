
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include<sbhs_device_source.h>
#include<gr_io_signature.h>
#include<cstdio>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdexcept>
#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<math.h>
#include<string.h>
#include<sys/shm.h>
#include<sys/ipc.h>
#include<unistd.h>

 
sbhs_device_source_sptr
sbhs_make_device_source (int fan_speed,int heater_temperature)
{
	return sbhs_device_source_sptr (new sbhs_device_source(fan_speed,heater_temperature));
}

void sbhs_device_source::create_file()
{
	FILE *fp=fopen("sleeper.py","w");
	fprintf(fp,"%s","#/usr/bin/env python\n");
	fprintf(fp,"%s","import time \n");
	fprintf(fp,"%s","time.sleep(0.05) \n");
	fclose(fp);	
}

void sbhs_device_source::python_sleep()
{	
	system("python sleeper.py");
}

void sbhs_device_source::search_device()
{
	char device[10][20],set_tty[100], id[10][20];
	int index=0,i,flag=0;

	FILE *fp=fopen("sbhs_shell","w");
	fprintf(fp,"%s","#!/bin/sh\n");
	fprintf(fp,"%s","for udi in `hal-find-by-capability --capability serial | sort` \n");
	fprintf(fp,"%s","do\n");
	fprintf(fp,"%s","parent=`hal-get-property --udi ${udi} --key 'info.parent'`\n");
	fprintf(fp,"%s","device=`hal-get-property --udi ${udi} --key 'linux.device_file'`\n");
	fprintf(fp,"%s","vendor=`hal-get-property --udi ${parent} --key 'usb.vendor_id'`\n");
	fprintf(fp,"%s","product=`hal-get-property --udi ${parent} --key 'usb.product_id'`\n");
	fprintf(fp,"%s","printf '%s %.4x:%.4x\\n' ");
	fprintf(fp,"%s","${device} ${vendor} ${product}\n");
	fprintf(fp,"%s","done\n");	
	fclose(fp);

	system("sh sbhs_shell > shbs_temp");
	
	fp=fopen("shbs_temp","r");
	while(!feof(fp))
	{
		fscanf(fp,"%s %s",device[index],id[index]);
		index++;		
	}
	
	for(i=0;i<index;i++)
	{
		if(strcmp(id[i],"0403:6001")==0)
		{	
			flag=1;							
			strcpy(d_tty,device[i]);
			printf("Device found: %s\n",d_tty);
			strcpy(set_tty,"stty -F ");
			strcat(set_tty,d_tty);
			strcat(set_tty," 9600 cs8\0");
			system(set_tty);					
			break;create_file()
		}
	}

	fclose(fp);		

	if(flag==0)
	{
		
		throw std::runtime_error ("SBHS device could not be found");
	}

}
void sbhs_device_source::initialize_device()
{
	d_sbhs=open(d_tty, O_RDWR | O_NOCTTY | O_NDELAY);

	if (d_sbhs==-1)
	{
		throw std::runtime_error ("Unable to open port for communication.");
	}

	else
	{
		fcntl(d_sbhs, F_SETFL,0);
		printf("Port has been sucessfully opened.\n");
	}

}

void sbhs_device_source::set_fan_speed(int fan_speed)
{
	char buffer[2];
	d_fan_speed=fan_speed;
	
	buffer[0]=FAN;buffer[1]=NULL;
	write(d_sbhs,buffer,1);

	buffer[0]=fan_speed;buffer[1]=NULL;
	write(d_sbhs,buffer,1);
	
}

void sbhs_device_source::set_heater_temperature(int heater_temperature)
{
	char buffer[2];
	d_heater_temperature=heater_temperature;

	buffer[0]=HEATER;buffer[1]=NULL;
	write(d_sbhs,buffer,1);
	
	buffer[0]=heater_temperature;buffer[1]=NULL;
	write(d_sbhs,buffer,1);
}


sbhs_device_source::sbhs_device_source (int fan_speed,int heater_temperature)
	: gr_block ("device_source",
		gr_make_io_signature (0, 0, 0),
		gr_make_io_signature (1, 1, sizeof(float)))
{	 	
	search_device();

	initialize_device();	

	create_file();	

	set_fan_speed(fan_speed);

	set_heater_temperature(heater_temperature);
}

sbhs_device_source::~sbhs_device_source ()
{
	close(d_sbhs);
}


int 
sbhs_device_source::general_work (int noutput_items,
			       gr_vector_int &ninput_items,
			       gr_vector_const_void_star &input_items,
			       gr_vector_void_star &output_items)
{
   	float *output = (float *) output_items[0];
  	int size = noutput_items;	
	char buffer[2];
	int DONE=1;
		
	python_sleep();

	while(DONE)
	{		
		buffer[0]=TEMPERATURE;buffer[1]=NULL;
		write(d_sbhs,buffer,1);

		buffer[0]=NULL;buffer[1]=NULL;
		read(d_sbhs,buffer,2);

		if(buffer[0]>9 && buffer[1]<10)
		{
			output[0]=(float)buffer[0]+(0.1*buffer[1]);					
			printf("Temperature Read [%.1f]\n",output[0]);
			DONE=0;
		}		
	}		
	consume_each(1);	
	return 1;
}


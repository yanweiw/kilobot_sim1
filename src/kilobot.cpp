#pragma once
#include "kilolib.h"

//my definitions for this assignment
#define ROBOT_SPACING 40
#define ARENA_WIDTH 32*32 + 33*ROBOT_SPACING
#define MAX_HOPCOUNT 255
#define MIN(a, b) (a < b ? a : b)


class mykilobot : public kilobot
{
	unsigned char distance;
	message_t out_message;
	int rxed=0;

	// set motion to stop
	int motion=4;
	long int motion_timer=0;

	int msrx=0;
	struct mydata {
		unsigned int data1;
		unsigned int data2;
	};

	mydata hopcnt; // to store information of hopcounts from the two seeds

	//main loop
	void loop()
	{
		// //set_motors(kilo_straight_left, kilo_straight_right);
		// if(rxed)
		// {
		// 	rxed=0;
		// 	if(motion==1)
		// 	{
		// 		if(out_message.data[0]<(id))
		// 		{
		//
		// 			motion=0;
		// 			motion_timer=kilo_ticks;//kilo_ticks is the kilobots clock
		//
		// 		}
		// 	}
		// }
		//
		// if(motion==0)
		// {
		// 	if(kilo_ticks>motion_timer+100)
		// 		motion=1;
		// }
		//
		// if(motion==0)
		// {
		// 	set_motors(0, 0);//turn off motors
		// 	//set_color(RGB(3,0,0));//set color
		// }
		// else
		// {
		// 	//set_color(RGB(0,3,0));
		//
		// 	if(rand()%100<90)
		// 	{
		// 		spinup_motors();//first start motors
		// 		set_motors(kilo_straight_left, kilo_straight_right);//then command motion
		// 	}
		// 	else if(rand()%100<95)
		// 	{	spinup_motors();
		// 		set_motors(0, kilo_turn_right);
		// 	}
		// 	else
		// 	{
		// 		spinup_motors();
		// 		set_motors(kilo_turn_left, 0);
		// 	}
		//
		// }

		//update message
		out_message.type = NORMAL;
		out_message.data[0] = MIN(hopcnt.data1 + 1, MAX_HOPCOUNT);
		out_message.data[1] = MIN(hopcnt.data2 + 1, MAX_HOPCOUNT);
		// out_message.data[2] = 0;
		out_message.crc = message_crc(&out_message);

		//update color
		if (hopcnt.data2 % 3 == 1) {
			set_color(RGB(3,0,0));
		} else if (hopcnt.data2 % 3 == 2) {
			set_color(RGB(0,3,0));
		} else if (hopcnt.data2 % 3 == 0){
			set_color(RGB(0,0,3));
		}
	}

	//executed once at start
	void setup()
	{
		// initialize hopcounts
		hopcnt.data1 = MAX_HOPCOUNT;
		hopcnt.data2 = MAX_HOPCOUNT;

		//id=id&0xff;
		out_message.type = NORMAL;
		// out_message.data[0] = id;
		out_message.data[0] = MIN(hopcnt.data1 + 1, MAX_HOPCOUNT); // to prevent negatives
		out_message.data[1] = MIN(hopcnt.data2 + 1, MAX_HOPCOUNT);
		out_message.crc = message_crc(&out_message);

		//set two special seeds
		if (pos[0] == radius + ROBOT_SPACING && pos[1] == radius + ROBOT_SPACING) {
			id = 1;
			hopcnt.data1 = 1;
			set_color(RGB(3,0,0));
		} else if (pos[0] == ARENA_WIDTH - radius - ROBOT_SPACING
		 						&& pos[1] == radius + ROBOT_SPACING) {
			id = 2;
			hopcnt.data2 = 1;
			set_color(RGB(0,3,0));
		} else {
			id = 0;
			set_color(RGB(0,0,0));
		}
	}

	//executed on successfull message send
	void message_tx_success()
	{
		msrx=1;
	}

	//sends message at fixed rate
	message_t *message_tx()
	{
		static int count = rand();
		count--;
		if (!(count % 50))
		{
			return &out_message;
		}
		return NULL;
	}

	//receives message
	void message_rx(message_t *message, distance_measurement_t *distance_measurement)
	{
		distance = estimate_distance(distance_measurement);
		if (message->data[0] < hopcnt.data1) {
			hopcnt.data1 = message->data[0];
		}
		if (message->data[1] < hopcnt.data2) {
			hopcnt.data2 = message->data[1];
		}
		// out_message.data[0] = message->data[0];
		// out_message.data[1] = message->data[1];
		// out_message.data[2] = message->data[2];
		rxed=1;
	}
};

#pragma once
#include "kilolib.h"

//my definitions for this assignment
#define ROBOT_SPACING 40
#define ARENA_WIDTH 32*32 + 33*ROBOT_SPACING
#define ARENA_HEIGHT 32*32 + 33*ROBOT_SPACING
#define MAX_HOPCOUNT 254 // the largest even num < 255 to initialize dark screen
#define MIN(a, b) (a < b ? a : b)

class mykilobot : public kilobot
{
	unsigned char dist;
	message_t out_message;
	int rxed=0;

	// set motion to stop
	int motion=4;
	long int motion_timer=0;

	// set loop counter
	int iteration = 0;

	int msrx=0;
	struct mydata {
		unsigned int data1;
		unsigned int data2;
	};

	mydata hopcnt; // to store information of hopcounts from the two seeds
	mydata estpos; // estimated position
	mydata color;	 // two color to display for debugging

	//main loop
	void loop()
	{
		//update message
		if (id == 1) {
			hopcnt.data1 = 1;
		} else if (id == 2) {
			hopcnt.data2 = 1;
		}
		out_message.type = NORMAL;
		out_message.data[0] = MIN(hopcnt.data1 + 1, MAX_HOPCOUNT);
		out_message.data[1] = MIN(hopcnt.data2 + 1, MAX_HOPCOUNT);
		out_message.crc = message_crc(&out_message);

		//update color
		if (iteration <= 1500) {
			if (hopcnt.data1 % 2 == 1) {
				color.data1 = 2;
			} else {
				color.data1 = 0;
			}
			if (hopcnt.data2 % 2 == 1) {
				color.data2 = 2;
			} else  {
				color.data2 = 0;
			}
		}

		// else {
		// 	if (estpos.data1 % 2 == 1) {
		// 		color.data1 = 3;
		// 	} else {
		// 		color.data1 = 0;
		// 	}
		// 	if (estpos.data2 % 2 == 1) {
		// 		color.data2 = 3;
		// 	} else {
		// 		color.data2 = 0;
		// 	}
		// }
		set_color(RGB(color.data1,0,color.data2));
		// printf("%d\n", iteration);
		iteration++;

		// now estimate coordinates after hopcounts stabilize
		// if (iteration >= 1000)
		// {
		// 	double error = distance(0,0, ARENA_WIDTH, ARENA_HEIGHT); // max error
		// 	for (int i = 0; i < ARENA_WIDTH; i++)
		// 	{
		// 		for (int j = 0; j < ARENA_HEIGHT; j++)
		// 		{
		// 			double temp_dist = distance(i, j, (hopcnt.data1 * comm_range), (hopcnt.data2 * comm_range));
		// 			if (temp_dist < error)
		// 			{
		// 				error = temp_dist;
		// 				estpos.data1 = hopcnt.data1;
		// 				estpos.data2 = hopcnt.data2;
		// 			}
		// 		}
		// 	}
		// }

	}

	//executed once at start
	void setup()
	{
		// initialize hopcounts
		hopcnt.data1 = MAX_HOPCOUNT;
		hopcnt.data2 = MAX_HOPCOUNT;

		// out_message.type = NORMAL;
		// out_message.data[0] = MIN(hopcnt.data1 + 1, MAX_HOPCOUNT); // to prevent negatives
		// out_message.data[1] = MIN(hopcnt.data2 + 1, MAX_HOPCOUNT);
		// out_message.crc = message_crc(&out_message);
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
		dist = estimate_distance(distance_measurement);
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

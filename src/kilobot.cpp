#pragma once
#include "kilolib.h"

//my definitions for this assignment
#define ROBOT_SPACING 40
#define ARENA_WIDTH 32*32 + 33*ROBOT_SPACING
#define ARENA_HEIGHT 32*32 + 33*ROBOT_SPACING
#define MAX_HOPCOUNT 254 // the largest even num < 255 to initialize dark screen
#define MIN(a, b) (a < b ? a : b)
#define LEARNING_RATE 0.0008

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
	mydata estpos; // estimated position in coords
	mydata color;	 // two color to display for debugging
	mydata seed1;  // position of seed 1 in rows and cols
	mydata seed2;  // position of seed 2 in rows and cols

	mydata calcRowColPos(int x, int y) {
		mydata rowcol;
		rowcol.data1 = x / (radius*2 + ROBOT_SPACING);
		rowcol.data2 = y / (radius*2 + ROBOT_SPACING);
		return rowcol;
	};

	double calcCoord(int RowColPos) {
		double coord = (RowColPos + 1) * (ROBOT_SPACING + radius*2) - radius;
		return coord;
	}

	double calcError(int currX, int currY) {
		double hopdist1 = (hopcnt.data1-1) * comm_range;
		double hopdist2 = (hopcnt.data2-1) * comm_range;
		double seed1x = calcCoord(seed1.data1);
		double seed1y = calcCoord(seed1.data2);
		double seed2x = calcCoord(seed2.data1);
		double seed2y = calcCoord(seed2.data2);
		double distToSeed1 = distance(currX, currY, ROBOT_SPACING+16, ROBOT_SPACING+16);
		double distToSeed2 = distance(currX, currY, (ROBOT_SPACING+32)*32-16, ROBOT_SPACING+16);
		double error = pow(distance(hopdist1, hopdist2, distToSeed1, distToSeed2), 2);
		return error;
	}

	//main loop
	void loop()
	{
		//update message, we only know pos of seeds whose id are 1 or 2
		if (id == 0) {
			hopcnt.data1 = 1;
			seed1 = calcRowColPos((int)pos[0], (int)pos[1]);
		} else if (id == 31) {
			hopcnt.data2 = 1;
			seed2 = calcRowColPos((int)pos[0], (int)pos[1]);
		}
		out_message.type = NORMAL;
		out_message.data[0] = MIN(hopcnt.data1 + 1, MAX_HOPCOUNT);
		out_message.data[1] = MIN(hopcnt.data2 + 1, MAX_HOPCOUNT);
		out_message.data[2] = seed1.data1;	// broadcast seed 1 position
		out_message.data[3] = seed1.data2;
		out_message.data[4] = seed2.data1;	// broadcast seed 2 position
		out_message.data[5] = seed2.data2;
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
		//gradient decent
		else {
			int currX = estpos.data1;
			int currY = estpos.data2;
			double e0 = calcError(currX, currY);
			double e1 = calcError(currX-1, currY);
			double e2 = calcError(currX+1, currY);
			double e3 = calcError(currX, currY-1);
			double e4 = calcError(currX, currY+1);
			if (e0 > e1) {
				if (e0 > e2 && e0-e2 > e0-e1) {
					estpos.data1 = currX + (int)(LEARNING_RATE*(e0-e2));
				} else {
					estpos.data1 = currX - (int)(LEARNING_RATE*(e0-e1));
				}
			} else if (e0 > e2) {
				estpos.data1 = currX + (int)(LEARNING_RATE*(e0-e2));
			}
			if (e0 > e3) {
				if (e0 > e4 && e0-e4 > e0-e3) {
					estpos.data2 = currY + (int)(LEARNING_RATE*(e0-e4));
				} else {
					estpos.data2 = currY - (int)(LEARNING_RATE*(e0-e3));
				}
			} else if (e0 > e4) {
				estpos.data2 = currY + (int)(LEARNING_RATE*(e0-e4));
			}

			if (iteration == 2500 && (id == 0 || id == 31 || id == 100 || id == 200 || id == 400 || id == 600 || id == 800 || id == 1000)) {
				printf("id: %d; e0: %d; X: %d; Y: %d; x: %d; y: %d\n", id, (int)e0,(int)pos[0],(int)pos[1], estpos.data1, estpos.data2);
				printf("truedist1: %d; calcdist1: %d; hopdist1: %d\n", (int)distance(pos[0], pos[1], ROBOT_SPACING+16, ROBOT_SPACING+16), (int)distance(currX, currY, ROBOT_SPACING+16, ROBOT_SPACING+16), (hopcnt.data1-1) * (int)comm_range);
				printf("truedist2: %d; calcdist2: %d; hopdist2: %d\n\n", (int)distance(pos[0], pos[1], (ROBOT_SPACING+32)*32-16, ROBOT_SPACING+16), (int)distance(currX, currY, (ROBOT_SPACING+32)*32-16, ROBOT_SPACING+16), (hopcnt.data2-1)*(int)comm_range);
			}

			if (((estpos.data1+16)/(32+ROBOT_SPACING)) % 2 == 1) {
				color.data1 = 3;
			} else {
				color.data1 = 0;
			}
			if (((estpos.data2+16)/(32+ROBOT_SPACING)) % 2 == 1) {
				color.data2 = 3;
			} else {
				color.data2 = 0;
			}

			// if ((((int)pos[0]+16)/(32+ROBOT_SPACING)) % 2 == 1) {
			// 	color.data1 = 3;
			// } else {
			// 	color.data1 = 0;
			// }
			// if ((((int)pos[1]+16)/(32+ROBOT_SPACING)) % 2 == 1) {
			// 	color.data2 = 3;
			// } else {
			// 	color.data2 = 0;
			// }
		}

		set_color(RGB(color.data1,0,color.data2));
		// printf("%d\n", iteration);
		iteration++;

		// printf("pos: %d, %d\n", (int)pos[0], (int)pos[1]);

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

		// float x = (float) rand() * (ARENA_WIDTH-2*radius) / RAND_MAX + radius;
		// float y = (float) rand() * (ARENA_HEIGHT-2*radius) / RAND_MAX + radius;
		estpos.data1 = rand() % ARENA_WIDTH; // randomize initial x estimate
		estpos.data2 = rand() % ARENA_HEIGHT; // randomize initial y estimate
		// printf("intial x and y: %d, %d\n", estpos.data1, estpos.data2);

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
		seed1.data1 = message->data[2];
		seed1.data2 = message->data[3];
		seed2.data1 = message->data[4];
		seed2.data2 = message->data[5];
		rxed=1;
	}
};

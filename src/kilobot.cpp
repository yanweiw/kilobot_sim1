#pragma once
#include "kilolib.h"

//my definitions for this assignment
#define ROBOT_SPACING 8
#define ARENA_WIDTH 32*32 + 33*ROBOT_SPACING
#define ARENA_HEIGHT 32*32 + 33*ROBOT_SPACING
#define MAX_HOPCOUNT 254 // the largest even num < 255 to initialize dark screen
#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)
#define LEARNING_RATE 0.0008
#define STAGE_1	700// time for hopcnt propagation of seeds
#define STAGE_2 1000  // time to identify farthest points to the seeds
#define STAGE_3 1700 // time for hopcnt propagation of farthest points to the seeds
#define STAGE_4 2500
#define BOUNDARY 500 // 8 hopcnts horizonally * comm_range = 480 -> position span
#define SMOOTHING 1

class mykilobot : public kilobot
{
	unsigned char dist;
	message_t out_message;
	int rxed=0;
	double avehop1 = 0;
	double avehop2 = 0;
	double avehop3 = 0;
	double avehop4 = 0;
	int count1;
	int count2;
	int count3;
	int count4;
	double hop1;
	double hop2;
	double hop3;
	double hop4;
	int amIfar1=0;	// label to find farthest points to the two seeds
	int amIfar2=0;

	// set motion to stop
	int motion=4;
	long int motion_timer=0;

	// set loop counter
	int iteration = 0;

	// belief
	int amIpurple=0;

	int msrx=0;
	struct mydata {
		unsigned int data1;
		unsigned int data2;
	};

	mydata hopcnt; // to store information of hopcounts from the two seeds
	mydata estpos; // estimated position in coords
	mydata newhopcnt; // record hopcnt gradient from the farthest points to seeds

	double calcError(int currX, int currY, double hop1, double hop2, double hop3, double hop4) {
		int hopdist1 = (int) (MAX((hop1-1), 0) * comm_range);
		int hopdist2 = (int) (MAX((hop2-1), 0) * comm_range);
		int hopdist3 = (int) (MAX((hop3-1), 0) * comm_range);
		int hopdist4 = (int) (MAX((hop4-1), 0) * comm_range);

		int distToSeed1 = (int) distance(currX, currY, 30, 30);
		int distToSeed2 = (int) distance(currX, currY, 470, 30);
		int distToSeed3 = (int) distance(currX, currY, 470, 470);
		int distToSeed4 = (int) distance(currX, currY, 30, 470);

		int diff1 = pow(hopdist1 - distToSeed1, 2);
		int diff2 = pow(hopdist2 - distToSeed2, 2);
		int diff3 = pow(hopdist3 - distToSeed3, 2);
		int diff4 = pow(hopdist4 - distToSeed4, 2);
		// double weight = hop1 + hop2 + hop3 + hop4;
		// double error = diff1 / pow(hop1,2) + diff2 / pow(hop2,2) + diff3 / pow(hop3,2) + diff4 / pow(hop4,4);
		// printf("errors: %f\n", error);
		double error = diff1 + diff2 + diff3 + diff4;
		return error;
	}

	//main loop
	void loop()
	{
		//update message
		out_message.type = NORMAL;
		int r=0, g=0, b=0, val=3;

		// STAGE_1: we only know pos of seeds whose id are 1 or 2, sends hopcnts
		if (iteration < STAGE_1) {
			if (id == 0) {
				hopcnt.data1 = 1;
			} else if (id == 31) {
				hopcnt.data2 = 1;
			}
			out_message.data[0] = hopcnt.data1;
			out_message.data[1] = hopcnt.data2;
			// update color
			if (hopcnt.data1 % 2 == 1) {
				r = 2;
			} else {
				r = 0;
			}
			if (hopcnt.data2 % 2 == 1) {
				b = 2;
			} else  {
				b = 0;
			}
		}

		// STAGE_2, we search for largest hopcnts below
		if (iteration == STAGE_1) {
			amIfar1 = 1;
			amIfar2 = 1;
		}
		if (iteration > STAGE_1 && iteration <= STAGE_2) { // find farthest points
			if (amIfar1) {
				r=3;g=3;b=3;
			}
			if (amIfar2) {
				r=2;g=2;b=2;
			}
		}

		// STAGE_3, we send hopcnts of farthest points
		if (iteration > STAGE_2 && iteration < STAGE_3) {
			if (amIfar1) {
				newhopcnt.data1 = 1;
			}
			out_message.data[3] = newhopcnt.data1;
			if (amIfar2) {
				newhopcnt.data2 = 1;
			}
			out_message.data[5] = newhopcnt.data2;
			// update color
			if (newhopcnt.data1 % 2 == 1) {
				r = 2;
			} else {
				r = 0;
			}
			if (newhopcnt.data2 % 2 == 1) {
				b = 2;
			} else {
				b =0 ;
			}
		}

		// STAGE_4, gradient decent in 50 * 50 box out of 0-500 * 0-500 range
		if (iteration >= STAGE_3 && iteration < STAGE_4) {
			hop1 = hopcnt.data1;
			hop2 = hopcnt.data2;
			hop3 = newhopcnt.data1;
			hop4 = newhopcnt.data2;
			if (SMOOTHING) {
				hop1 += avehop1;
				hop1 /= count1 + 1;
				hop2 += avehop2;
				hop2 /= count2 + 1;
				hop3 += avehop3;
				hop3 /= count3 + 1;
				hop4 += avehop4;
				hop4 /= count4 + 1;
				if ((hop4 - (int)hop4) <= 0.33) {
					val = 1;
				} else if ((hop4 - (int)hop4) <= 0.66) {
					val = 2;
				} else {
					val = 3;
				}
			}
			if ((int)hop4 % 3 < 1) {
				r = val;
			} else if ((int)hop4 % 3 < 2) {
				g = val;
			} else {
				b = val;
			}
			out_message.data[0] = hopcnt.data1;
			out_message.data[1] = hopcnt.data2;
			out_message.data[3] = newhopcnt.data1;
			out_message.data[5] = newhopcnt.data2;
		}

		if (iteration >= STAGE_4) {
			//gradient decent, or in fact to search in a box
			int currX = estpos.data1;
			int currY = estpos.data2;
			double error = calcError(currX, currY, hop1, hop2, hop3, hop4);
			for (int i = -25; i < 25; i++) {
				if (currX + i >= 0 && currX + i < BOUNDARY) {
					for (int j = -25; j < 25; j++) {
						if (currY + j >=0 && currY + j < BOUNDARY) {
							double tmp_error = calcError(currX+i, currY+j, hop1, hop2, hop3, hop4);
							if (tmp_error < error) {
								error = tmp_error;
								estpos.data1 = currX + i;
								estpos.data2 = currY + j;
							}
						}
					}
				}
			}
			if (currX < 150 || currX > 350 || (1.1*currX + 0.9*currY > 430 && 1.1*currX + 0.9*currY < 600)) {
				r=2; b=3;
			} else {
				r=3; b=3; g=3;
			}
		}

		set_color(RGB(r,g,b));

		iteration++;
		out_message.crc = message_crc(&out_message);
	}

	//executed once at start
	void setup()
	{
		// initialize hopcounts
		hopcnt.data1 = MAX_HOPCOUNT-1;
		hopcnt.data2 = MAX_HOPCOUNT-1;
		newhopcnt.data1 = MAX_HOPCOUNT-1;
		newhopcnt.data2 = MAX_HOPCOUNT-1;
		amIpurple=0;
		avehop1=0;
		count1 =0;
		avehop2=0;
		count2 =0;
		avehop3=0;
		count3 =0;
		avehop4=0;
		count4 =0;
		// float x = (float) rand() * (ARENA_WIDTH-2*radius) / RAND_MAX + radius;
		// float y = (float) rand() * (ARENA_HEIGHT-2*radius) / RAND_MAX + radius;
		estpos.data1 = rand() % BOUNDARY; // randomize initial x estimate
		estpos.data2 = rand() % BOUNDARY; // randomize initial y estimate
		// printf("intial x and y: %d, %d\n", estpos.data1, estpos.data2);

		out_message.type = NORMAL;
		out_message.data[0] = hopcnt.data1;
		out_message.data[1] = hopcnt.data2;
		out_message.data[3] = newhopcnt.data1;
		out_message.data[5] = newhopcnt.data2;
		out_message.crc = message_crc(&out_message);
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
		// initial hopcnts
		if (iteration < STAGE_1) {
			dist = estimate_distance(distance_measurement);
			if (MIN(message->data[0] + 1, MAX_HOPCOUNT) < hopcnt.data1) {
				hopcnt.data1 = MIN(message->data[0] + 1, MAX_HOPCOUNT);
			}
			if (MIN(message->data[1] + 1, MAX_HOPCOUNT) < hopcnt.data2) {
				hopcnt.data2 = MIN(message->data[1] + 1, MAX_HOPCOUNT);
			}
		}
		// find the largest hopcnt
		if (iteration > STAGE_1 && iteration < STAGE_2) {
			if (message->data[2] < hopcnt.data1) {
				out_message.data[2] = hopcnt.data1;
			} else {
				out_message.data[2] = message->data[2];
			}
			if (message->data[2] > hopcnt.data1) {
				amIfar1 = 0;
			}
			if (message->data[4] < hopcnt.data2) {
				out_message.data[4] = hopcnt.data2;
			} else {
				out_message.data[4] = message->data[4];
			}
			if (message->data[4] > hopcnt.data2) {
				amIfar2 = 0;
			}
		}
		// second round of hopcnts
		if (iteration > STAGE_2 && iteration < STAGE_3) {
			if (MIN(message->data[3] + 1, MAX_HOPCOUNT) < newhopcnt.data1) {
				newhopcnt.data1 = MIN(message->data[3] + 1, MAX_HOPCOUNT);
			}
			if (MIN(message->data[5] + 1, MAX_HOPCOUNT) < newhopcnt.data2) {
				newhopcnt.data2 = MIN(message->data[5] + 1, MAX_HOPCOUNT);
			}
		}

		// if (iteration > 2600 && message->data[6]) {
		// 	amIpurple++;
		// }
		if (SMOOTHING && iteration > STAGE_3 && iteration < STAGE_4) {
			avehop1 += message->data[0];
			count1 += 1;
			avehop2 += message->data[1];
			count2 += 1;
			avehop3 += message->data[3];
			count3 += 1;
			avehop4 += message->data[5];
			count4 += 1;
		}
		rxed=1;
	}
};

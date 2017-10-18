#define ROBOT_COUNT 50

#define ARENA_WIDTH 1000
#define ARENA_HEIGHT 1000

#define SIMULATION_TIME 180 //in seconds

// function to check if randomly generated positions are in collision
bool collision(float x, float y, float pos_list[ROBOT_COUNT][3], int limit) {
	for (int i = 0; i < limit; i++) {
		float existing_x = pos_list[i][0];
		float existing_y = pos_list[i][1];
		float dist = sqrt(pow(x - existing_x, 2) + pow(y - existing_y, 2));
		if (dist <= radius * 2) {
			return true;
		}
	}
	return false;
}

void setup_positions(float robot_pos[ROBOT_COUNT][3])
{
	// int columns = 5;
	// int horizontal_separation = 200;
	// int vertical_separation = 200;
	// for (int i = 0;i < ROBOT_COUNT;i++)
	// {
	// 	int c = i % columns + 1;
	// 	int r = i / columns + 1;
	// 	robot_pos[i][0] = c * horizontal_separation; //x
	// 	robot_pos[i][1] = r * vertical_separation;   //y
	// 	robot_pos[i][2]= rand() * 2 * PI / RAND_MAX; //theta
	// }

// my code to initialize random positions

	int i = 0; //upper limit for iteration
	while (i < ROBOT_COUNT) {
		float x = (float) rand() * (ARENA_WIDTH-2*radius) / RAND_MAX + radius;
		float y = (float) rand() * (ARENA_HEIGHT-2*radius) / RAND_MAX + radius;
		float theta = rand() * 2 * PI / RAND_MAX;
		if (collision(x, y, robot_pos, i)) {
			continue;
		}
		robot_pos[i][0] = x;
		robot_pos[i][1] = y;
		robot_pos[i][2] = theta;
		i++;
	}

}

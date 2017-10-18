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
		printf("%f\n", dist);
		if (dist <= radius * 2) {
			return true;
		}
	}
	return false;
}

void setup_positions(float robot_pos[ROBOT_COUNT][3])
{
	 int columns = 5;
	 int horizontal_separation = 200;
	 int vertical_separation = 200;
	 for (int i = 0;i < ROBOT_COUNT;i++)
	 {
	 	int c = i % columns + 1;
	 	int r = i / columns + 1;
		bool col = true;
		float x,y;
		while (col)
		{
			x = (float) rand() * (ARENA_WIDTH-2*radius) / RAND_MAX + radius;
			y = (float) rand() * (ARENA_HEIGHT-2*radius) / RAND_MAX + radius;
			col = collision(x,y,robot_pos,i);
		}
		robot_pos[i][0] = x;//c * horizontal_separation; //x
 		robot_pos[i][1] = y;//r * vertical_separation;   //y


	 	robot_pos[i][2]= rand() * 2 * PI / RAND_MAX; //theta
	}
	// my code to generate random inital positions
	/*int i = 0; // counter to iterate 50 times
	printf("hello");
	while (i < 50) {
		float x = rand() / RAND_MAX * ARENA_WIDTH;
		float y = rand() / RAND_MAX * ARENA_HEIGHT;
		float theta = rand() / RAND_MAX * 2 * PI;
		if (collision(x, y, robot_pos)) {
			continue;
		}
		robot_pos[i][0] = x;
		robot_pos[i][1] = y;
		robot_pos[i][2] = theta;
		i++;
	}*/

}

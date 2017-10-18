#define ROBOT_COUNT 100

#define ARENA_WIDTH 2400
#define ARENA_HEIGHT 2400

#define SIMULATION_TIME 180 //in seconds

void setup_positions(float robot_pos[ROBOT_COUNT][3])
{
	int columns = 5;
	int horizontal_separation = 200;
	int vertical_separation = 200;
	for (int i = 0;i < ROBOT_COUNT;i++)
	{
		int c = i % columns + 1;
		int r = i / columns + 1;
		robot_pos[i][0] = c * horizontal_separation; //x
		robot_pos[i][1] = r * vertical_separation;   //y
		robot_pos[i][2]= rand() * 2 * PI / RAND_MAX; //theta
	}
}


#define ROBOT_COUNT 32*32

#define ARENA_WIDTH 32*32 + 33*40
#define ARENA_HEIGHT 32*32 + 33*40

#define SIMULATION_TIME 180 //in seconds

void setup_positions(float robot_pos[ROBOT_COUNT][3])
{
	// I modified original codes to setup 32*32 immobile kilobots
	int columns = 32;
	// 40 mm spacing between robots
	int horizontal_separation = 32 + 40;
	int vertical_separation = 32 + 40;
	for (int i = 0;i < ROBOT_COUNT;i++)
	{
		int c = i % columns;
		int r = i / columns;
		robot_pos[i][0] = c * horizontal_separation + 16 + 40; //x
		robot_pos[i][1] = r * vertical_separation + 16 + 30;   //y
		robot_pos[i][2] = 0; //theta
	}
}

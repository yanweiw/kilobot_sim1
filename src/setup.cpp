#define ROBOT_COUNT 32*32
#define ROBOT_SPACING 8

#define ARENA_WIDTH 32*32 + 33*ROBOT_SPACING
#define ARENA_HEIGHT 32*32 + 33*ROBOT_SPACING

#define SIMULATION_TIME 180 //in seconds

void setup_positions(float robot_pos[ROBOT_COUNT][4])
{
	// I modified original codes to setup 32*32 immobile kilobots
	int columns = 32;
	// 40 mm spacing between robots
	int horizontal_separation = 32 + ROBOT_SPACING;
	int vertical_separation = 32 + ROBOT_SPACING;
	for (int i = 0;i < ROBOT_COUNT;i++)
	{
		int c = i % columns;
		int r = i / columns;
		robot_pos[i][0] = c * horizontal_separation + 16 + ROBOT_SPACING; //x
		robot_pos[i][1] = r * vertical_separation + 16 + ROBOT_SPACING;   //y
		robot_pos[i][2] = 0; //theta
		robot_pos[i][3] = 0; //id, initialize all with trivial id
	}
	// pick two robots to become seeds
	robot_pos[0][3] = 1; // seed 1
	robot_pos[31][3] = 32; // seed 2
}

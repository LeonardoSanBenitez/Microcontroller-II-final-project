#define OBS_RANGE OBS_SIZE*2 -1
#define OBS_SIZE 3
#define GRID_SIZE 10
//#define DEPLOYED

#ifdef DEPLOYED
	#define LOAD_Q_TABLE
	#define INITIAL_EPSILON  0.1
	#define SHOW_EVERY 1
	#define LOG_EVERY 1
    #define HM_EPISODES 30
#else
	#define INITIAL_EPSILON 90
	#define SHOW_EVERY HM_EPISODES + 1
	#define LOG_EVERY 300000 //3000
    #define HM_EPISODES 30000000 //30000
#endif
 


#define ACTION_SPACE 4

#define LEARNING_RATE   0.1
#define DISCOUNT        0.2
#define EPS_DECAY       0.999995

#define ENEMY_PENALTY -300
#define FOOD_REWARD 1000
/*
 * app.cpp
 *
 *  Created on: 15 de abr de 2021
 *      Author: jeffe
 */

/* g++ main.cpp -o main `pkg-config --cflags --libs opencv` && main*/
/* cmake ../ && make && ./main */

//#include <opencv2/imgproc.hpp>
#include <app.hpp>
#include <nokia.hpp>
//#include <iostream>
#include <assert.h> //to disable, #define NDEBUG
#include <PRNG_LFSR.hpp>

#include "model.h"
#include "game_config.h"

int max (int* arr, int len){
    int maxval = -999; //TODO: proper -inf
    for (int i=0; i<len; i++){
        if (arr[i] > maxval){
            maxval = arr[i];
        }
    }
    return maxval;
}
int argmax (int* arr, int len){
    int maxidx = 0;
    int maxval = -999; //TODO: proper -inf
    for (int i=0; i<len; i++){
        if (arr[i] > maxval){
            maxidx = i;
            maxval = arr[i];
        }
    }
    return maxidx;
}


class EnvBlobObs{
    public:
    int dfx;
    int dfy;
    int dex;
    int dey;
};

class Step {
    public:
    EnvBlobObs new_obs;
    int reward;
    int done;
    int info;
};

class Blob{
    public:
    int x;
    int y;

    void set_pos(int x0, int y0){
        x=x0;
        y=y0;
    }

    void move(int dx, int dy){
        x += dx;
        y += dy;

        // Wrap border
        if (x >= GRID_SIZE)
        {
        	 x = 0;
        	 limpa_LCD();
        }
        if (x<0)
        {
            x = GRID_SIZE - 1;
            limpa_LCD();
        }
        if (y >= GRID_SIZE)
        {
        	y = 0;
        	limpa_LCD();
        }
        if (y<0)
        {
            y = GRID_SIZE - 1;
            limpa_LCD();
        }
    }

};

class EnvBlob{
    public:
    int _step_counter;
    int _done;

    Blob _player;
    Blob _food;
    Blob _enemy;

    EnvBlob(){
        // TODO: color
        _player = Blob();
        _food = Blob();
        _enemy = Blob();
    }

    void reset(void){
        // Generate unique tuples
        int inits[6] = {0,0,0,0,0,0};
        inits[0] = prng_LFSR()%GRID_SIZE;
        inits[1] = prng_LFSR()%GRID_SIZE;
        while (1){
            inits[2] = prng_LFSR()%GRID_SIZE;
            inits[3] = prng_LFSR()%GRID_SIZE;
            if (inits[2]!=inits[0] || inits[3]!=inits[1])
                break;
	    }
        while (1){
            inits[4] = prng_LFSR()%GRID_SIZE;
            inits[5] = prng_LFSR()%GRID_SIZE;
            if ((inits[4]!=inits[0] || inits[5]!=inits[1]) && (inits[4]!=inits[2] || inits[5]!=inits[3]))
                break;
	    }

        // Reset the agents
        _player.set_pos(inits[0], inits[1]);
        _food.set_pos(inits[2], inits[3]);
        _enemy.set_pos(inits[4], inits[5]);
        _step_counter = 0;
        _done = 0;

 	    // Impossible game check
       assert(_player.x != _enemy.x or _player.y != _enemy.y);
       assert(_player.x != _food.x or _player.y != _food.y);
       assert(_enemy.x != _food.x or _enemy.y != _food.y);
    }

    EnvBlobObs observation(void){
        EnvBlobObs o =  EnvBlobObs();
        o.dfx = _preprocess_obs(_player.x - _food.x);
        o.dfy = _preprocess_obs(_player.y - _food.y);
        o.dex = _preprocess_obs(_player.x - _enemy.x);
        o.dey = _preprocess_obs(_player.y - _enemy.y);
        return o;
    }

    // @Brief: convert the env internal representation to what is actually shown to the agent
    int _preprocess_obs(int obs){
        int max_obs = OBS_SIZE -1;
        if (obs>max_obs)
            obs = max_obs;   // Saturation
        if (obs<-max_obs)
            obs = -max_obs;

        obs = obs+max_obs; // Make positive
        return obs;
    }

    Step step(int action){
        Step s = Step(); //TODO: quando esse memória é desalocada?

        // Game mechanics
        if (action == 0)
            _player.move(1, 0);
        else if (action == 1)
            _player.move(-1, 0);
        else if (action == 2)
            _player.move(0, 1);
        else if (action == 3)
            _player.move(0, -1);
        else{}
            //throw std::logic_error("Invalid action");

        // Reward function
        int dfx = _player.x - _food.x;
        int dfy = _player.y - _food.y;
        int dex = _player.x - _enemy.x;
        int dey = _player.y - _enemy.y;
        if (dex==0 && dey==0) // exactly on the enemy
            s.reward = ENEMY_PENALTY;
        else if (dfx==0 && dfy==0) // exactly on the food
            s.reward = FOOD_REWARD;
        else if ((dfx*dfx + dfy*dfy)<3) // Near food //TODO: overflow?
            s.reward = 0;
        else
            s.reward = -1;

        EnvBlobObs o = EnvBlobObs(); //TODO: quando esse memória é desalocada?
        o.dfx = 0;
        o.dfy = 0;
        o.dex = 0;
        o.dey = 0;


        s.new_obs = o;
        s.info = 0;

        _step_counter += 1;
        if (_step_counter>=199 || s.reward==FOOD_REWARD  || s.reward==ENEMY_PENALTY)
            s.done=1;
        else
            s.done=0;
        return s;
    }

    void render(void){
        //std::cout << "------------\n";
        //std::cout << _player.x << ' ' << _player.y << '\n';
        //std::cout << _food.x << ' ' << _food.y << '\n';
        //std::cout << _enemy.x << ' ' << _enemy.y << '\n';

    	struct pontos_t scenery;

    	// limits scenery
		scenery.x1 = 0;
		scenery.y1 = 0;
		scenery.x2 = 83;
		scenery.y2 = 47;

		desenha_retangulo(&scenery,1);

    	print_AI(_player.x,_player.y);
    	print_food(_food.x,_food.y);
    	print_monster(_enemy.x,_enemy.y);

    }
};

class IntelligentAgent{
    int _epsilon; //percentual, de 0 a 100
    int _q_table[OBS_RANGE][OBS_RANGE][OBS_RANGE][OBS_RANGE][ACTION_SPACE];
    public:
        IntelligentAgent(){
            _epsilon = INITIAL_EPSILON;

            #ifdef LOAD_Q_TABLE
                //TODO: load q_table from .h file
            #else
                for (int a=0; a<OBS_RANGE; a++){
                    for (int b=0; b<OBS_RANGE; b++){
                        for (int c=0; c<OBS_RANGE; c++){
                            for (int d=0; d<OBS_RANGE; d++){
                                for (int e=0; e<ACTION_SPACE; e++){
                                    _q_table[a][b][c][d][e] =  prng_LFSR()%6 - 5;
                                }
                            }
                        }
                    }
                }
            #endif

            //std::cout << "[AI] Init done\n";
        }

        int action(EnvBlobObs* obs){
            int action;
            if (prng_LFSR()%100 > _epsilon)
                action = argmax(&_q_table[obs->dfx][obs->dfy][obs->dex][obs->dey][0], ACTION_SPACE);
            else
                action = prng_LFSR()%ACTION_SPACE;

            return action;
        }
        void feedback(EnvBlobObs* obs, EnvBlobObs* new_obs, int action, int reward){
            float max_future_q = (float) max(&_q_table[new_obs->dfx][new_obs->dfy][new_obs->dex][new_obs->dey][0], ACTION_SPACE);
            float current_q = (float) _q_table[obs->dfx][obs->dfy][obs->dex][obs->dey][action];
            int new_q = (int) ((1 - LEARNING_RATE)*current_q + LEARNING_RATE*((float)reward+DISCOUNT*max_future_q));
            _q_table[obs->dfx][obs->dfy][obs->dex][obs->dey][action] = new_q;



            //std::cout << "[AI] Great feedback, thanks\n";
        }
        int to_disk(void){
            return 0; //TODO
        }
        int from_disk(void){
            return 0; //TODO
        }
        void episode_callback(void){
            _epsilon *= EPS_DECAY;
        }
};

void game_init();
//init_LFSR(666);
EnvBlob env = EnvBlob();
IntelligentAgent agent1 = IntelligentAgent();
int episode_rewards = 0;
int done = 0;

int app(){
      //  int episode_reward = 0;

        // Play the game within episode
        //while (!done){
        EnvBlobObs obs = env.observation();

            // Collect actions
            int action = agent1.action(&obs);
			//int action = 0;
			//action = prng_LFSR()%ACTION_SPACE;
            //action from agent2?
            //action fom player?

            // Avance the game
            Step step = env.step(action);
            agent1.feedback(&obs, &step.new_obs, action, step.reward);
            episode_rewards += step.reward;
            //done=step.done;
            if(step.done==1)
            {
            	game_init();
            }
            //if (episode%SHOW_EVERY==0 && episode!=0){
                env.render();
            //}
       // }
}

void move_enemy(int action)
{

}

void game_init()
{
	limpa_LCD();
	env.reset();
	agent1.episode_callback();
}

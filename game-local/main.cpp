/* g++ main.cpp -o main `pkg-config --cflags --libs opencv` && main*/
/* cmake ../ && make && ./main */
#include <opencv2/imgproc.hpp>

#include <iostream>
#include <assert.h> //to disable, #define NDEBUG 

#include "model.h"
#include "PRNG_LFSR.h"

#define DEPLOYED 0

#define SIZE 10
#define GRID_SIZE 10
#define HM_EPISODES 10

#ifndef DEPLOYED
	#define LOAD_Q_TABLE 0
	#define EPSILON 0.9
	#define SHOW_EVERY 1
	#define LOG_EVERY 1
#else
	#define LOAD_Q_TABLE 1
	#define EPSILON  0
	#define SHOW_EVERY 1
	#define LOG_EVERY 1
#endif
 





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
    int _limit;
    //TODO: color... what class?

    Blob(int limit){
        _limit = limit;
        std::cout << "[AI] Blob init done\n";
    }
    Blob(){}
    
    void set_pos(int x0, int y0){
        x=x0;
        y=y0;
    }
    
    //TODO: overload subtraction?
    void move(int dx, int dy){
        x += dx;
        y += dy;

        // Wrap border
        if (x >= _limit)
            x = 0;
        if (x<0)
            x = _limit - 1;
        if (y >= _limit)
            y = 0;
        if (y<0)
            y = _limit - 1;
    }

};

#define ENEMY_PENALTY -300
#define FOOD_REWARD 1000

class EnvBlob{
    public:
    int _limit;
    int _step_counter;
    int _action_space = 4;
    int _obs_size;
    int _grid_size;
    int _done;

    Blob _player;
    Blob _food;
    Blob _enemy;

    EnvBlob(int limit){
        // TODO: color
        _limit = limit;
        _obs_size = 3;
        _grid_size = 10;

        _player = Blob(_grid_size);
        _food = Blob(_grid_size);
        _enemy = Blob(_grid_size);
    }

    void reset(void){ 
        // Generate unique tuples
        int inits[6] = {0,0,0,0,0,0};
        inits[0] = prng_LFSR()%_grid_size;
        inits[1] = prng_LFSR()%_grid_size;
        while (1){
            inits[2] = prng_LFSR()%_grid_size;
            inits[3] = prng_LFSR()%_grid_size;
            if (inits[2]!=inits[0] || inits[3]!=inits[1])
                break;
	    }
        while (1){
            inits[4] = prng_LFSR()%_grid_size;
            inits[5] = prng_LFSR()%_grid_size;
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
        EnvBlobObs o = EnvBlobObs();
        o.dfx = _preprocess_obs(_player.x - _food.x);
        o.dfy = _preprocess_obs(_player.y - _food.y);
        o.dex = _preprocess_obs(_player.x - _enemy.x);
        o.dey = _preprocess_obs(_player.y - _enemy.y);
        return o;
    }
    
    // @Brief: convert the env internal representation to what is actually shown to the agent
    int _preprocess_obs(int obs){
        int max_obs = _obs_size -1;
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
        else
            throw std::logic_error("Invalid action");

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
        //s.reward = 10*_step_counter;
        s.info = 0;
         
        _step_counter += 1;
        if (_step_counter>=5){s.done=1;} else {s.done=0;}
        return s;
    }

    void render(void){
        std::cout << "------------\n";
        std::cout << _player.x << ' ' << _player.y << '\n';
        std::cout << _food.x << ' ' << _food.y << '\n';
        std::cout << _enemy.x << ' ' << _enemy.y << '\n';

    }
};


class IntelligentAgent{
    public:
        int action(EnvBlobObs* obs){
            return prng_LFSR()%4;
        }
        void feedback(EnvBlobObs obs, EnvBlobObs new_obs, int action, int reward){
            std::cout << "[AI] Great feedback, thanks\n";
        }
        void episode_callback(void){}
};



int main(){
    init_LFSR(666);
    EnvBlob env = EnvBlob(SIZE); 
    IntelligentAgent agent1 = IntelligentAgent();
    int episode_rewards[HM_EPISODES];
    
    for (int episode=0; episode<HM_EPISODES; episode++){
        int episode_reward = 0;
        int done = 0;
        env.reset();

        // Play the game within episode
        while (!done){
            EnvBlobObs obs = env.observation();

            // Collect actions
            int action = agent1.action(&obs);
            //action from agent2?
            //action fom player?

            // Avance the game
            Step step = env.step(action);
            agent1.feedback(obs, step.new_obs, action, step.reward);
            episode_reward += step.reward;
            done=step.done;
            if (episode%SHOW_EVERY==0 && episode!=0){
                env.render();
            }
        }

        // After the episode is done
        if (episode%LOG_EVERY==0){
            std::cout << "end of ep " << episode << ", reward: " << episode_reward << '\n';
        }
        episode_rewards[episode] = episode_reward;
        agent1.episode_callback();
        
    }



    /*
    for (int i=0; i<LEN; i++){
        for (int j=0; j<LEN; j++){
            //std::cout << "HI" << q_table[i][j] << "\n";
            std::cout << a.action(i+j) << ' ';
        }
    }*/


}

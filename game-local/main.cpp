/* g++ main.cpp -o main `pkg-config --cflags --libs opencv` && main*/
/* cmake ../ && make && ./main */
#include <opencv2/imgproc.hpp>

#include <iostream>
//#include "InteligentAgent.h"
#include "model.h"



#define SIZE 10
#define HM_EPISODES 10
#define LOG_EVERY 1
#define SHOW_EVERY 1


class Env{
    //public:
    //void seed(int seed)=0;
};


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

    Blob(int limit){
        _limit = limit;
        std::cout << "[AI] Blob init done\ns";
    }
    Blob(){}
    
    void set_pos(int x0, int y0){
        x=x0;
        y=y0;
    }
};

#define ENEMY_PENALTY -300
#define FOOD_REWARD 1000

class EnvBlob: public Env {
    public:
    int _limit;
    int _step_counter;
    int _action_space = 4;
    int _obs_size;
    int _grid_size;

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
        _step_counter=0;
        //TODO: asserts here
    }

    EnvBlobObs observation(void){
        EnvBlobObs o = EnvBlobObs();
        o.dfx = 0;
        o.dfy = 0;
        o.dex = 0;
        o.dey = 0;
        return o;
    }

    Step step(int action){
        EnvBlobObs o = EnvBlobObs();
        o.dfx = 0;
        o.dfy = 0;
        o.dex = 0;
        o.dey = 0;

        Step s = Step();
        s.new_obs = o;
        s.reward = 10*_step_counter;
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
            return 666+obs->dfx;
        }
        void feedback(EnvBlobObs obs, EnvBlobObs new_obs, int action, int reward){
            std::cout << "[AI] Great feedback, thanks\n";
        }
        void episode_callback(void){}
};



int main(){
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

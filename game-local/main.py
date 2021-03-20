'''
The plan is to have a player blob (blue), which aims to navigate its way as quickly as possible to the food blob (green), while avoiding the enemy blob (red).


 our environment will be a 20 x 20 grid, where we have 1 player, 1 enemy, and 1 food. For now, we'll just have the player able to move

A 10x10 Q-Table for example, in this case, is ~15MB. A 20x20 is ~195MB
I wanted to see how much of a difference going from 10x10 to a 20x20 would make. As we saw, somewhere between 25k and 75K training episodes were required for the 10x10 to learn. I found it took more like 2.5 million episodes for a 20x20 model to learn.



# TODO
* Mudar tamanho Q table
* desassociar Q-table do state global
* Desacoplar agent, environment e display

# Formulação da Q table
we simply pass the x and y deltas for the food and enemy to our agent.
4 actions

# Experiment log
## Initial
on ep. 0, mean reward is nan (epsilon is 0.900)
on ep. 3000, mean reward is -174.30 (epsilon is 0.494)
on ep. 6000, mean reward is -113.65 (epsilon is 0.271)
on ep. 9000, mean reward is -84.33 (epsilon is 0.149)
on ep. 12000, mean reward is -70.38 (epsilon is 0.082)
on ep. 15000, mean reward is -58.04 (epsilon is 0.045)
on ep. 18000, mean reward is -48.46 (epsilon is 0.025)
on ep. 21000, mean reward is -42.77 (epsilon is 0.013)
on ep. 24000, mean reward is -36.74 (epsilon is 0.007)

## New actions
on ep. 0, mean reward is nan (epsilon is 0.900)
on ep. 3000, mean reward is -179.33 (epsilon is 0.494)
on ep. 6000, mean reward is -130.48 (epsilon is 0.271)
on ep. 9000, mean reward is -88.94 (epsilon is 0.149)
on ep. 12000, mean reward is -63.96 (epsilon is 0.082)
on ep. 15000, mean reward is -47.53 (epsilon is 0.045)
on ep. 18000, mean reward is -38.93 (epsilon is 0.025)
on ep. 21000, mean reward is -33.63 (epsilon is 0.013)
on ep. 24000, mean reward is -24.95 (epsilon is 0.007)
'''
import numpy as np
from PIL import Image
import cv2
import matplotlib.pyplot as plt
import pickle
from matplotlib import style
import time

style.use("ggplot")

SIZE = 10

HM_EPISODES = 25000
MOVE_PENALTY = 1
ENEMY_PENALTY = 300
FOOD_REWARD = 25
epsilon = 0.9
EPS_DECAY = 0.9998  # Every episode will be epsilon*EPS_DECAY
SHOW_EVERY = 3000  # how often to play through env visually.

start_q_table = None # None or Filename

LEARNING_RATE = 0.1
DISCOUNT = 0.95

PLAYER_N = 1  # player key in dict
FOOD_N = 2  # food key in dict
ENEMY_N = 3  # enemy key in dict

# the dict!
d = {1: (255, 175, 0),
     2: (0, 255, 0),
     3: (0, 0, 255)}


class Blob:
    def __init__(self):
        self.x = 0
        self.y = 0

    def reset(self):
        self.x = np.random.randint(0, SIZE)
        self.y = np.random.randint(0, SIZE)

    def __str__(self):
        return f"{self.x}, {self.y}"

    def __sub__(self, other):
        return (self.x-other.x, self.y-other.y)

    def move(self, x, y):
        self.x += x
        self.y += y

class BlobInteligent(Blob):
    def __init__(self, start_q_table):
        if start_q_table is None:
            self.q_table = {}
            for i in range(-SIZE+1, SIZE):
                for ii in range(-SIZE+1, SIZE):
                    for iii in range(-SIZE+1, SIZE):
                            for iiii in range(-SIZE+1, SIZE):
                                self.q_table[((i, ii), (iii, iiii))] = [np.random.uniform(-5, 0) for _ in range(4)]

        else:
            with open(start_q_table, "rb") as f:
                self.q_table = pickle.load(f)

        super().__init__()

    def action(self, obs):
        if np.random.random() > epsilon:
            action = np.argmax(self.q_table[obs])
        else:
            action = np.random.randint(0, 4)

        return action

    def feedback(self, obs, new_obs, action, reward):
        max_future_q = np.max(self.q_table[new_obs])
        current_q = self.q_table[obs][action]

        if reward == FOOD_REWARD:
            new_q = FOOD_REWARD
        else:
            new_q = (1 - LEARNING_RATE) * current_q + LEARNING_RATE * (reward + DISCOUNT * max_future_q)
        self.q_table[obs][action] = new_q

    def to_disk(self, name):
        with open(name, "wb") as f:
            pickle.dump(self.q_table, f)



# can look up from Q-table with: print(self.q_table[((-9, -2), (3, 9))]) for example
class Env():
    '''
    Environment - OpenGym interface
    '''
    def seed(self, seed):
        '''
        sets a seed that  governs the simulation's random aspects. For this environment, the seed  oversees the random fluctuations in inflation and rates of return.
        '''
        pass

    def reset(self):
        '''
        resets the state for a new episode.
        '''
        pass

    def render(self):
        '''
        renders one frame  of the simulation.  The rendering is only for display and does not  affect reinforcement learning.
        '''
        pass

    def step(self):
        '''
        performs one step of your simulation
        '''
        pass

class EnvBlob(Env):
    action_space = None
    observation_space = None



### Main
#env = wrap_env(gym.make("Atlantis-v0"))
#observation = env.reset()
#while True:
#    env.render()
#    action = agent(observation)
#    action = env.action_space.sample() 
#    observation, reward, done, info = env.step(action) 
#    if done: 
#      break;
            
episode_rewards = []
player = BlobInteligent(start_q_table=start_q_table)
food = Blob()
enemy = Blob()
for episode in range(HM_EPISODES):
    player.reset()
    food.reset()
    enemy.reset()
    if episode % SHOW_EVERY == 0:
        print("on ep. %d, mean reward is %.2f (epsilon is %.3f)"%(episode, np.mean(episode_rewards[-SHOW_EVERY:]), epsilon))
        show = True
    else:
        show = False

    episode_reward = 0
    for i in range(200):
        obs = (player-food, player-enemy)

        # Take the action!
        action = player.action(obs)
        if action == 0 and player.x < (SIZE - 1):
            player.move(x=1, y=0)
        elif action == 1 and player.x>0:
            player.move(x=-1, y=0)
        elif action == 2 and player.y < (SIZE - 1):
            player.move(x=0, y=1)
        elif action == 3 and player.y>0:
            player.move(x=0, y=-1)



        #### MAYBE ###
        #enemy.move()
        #food.move()
        ##############

        # Game logic
        if player.x == enemy.x and player.y == enemy.y:
            reward = -ENEMY_PENALTY
        elif player.x == food.x and player.y == food.y:
            reward = FOOD_REWARD
        else:
            reward = -MOVE_PENALTY

        ## NOW WE KNOW THE REWARD, LET'S CALC YO
        # first we need to obs immediately after the move.
        new_obs = (player-food, player-enemy)
        player.feedback(obs, new_obs, action, reward)


        #if show:
        #    env = np.zeros((SIZE, SIZE, 3), dtype=np.uint8)  # starts an rbg of our size
        #    env[food.x][food.y] = d[FOOD_N]  # sets the food location tile to green color
        #    env[player.x][player.y] = d[PLAYER_N]  # sets the player tile to blue
        #    env[enemy.x][enemy.y] = d[ENEMY_N]  # sets the enemy location to red
        #    img = Image.fromarray(env, 'RGB')  # reading to rgb. Apparently. Even tho color definitions are bgr. ???
        #    img = img.resize((300, 300))  # resizing so we can see our agent in all its glory.
        #    cv2.imshow("image", np.array(img))  # show it!
        #    if reward == FOOD_REWARD or reward == -ENEMY_PENALTY:  # crummy code to hang at the end if we reach abrupt end for good reasons or not.
        #        if cv2.waitKey(500) & 0xFF == ord('q'):
        #            break
        #    else:
        #        if cv2.waitKey(1) & 0xFF == ord('q'):
        #            break

        episode_reward += reward
        if reward == FOOD_REWARD or reward == -ENEMY_PENALTY:
            break

    #print(episode_reward)
    episode_rewards.append(episode_reward)
    epsilon *= EPS_DECAY

moving_avg = np.convolve(episode_rewards, np.ones((SHOW_EVERY,))/SHOW_EVERY, mode='valid')

plt.plot([i for i in range(len(moving_avg))], moving_avg)
plt.ylabel(f"Reward {SHOW_EVERY}ma")
plt.xlabel("episode #")
plt.show()

player.to_disk(f'model-{int(time.time())}.pickle')

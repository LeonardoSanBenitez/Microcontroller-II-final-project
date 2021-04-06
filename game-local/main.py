import numpy as np
from PIL import Image
import cv2
import matplotlib.pyplot as plt
import pickle
from matplotlib import style
import time

# Config train
SIZE = 3
GRID_SIZE = 10
HM_EPISODES = 30000
LOG_EVERY = 3000  
SHOW_EVERY = 500000 # how often to play through env visually.
start_q_table = None #'model-1616452736.pickle' #None # None or Filename
EPSILON = 0.9

# Config deploy
#start_q_table = 'model.pickle' #None #'model-1616452736.pickle' #None # None or Filename
#EPSILON = 0
#SHOW_EVERY = 1


class IntelligentAgent():
    LEARNING_RATE = 0.3
    DISCOUNT = 0.2
    epsilon = 0.9
    EPS_DECAY = 0.99995

    def __init__(self, start_q_table, obs_size:int, obs_range: int = None, action_space: int = None, epsilon: float = 0.9):
        self.epsilon = epsilon
        if obs_range==None: obs_range = obs_size*2 - 1
        if action_space==None: self.action_space = 4

        if start_q_table is None:
            #self.q_table = np.array((obs_range, obs_range, obs_range, obs_range, self.action_space))# np.random.rand(obs_range, obs_range, obs_range, obs_range, self.action_space)*5 - 5
            self.q_table = np.random.rand(obs_range, obs_range, obs_range, obs_range, self.action_space)*5 - 5
            #self.q_table = {}
            #for i in range(obs_range):
            #    for ii in range(obs_range):
            #        for iii in range(obs_range):
            #                for iiii in range(obs_range):
            #                    self.q_table[i, ii, iii, iiii] = np.array([np.random.uniform(-5, 0) for _ in range(self.action_space)])

        else:
            self.q_table = self.from_disk(start_q_table)
        # can look up from Q-table with: print(self.q_table[((-9, -2), (3, 9))]) for example
        print('[AI] Init object with Q table size of %d elements'%self.q_table.flatten().shape[0])

        super().__init__()

    def action(self, obs):
        if np.random.random() > self.epsilon:
            action = np.argmax(self.q_table[obs[0], obs[1], obs[2], obs[3], :])
        else:
            action = np.random.randint(0, self.action_space)

        return action

    def feedback(self, obs, new_obs, action, reward):
        max_future_q = np.max(self.q_table[obs[0], obs[1], obs[2], obs[3], :])
        current_q = self.q_table[obs[0], obs[1], obs[2], obs[3], action]

        new_q = (1 - self.LEARNING_RATE) * current_q + self.LEARNING_RATE * (reward + self.DISCOUNT * max_future_q)
        self.q_table[obs[0], obs[1], obs[2], obs[3], action] = new_q

    def to_disk(self, name):
        with open(name, "wb") as f:
            pickle.dump(self.q_table, f)

    def from_disk(self, name):
        with open(name, "rb") as f:
            return pickle.load(f)

    def episode_callback(self):
        self.epsilon *= self.EPS_DECAY

class Env():
    '''
    Environment - OpenGym interface
    https://gym.openai.com/docs/
    '''
    def seed(self, seed):
        '''
        Sets a seed that  governs the simulation's random aspects
        '''
        pass

    def reset(self):
        '''
        resets the state for a new episode.
        '''
        observation = None
        return observation

    def render(self):
        '''
        renders one frame  of the simulation.  The rendering is only for display and does not  affect reinforcement learning.
        '''
        pass

    def step(self, action):
        '''
        performs one step of the simulation
        '''
        observation = None
        reward = None
        done = None
        info = None
        return observation, reward, done, info

class Blob:
    def __init__(self, limit:int, color:tuple = (255,255,255)):
        self.x = 0
        self.y = 0
        self.limit = limit
        self.color = color

    def set_pos(self, x, y):
        self.x = x
        self.y = y

    def __str__(self):
        return f"{self.x}, {self.y}"

    def __sub__(self, other):
        return (self.x-other.x, self.y-other.y)

    def move(self, x, y):
        self.x += x
        self.y += y

        # Wrap border
        if self.x >= self.limit:
            self.x = 0
        if self.x<0:
            self.x = self.limit - 1
        if self.y >= self.limit:
            self.y = 0
        if self.y<0:
            self.y = self.limit - 1

class EnvBlob(Env):
    action_space = 4
    obs_size = None
    grid_size = None

    ENEMY_PENALTY = -100
    FOOD_REWARD = 300

    def __init__(self, obs_size:int = 3, grid_size:int = 10):
        self.obs_size = obs_size
        self.grid_size = grid_size

        self.player = Blob(color=(255, 175, 0), limit=self.grid_size)
        self.food = Blob(color=(0, 255, 0), limit=self.grid_size)
        self.enemy = Blob(color=(0, 0, 255), limit=self.grid_size)

        super().__init__()

    def reset(self):
        # generate unique tuples
        inits = [0]*6
        inits[0] = np.random.choice(self.grid_size)#np.random.default_rng().choice(self.grid_size, size=6, replace=False)
        inits[1] = np.random.choice(self.grid_size)
        while True:
            inits[2] = np.random.choice(self.grid_size)
            inits[3] = np.random.choice(self.grid_size)
            if inits[2]!=inits[0] or inits[3]!=inits[1]:
                break
        while True:
            inits[4] = np.random.choice(self.grid_size)
            inits[5] = np.random.choice(self.grid_size)
            if (inits[4]!=inits[0] or inits[5]!=inits[1]) and (inits[4]!=inits[2] or inits[5]!=inits[3]):
                break

        # Reset the agents
        self.player.set_pos(inits[0], inits[1])
        self.food.set_pos(inits[2], inits[3])
        self.enemy.set_pos(inits[4], inits[5])
        self._step_counter = 0
        self._done = False

    def observation(self):
        return (self.preprocess_obs(self.player.x-self.food.x), 
                self.preprocess_obs(self.player.y-self.food.y), 
                self.preprocess_obs(self.player.x-self.enemy.x), 
                self.preprocess_obs(self.player.y-self.enemy.y))

    def preprocess_obs(self, obs:int):
        '''
        Convert the env internal representation to what is actually shown to the agent
        '''
        max_obs = self.obs_size -1
        if obs>max_obs: obs = max_obs   # Saturation
        if obs<-max_obs: obs = -max_obs
        obs = obs+max_obs # make positive
        return obs

    def step(self, action):
        # Game mechanics
        if action == 0: self.player.move(x=1, y=0)
        elif action == 1: self.player.move(x=-1, y=0)
        elif action == 2: self.player.move(x=0, y=1)
        elif action == 3: self.player.move(x=0, y=-1)
        else:
            raise ValueError('Invalid action')

        # Reward function
        distance_enemy = self.player - self.enemy
        distance_food = self.player - self.food
        if distance_enemy[0]==0 and distance_enemy[1]==0:
            # exactly on the enemy
            reward = self.ENEMY_PENALTY
        elif distance_food[0]==0 and distance_food[1]==0:
            # exactly on the food
            reward = self.FOOD_REWARD
        #elif (distance_enemy[0]**2 + distance_enemy[1]**2)<3:
        #    # Near enemy
        #    reward = int(self.ENEMY_PENALTY/2)
        elif (distance_food[0]**2 + distance_food[1]**2)<3:
            # Near food
            reward = 0
        else:
        #    # Near food
            reward = -1
        #    reward = int(-0.1*self.FOOD_REWARD + 0.1*self.FOOD_REWARD/(distance_food[0]**2 + distance_food[1]**2))

        observation = self.observation()

        self._step_counter += 1
        if self._step_counter>=199 or reward == self.FOOD_REWARD or reward == self.ENEMY_PENALTY:
            self._done = True
        else:
            self._done = False

        info = None
        return observation, reward, self._done, info

    def render(self):
        grid = np.zeros((self.grid_size, self.grid_size, 3), dtype=np.uint8)  # starts an rbg of our size
        grid[self.food.x][self.food.y] = self.food.color
        grid[self.player.x][self.player.y] = self.player.color
        grid[self.enemy.x][self.enemy.y] = self.enemy.color
        img = Image.fromarray(grid, 'RGB')  # reading to rgb. Apparently. Even tho color definitions are bgr. ???
        img = img.resize((300, 300))  # resizing so we can see our agent in all its glory.
        cv2.imshow("image", np.array(img))  # show it!
        if self._done:  # wait some time at the end of the episode
            if cv2.waitKey(1000) & 0xFF == ord('q'):
                return None
        else:
            if cv2.waitKey(100) & 0xFF == ord('q'):
                return None


           

env = EnvBlob(obs_size=SIZE, grid_size=GRID_SIZE)
AI = IntelligentAgent(start_q_table=start_q_table, obs_size=env.obs_size, epsilon=EPSILON)

episode_rewards = []
for episode in range(HM_EPISODES):
    env.reset()

    assert env.player.x != env.enemy.x or env.player.y != env.enemy.y, 'Impossible game generated'
    assert env.player.x != env.food.x or env.player.y != env.food.y, 'Impossible game generated'
    assert env.enemy.x != env.food.x or env.enemy.y != env.food.y, 'Impossible game generated'

    episode_reward = 0
    done = False

    # Play the game within episode
    while not done:
        obs = env.observation()

        # Take the action!
        action = AI.action(obs)
        #enemy.action() ??

        new_obs, reward, done, info = env.step(action)
        AI.feedback(obs, new_obs, action, reward)
        episode_reward += reward

        if episode % SHOW_EVERY == 0 and episode!=0:
            print('------------', np.random.random())
            print(env.player)
            print(env.food)
            print(env.enemy)
            print(new_obs)
            print(reward)
            env.render()
    if episode % LOG_EVERY == 0:
        print("on ep. %d, mean reward is %.2f (epsilon is %.3f)"%(episode, np.mean(episode_rewards[-LOG_EVERY:]), AI.epsilon))

    episode_rewards.append(episode_reward)
    AI.episode_callback()
    #break

AI.to_disk(f'model.pickle')

moving_avg = np.convolve(episode_rewards, np.ones((LOG_EVERY,))/LOG_EVERY, mode='valid')
plt.plot([i for i in range(len(moving_avg))], moving_avg)
plt.ylabel(f"Reward {LOG_EVERY}ma")
plt.xlabel("episode #")
plt.show()
#AI.to_disk(f'model-{int(time.time())}.pickle')



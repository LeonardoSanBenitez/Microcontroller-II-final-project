import numpy as np
from PIL import Image
import cv2
import matplotlib.pyplot as plt
import pickle
from matplotlib import style
import time

# Config train
SIZE = 4
HM_EPISODES = 30000
LOG_EVERY = 3000  
SHOW_EVERY = 80000 # how often to play through env visually.
start_q_table = None #'model-1616452736.pickle' #None # None or Filename
EPSILON = 0.9

# Config deploy
#start_q_table = 'model.pickle' #None #'model-1616452736.pickle' #None # None or Filename
#EPSILON = 0.0
#SHOW_EVERY = 1


class IntelligentAgent():
    LEARNING_RATE = 0.15
    DISCOUNT = 0.95
    epsilon = 0.9
    EPS_DECAY = 0.9998

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
            with open(start_q_table, "rb") as f:
                self.q_table = pickle.load(f)
        # can look up from Q-table with: print(self.q_table[((-9, -2), (3, 9))]) for example
        #TODO:print('[AI] Init object with Q table size of %d elements'%(len(self.q_table)*len(list(self.q_table.values())[0])))

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
    def __init__(self):
        self.x = 0
        self.y = 0

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

class EnvBlob(Env):
    action_space = 4
    obs_size = None
    grid_size = None

    PLAYER_N = 1  # player key in dict
    FOOD_N = 2  # food key in dict
    ENEMY_N = 3  # enemy key in dict

    MOVE_PENALTY = 1
    ENEMY_PENALTY = 300
    FOOD_REWARD = 25

    # Color dict
    d = {1: (255, 175, 0),
        2: (0, 255, 0),
        3: (0, 0, 255)}

    def __init__(self, obs_size:int = 3, grid_size:int = 10):
        self.player = Blob()
        self.food = Blob()
        self.enemy = Blob()
        self.obs_size = obs_size
        self.grid_size = grid_size
        super().__init__()

    def reset(self):
        inits = np.random.default_rng().choice(self.obs_size, size=6, replace=True)
        self.player.set_pos(inits[0], inits[1])
        self.food.set_pos(inits[2], inits[3])
        self.enemy.set_pos(inits[4], inits[5])
        self._step_counter = 0
        self._done = False

    def observation(self):
        # TODO: calculate observation based on the grid_state
        # Maybe something along the line:
        #def saturation(x, s):
        #    if x>s:
        #        return s
        #    else:
        #        return x
        return (self.player.x-self.food.x+self.obs_size-1, 
                self.player.y-self.food.y+self.obs_size-1, 
                self.player.x-self.enemy.x+self.obs_size-1, 
                self.player.y-self.enemy.y+self.obs_size-1)

    def step(self, action):
        # Game mechanics
        if action == 0 and self.player.x < (self.obs_size - 1):
            self.player.move(x=1, y=0)
        elif action == 1 and self.player.x>0:
            self.player.move(x=-1, y=0)
        elif action == 2 and self.player.y < (self.obs_size - 1):
            self.player.move(x=0, y=1)
        elif action == 3 and self.player.y>0:
            self.player.move(x=0, y=-1)

        # Reward function
        if self.player.x == self.enemy.x and self.player.y == self.enemy.y:
            reward = -self.ENEMY_PENALTY
        elif self.player.x == self.food.x and self.player.y == self.food.y:
            reward = self.FOOD_REWARD
        else:
            reward = -self.MOVE_PENALTY

        observation = self.observation()

        self._step_counter += 1
        if self._step_counter>=199 or reward == self.FOOD_REWARD or reward == -self.ENEMY_PENALTY:
            self._done = True
        else:
            self._done = False

        info = None
        return observation, reward, self._done, info

    def render(self):
        grid = np.zeros((self.obs_size, self.obs_size, 3), dtype=np.uint8)  # starts an rbg of our size
        grid[self.food.x][self.food.y] = self.d[self.FOOD_N]  # sets the food location tile to green color
        grid[self.player.x][self.player.y] = self.d[self.PLAYER_N]  # sets the player tile to blue
        grid[self.enemy.x][self.enemy.y] = self.d[self.ENEMY_N]  # sets the enemy location to red
        img = Image.fromarray(grid, 'RGB')  # reading to rgb. Apparently. Even tho color definitions are bgr. ???
        img = img.resize((300, 300))  # resizing so we can see our agent in all its glory.
        cv2.imshow("image", np.array(img))  # show it!
        if self._done:  # wait some time at the end of the episode
            if cv2.waitKey(1000) & 0xFF == ord('q'):
                return None
        else:
            if cv2.waitKey(100) & 0xFF == ord('q'):
                return None


           

env = EnvBlob(obs_size=SIZE)
AI = IntelligentAgent(start_q_table=start_q_table, obs_size=env.obs_size, epsilon=EPSILON)

episode_rewards = []
for episode in range(HM_EPISODES):
    env.reset()
    episode_reward = 0
    done = False

    # Play the game within episode
    while not done:
        obs = env.observation()
        #print(obs)
        # Take the action!
        action = AI.action(obs)
        #enemy.action() ??

        new_obs, reward, done, info = env.step(action)
        AI.feedback(obs, new_obs, action, reward)
        episode_reward += reward

        if episode % SHOW_EVERY == 0 and episode!=0:
            env.render()
    if episode % LOG_EVERY == 0:
        print("on ep. %d, mean reward is %.2f (epsilon is %.3f)"%(episode, np.mean(episode_rewards[-LOG_EVERY:]), AI.epsilon))

    episode_rewards.append(episode_reward)
    AI.episode_callback()
    #break
moving_avg = np.convolve(episode_rewards, np.ones((LOG_EVERY,))/LOG_EVERY, mode='valid')
plt.plot([i for i in range(len(moving_avg))], moving_avg)
plt.ylabel(f"Reward {LOG_EVERY}ma")
plt.xlabel("episode #")
plt.show()
#AI.to_disk(f'model-{int(time.time())}.pickle')
AI.to_disk(f'model.pickle')


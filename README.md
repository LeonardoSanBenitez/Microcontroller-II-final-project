# Microcontroller-II-final-project
Didactic project about Reinforcement Learning: an arcade game implemented with an STM32 microcontroller and a graphical display (like those for old Nokia cellphones), where your goal is to prevent an AI-controlled agent to reach a given point in the map.

The AI agent (blue) aims to navigate its way as quickly as possible to the target (green), while avoiding the player (red):

![gif](demo_python.gif)

![gif](demo_stm.gif)

## AI problem formulation
The AI agent has partial knowledge about the environment: it knows the x and y deltas for the food and enemy, with precision depending on the distance. If the food or the enemy is far away, the agent only knows its general direction. The agent can execute 4 actions: move up, down, left or right. 

The Q table can be implemented as a dictionary `[[s0,s1],[s2,s3]] -> [a0, a1, a2, a3]`, or as a vector of 5 dimentions. If implemented as a vector, it will have $ACTION_SIZE*(OBS_SIZE*2 -1)^4$ elements. In the STM implementation we used OBS_SIZE=3 and ACTION_SIZE=4, therefore $len(q) = 2500$


# Lessons learned
By making the environment partially observable, learning became more difficult. The ideal would be to have a probabilistic representation of the environment (ex: HMM), and not deterministic as is the Q-table. 

By changing the possible actions (not diagonal), learning became easier. The fact that planning is only 1 step ahead seems to make it very difficult to learn on large maps.

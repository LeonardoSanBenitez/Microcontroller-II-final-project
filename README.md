# Microcontroller-II-final-project

The plan is to have a player blob (blue), which aims to navigate its way as quickly as possible to the food blob (green), while avoiding the enemy blob (red).

 our environment will be a 20 x 20 grid, where we have 1 player, 1 enemy, and 1 food. For now, we'll just have the player able to move

A 10x10 Q-Table for example, in this case, is ~15MB. A 20x20 is ~195MB
I wanted to see how much of a difference going from 10x10 to a 20x20 would make. As we saw, somewhere between 25k and 75K training episodes were required for the 10x10 to learn. I found it took more like 2.5 million episodes for a 20x20 model to learn.


# TODO STM
* Rodar um hello world com OO
* Alocar um vetor de $4*(2*2 -1)^4=324$ e $4*(2*3-1)^4=q$ inteiros com sinal

# TODO RL
* desassociar obervation_space (o que o agente vê) da grid_space (o que o ambiente sabe)
* Usar uma matriz (e não association map) para armazenar a qtable
* Reformular o observation space para consumir o mínimo de memória possível

# Formulação da Q table
we simply pass the x and y deltas for the food and enemy to our agent.
4 actions
dicionário `[[s0,s1],[s2,s3]] -> [a0, a1, a2, a3]`
size 3 -> len(q) = 625 estados possíveis (keys) = (SIZE*2 -1)**4

# Lições
ao tornar o ambiente parcialmente observável, o aprendizado ficou muito mais difícil
O ideal seria ter uma representação PROBABILISTICA do ambiente (ex: HMM), e não determinstítca

Ao mudar as ações possíveis (não diagonal), o aprendizado ficou mais fácil

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

## Size 3
on ep. 0, mean reward is nan (epsilon is 0.900)
on ep. 3000, mean reward is -90.15 (epsilon is 0.494)
on ep. 6000, mean reward is -43.20 (epsilon is 0.271)
on ep. 9000, mean reward is -25.77 (epsilon is 0.149)
on ep. 12000, mean reward is -19.61 (epsilon is 0.082)
on ep. 15000, mean reward is -13.44 (epsilon is 0.045)
on ep. 18000, mean reward is -7.70 (epsilon is 0.025)
on ep. 21000, mean reward is -5.25 (epsilon is 0.013)
on ep. 24000, mean reward is -1.26 (epsilon is 0.007)

## Size 3, no special final reward, longer training
on ep. 3000, mean reward is -91.97 (epsilon is 0.494)
on ep. 6000, mean reward is -51.47 (epsilon is 0.271)
on ep. 9000, mean reward is -31.59 (epsilon is 0.149)
on ep. 12000, mean reward is -20.12 (epsilon is 0.082)
on ep. 15000, mean reward is -15.97 (epsilon is 0.045)
on ep. 18000, mean reward is -4.86 (epsilon is 0.025)
on ep. 21000, mean reward is -5.68 (epsilon is 0.013)
on ep. 24000, mean reward is -1.78 (epsilon is 0.007)
on ep. 27000, mean reward is -4.25 (epsilon is 0.004)





## SIZE = 3, GRID_SIZE = 10, 30k episodes, reward  Near food
on ep. 0, mean reward is nan (epsilon is 0.900)
on ep. 3000, mean reward is 123.05 (epsilon is 0.775)
on ep. 6000, mean reward is 194.16 (epsilon is 0.667)
on ep. 9000, mean reward is 223.99 (epsilon is 0.574)
on ep. 12000, mean reward is 237.12 (epsilon is 0.494)
on ep. 15000, mean reward is 252.09 (epsilon is 0.425)
on ep. 18000, mean reward is 260.69 (epsilon is 0.366)
on ep. 21000, mean reward is 266.70 (epsilon is 0.315)
on ep. 24000, mean reward is 269.16 (epsilon is 0.271)
on ep. 27000, mean reward is 276.57 (epsilon is 0.233)

## Same as beore but correcting new_obs
on ep. 0, mean reward is nan (epsilon is 0.900)
on ep. 3000, mean reward is 136.09 (epsilon is 0.775)
on ep. 6000, mean reward is 188.19 (epsilon is 0.667)
on ep. 9000, mean reward is 216.11 (epsilon is 0.574)
on ep. 12000, mean reward is 241.12 (epsilon is 0.494)
on ep. 15000, mean reward is 250.25 (epsilon is 0.425)
on ep. 18000, mean reward is 256.99 (epsilon is 0.366)
on ep. 21000, mean reward is 263.25 (epsilon is 0.315)
on ep. 24000, mean reward is 264.99 (epsilon is 0.271)
on ep. 27000, mean reward is 269.56 (epsilon is 0.233)
(ifscMCC) benitez@benitez-Aspire-A515-51:~/Desktop/Microcontroller-II-final-project/game-local$ git add -A
(ifscMCC) benitez@benitez-Aspire-A515-51:~/Desktop/Microcontroller-II-final-project/game-local$ git commit -m 'implementing action'
[main cd3ab24] implementing action
 2 files changed, 126 insertions(+), 90 deletions(-)
 create mode 100644 game-local/game_config.h
(ifscMCC) benitez@benitez-Aspire-A515-51:~/Desktop/Microcontroller-II-final-project/game-local$ python main.py 
[AI] Init object with Q table size of 2500 elements
/home/benitez/anaconda3/envs/ifscMCC/lib/python3.6/site-packages/numpy/core/fromnumeric.py:3335: RuntimeWarning: Mean of empty slice.
  out=out, **kwargs)
/home/benitez/anaconda3/envs/ifscMCC/lib/python3.6/site-packages/numpy/core/_methods.py:161: RuntimeWarning: invalid value encountered in double_scalars
  ret = ret.dtype.type(ret / rcount)
on ep. 0, mean reward is nan (epsilon is 0.900)
on ep. 3000, mean reward is 144.75 (epsilon is 0.775)
on ep. 6000, mean reward is 203.69 (epsilon is 0.667)
on ep. 9000, mean reward is 235.79 (epsilon is 0.574)
on ep. 12000, mean reward is 253.45 (epsilon is 0.494)
on ep. 15000, mean reward is 260.59 (epsilon is 0.425)
on ep. 18000, mean reward is 270.65 (epsilon is 0.366)
on ep. 21000, mean reward is 274.36 (epsilon is 0.315)
on ep. 24000, mean reward is 280.52 (epsilon is 0.271)
on ep. 27000, mean reward is 280.39 (epsilon is 0.233)
(ifscMCC) benitez@benitez-Aspire-A515-51:~/Desktop/Microcontroller-II-final-project/game-local$ python main.py 
[AI] Init object with Q table size of 2500 elements
/home/benitez/anaconda3/envs/ifscMCC/lib/python3.6/site-packages/numpy/core/fromnumeric.py:3335: RuntimeWarning: Mean of empty slice.
  out=out, **kwargs)
/home/benitez/anaconda3/envs/ifscMCC/lib/python3.6/site-packages/numpy/core/_methods.py:161: RuntimeWarning: invalid value encountered in double_scalars
  ret = ret.dtype.type(ret / rcount)
on ep. 0, mean reward is nan (epsilon is 0.900)
on ep. 3000, mean reward is 144.39 (epsilon is 0.775)
on ep. 6000, mean reward is 208.71 (epsilon is 0.667)
on ep. 9000, mean reward is 236.41 (epsilon is 0.574)
on ep. 12000, mean reward is 252.80 (epsilon is 0.494)
on ep. 15000, mean reward is 264.07 (epsilon is 0.425)
on ep. 18000, mean reward is 269.98 (epsilon is 0.366)
on ep. 21000, mean reward is 274.00 (epsilon is 0.315)
on ep. 24000, mean reward is 278.05 (epsilon is 0.271)
on ep. 27000, mean reward is 279.49 (epsilon is 0.233)
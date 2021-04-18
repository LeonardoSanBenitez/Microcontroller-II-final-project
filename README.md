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
The obervation is simply the x and y deltas for the food and enemy, and there are 4 actions. Pode ser implementado como um dicionário `[[s0,s1],[s2,s3]] -> [a0, a1, a2, a3]`, ou como um vetor de 5 dimensões.

OBS_SIZE 3 -> len(q) = 2500 estados possíveis 
ACTION_SIZE*(OBS_SIZE*2 -1)^4


# Lições
ao tornar o ambiente parcialmente observável, o aprendizado ficou muito mais difícil
O ideal seria ter uma representação PROBABILISTICA do ambiente (ex: HMM), e não determinstítca

Ao mudar as ações possíveis (não diagonal), o aprendizado ficou mais fácil
O fato do planejamento ser apenas *1 step head* parece tornar bem difícil aprender em mapas grandes
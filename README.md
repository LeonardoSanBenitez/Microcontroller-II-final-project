# Microcontroller-II-final-project

didactic project about Reinforcement Learning: an arcade game implemented with an STM32 microcontroller and a graphical display (like those ones for old Nokia cellphones), where your goal what to prevent an AI-controlled agent to reach a given point in the map.

The AI agent (blue) aims to navigate its way as quickly as possible to the target (green), while avoiding the player (red):

![gif](demo_python.gif)

## Formulação da Q table

The obervation is simply the x and y deltas for the food and enemy, and there are 4 actions. Pode ser implementado como um dicionário `[[s0,s1],[s2,s3]] -> [a0, a1, a2, a3]`, ou como um vetor de 5 dimensões.

$ACTION_SIZE*(OBS_SIZE*2 -1)^4$

In the STM implementation we used OBS_SIZE=3 and ACTION_SIZE=4, therefore $len(q) = 2500$

# Lições aprendidas

Ao tornar o ambiente parcialmente observável, o aprendizado ficou muito mais difícil
O ideal seria ter uma representação PROBABILISTICA do ambiente (ex: HMM), e não determinística como é a Q-table.

Ao mudar as ações possíveis (não diagonal), o aprendizado ficou mais fácil.

O fato do planejamento ser apenas *1 step head* parece tornar bem difícil aprender em mapas grandes.
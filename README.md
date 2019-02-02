# Megatron747

Megatron747 is a bot playing the 2 player 'Chain Reaction' game. It was developed for the AI Contest : BUET CSE FEST 2019.

AI Contest Problem Specification: https://bit.ly/2t42ClA

AI Contest FB Event: https://bit.ly/2t1jNUF

Megatron747.cpp contains the code of the bot. Go through the problem specification link given above for the instructions to run the code. If you want to play with the bot by providing input from keyboard, you can use consolePlayer.cpp.

Currently there are 4 bots implemented in Megatron747.cpp

bot0 -> plays randomly

bot11 -> tries to maximize the score1() in exactly one move. score1() returns the number of atoms of the current player in the grid.

bot21 -> went 2 steps deeper with minimax (obviously alpha beta pruning was in action), using score1() as heuristic

bot31 -> went arbitrary steps deeper with bot21. The number of steps will be determined from time limit. bot31 is basically iterative deepening of bot21. 

bot31 is the bot used by default.

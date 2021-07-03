# Checkers-AI-CPP


Predicts the best move for a particular state of the game using **min-max algorithm optimised with alpha beta pruning**. 
Contain two types of chips: **Red** and **Blue**.

## How to run

```
g++ main.cpp -o main
./a < input.txt
```
-In the input.txt file add p (Whose turn is it('r' for red, 'b' for blue)) and n (dimensions of the board)
followed by n x n character array denoting the state of the game

-The grid follows 0-indexing starting from top left corner x denoting the row and y denoting the column number

-You will get the output as no of moves, the position of the coin and the n lines listing the consecutive positions to move to



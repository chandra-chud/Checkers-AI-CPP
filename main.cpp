#include<bits/stdc++.h>
#include "Move.h"
#include "Chip.h"
#include "State.h"
#include "Player.h"

using namespace std;


int main(){
    //Whose turn is it
    char p; cin >> p;

    //Size of the side of the square
    int n; cin >> n;


    auto b = State(n, p);
    SmartPlayer mastermind(b, p, 7);

    //First list the no of moves, the position of the coin
    //And the n lines listing the positions to 
    mastermind.move();
}

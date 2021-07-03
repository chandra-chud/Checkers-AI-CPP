#ifndef PLAYER_H
#define PLAYER_H

#include<bits/stdc++.h>
#include "Chip.h"
#include "Move.h"
#include "State.h"
using namespace std;

class Player{
    protected:
        State state_;
        char player_;

    public:
        Player() {}

        Player(State b, char player){
            state_ = b;
            player_ = player;
        }

        Move virtual queryOptimumMove(){
            return Move({}, {}, false);
        }

        void move(){
            queryOptimumMove().performMove();
        }

        //Two players classes extended from the base class

        friend class SmartPlayer;
};


//The main AI algorithm with minimax algorithm optimised with alpha beta pruning 
class SmartPlayer : public Player{
    private:
        size_t depth_;
        bool debug_ = false;

    public:
        SmartPlayer(State b, char player, size_t depth) : Player(b, player){
            depth_ = depth;
        }

        Move virtual queryOptimumMove(){

            auto stuff = queryOptimumMoveRecursive(state_, 1, 1);

            if (debug_){
                cout<<"final returned score : %f"<<stuff.second<<endl;
            }

            return stuff.first;
        }

        // minimax algorithm optimised with alpha beta pruning
        // depth starts at 1, recursion ends when depth greater than depth_
        // maximizing refers to the level of the search tree and which player
        // is in control at that point in the game.

        pair<Move, float> queryOptimumMoveRecursive(State b, int depth, int maximizing, 
        float alpha = FLT_MIN, float beta = FLT_MAX){
            if (depth > depth_){
                throw;
            }

            // initialise  search parameters
            Move optimumMove({}, {}, false);
            float optimumScore = (maximizing ? -1000. : 1000.);

            //Of all the possible moves

            for (auto move : b.queryValidMoves()){

                // To calculate the score of the subtree 
                //Used to rank the positions
                float currScore;
                Move thisMove({}, {}, false);
                auto new_state = b.forecastMove(move);
                stateCalculator preCalculator(new_state);

                //check of the current state is the last

                if (depth < depth_ and not preCalculator.gameOver()){

                    //To switch turns of the players
                    new_state.switchPerspective();

                    auto moveAndScore = queryOptimumMoveRecursive(new_state,
                                                                    depth + 1, 1 - maximizing, alpha, beta);
                    thisMove = moveAndScore.first;
                    currScore = moveAndScore.second;
                } else {

                    // this is either truncated by depth or a leaf node (game over)
                    // whenver the score is computed, it should *always* be
                    // from the perpective of the original (maximizing) player.
                    // this is imperative for the minimax algo to work correctly.
                    new_state.switchPerspectiveToPlayer(player_);
                    stateCalculator calculator(new_state);
                    currScore = calculator.score();
                }

                if (debug_){
                    move.print();
                    cout<<"depth: "<<depth<<"maximizing: "<<maximizing<<"currScore: "<<currScore<<endl;
                }

                // minimax
                if (maximizing == 1){
                    if (currScore > optimumScore){
                        optimumMove = move;
                        optimumScore = currScore;
                    }
                } else {
                    if (currScore < optimumScore) {
                        optimumMove = move;
                        optimumScore = currScore;
                    }
                }

                //Optimising the algorithm using Alpha Beta Pruning

                if (maximizing){
                    alpha = max(alpha, currScore);
                } else {
                    beta = min(beta, currScore);
                }

                if (alpha >= beta){
                    break;
                }
            }

            return {optimumMove, optimumScore};
        }
};




#endif
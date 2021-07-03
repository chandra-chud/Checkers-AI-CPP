#ifndef CHIP_H
#define CHIP_H

#include<bits/stdc++.h>
using namespace std;

// contains x, y coordinates, type, whether the chip is a promoted
// and which chips it can kill

class Chip{
    private:
        int X_, Y_;
        char type_;
        bool isPromoted_;
        set<char> canKill_;
        int direction_;

    public:
        Chip(int x, int y, char player){
            X_ = x;
            Y_ = y;
            type_ = player;
            type_ = (type_ == 'B' ? 'b' : type_);
            type_ = (type_ == 'R' ? 'r' : type_);
            isPromoted_ = (player == 'B' or player == 'R' ? true : false);

            if (player == 'r' or player == 'R'){
                canKill_ = {'b', 'B'};
            } else {
                canKill_ = {'r', 'R'};
            }

            // Red chips are present at bottom and 
            //the blue chips are present at the top of the board.
            direction_ = (type_ == 'b' ? 1 : -1);
        }

        char queryType() const { return type_; }
        bool queryIsPromoted() const { return isPromoted_; }
        int queryDirection() const { return direction_; }
        int queryX() const { return X_; }
        int queryY() const { return Y_; }
        set<char> queryCanKill() const { return canKill_; }
};

#endif
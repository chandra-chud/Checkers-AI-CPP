#ifndef MOVE_H
#define MOVE_H

#include<bits/stdc++.h>
using namespace std;

// each move has a starting point and path to completion
// Its necessary to maintain a set of killed pieces to avoid 
// infinite loop in recursion

class Move : public pair<pair<int, int>, vector<pair<int, int>>>{

    private:
        bool promoted_ = false;
        set<pair<int, int>> kills_;

    public:
        Move(pair<int, int> pos, vector<pair<int, int>> path, bool promoted, set<pair<int, int>> kills = {}) : 
        pair<pair<int, int>, vector<pair<int, int>>>(pos, path){
            promoted_ = promoted;
            kills_ = kills;
        }

        void print(){
            cout<<"Move: ("<<this->first.first<<", "<<this->first.second<<endl;

            for (auto &pos : this->second)
            cout<<" ("<<pos.first<<", "<<pos.second<<") "<<endl;

            cout<<endl;
        }

        void performMove(){
            cout<<this->second.size()<<endl;

            cout<<this->first.first<<", "<<this->first.second<<endl;

            for (auto &pos : this->second)
            cout<<"( "<<pos.first<<", "<<pos.second<<") "<<endl;
        }

        void grantPromotion() { promoted_ = true; }
        bool queryPromoted() const { return promoted_; }
        void kill(pair<int, int> pos) { kills_.emplace(pos); }
        set<pair<int, int>> queryKills() { return kills_; }
        void addOneMovement(pair<int, int> new_pos) { this->second.push_back(new_pos); }
        pair<int, int> queryOrigin() const { return this->first; }
        pair<int, int> queryShiftedPosition(int shift) const{
            int n = this->second.size();
            if (n >= shift){
                return this->second[n - shift];
            } else {
                throw;
            }
        }

        bool hasKilled(pair<int, int> opponent_pos) const{
            return kills_.find(opponent_pos) != kills_.end();
        }
};


//Just and overloaded addition operation to aid the easy addition of the moves vector

class Moves : public vector<Move>{
    public:
        void operator+=(Moves B){
            for (Move &curr_move : B){
                this->push_back(curr_move);
            }
        }
};


#endif
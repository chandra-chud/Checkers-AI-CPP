#ifndef STATE_H
#define STATE_H

#include<bits/stdc++.h>
#include "Chip.h"
#include "Move.h"
// #include "Moves.h"

using namespace std;

// Class for the checkers state
// From a single players perspective
// Depends on the current player, no of pieces and their positions

class State{

    private:
        int n_;
        char player_;
        char myPawn_;
        char myPromotedPawn_;
        char opponentPawn_;
        char opponentPromotedPawn_;
        vector<vector<char>> state_;

        //Keep track of the currnet player's chips
        vector<Chip> chips_;

        //For instant checking of the category of the chips
        set<char> myChips_;
        set<char> opponentChips_;

        // to enable debug and see the final values
        bool debug_ = false;

        void designateChips(){
            // seach for my chips and initialise them
            for (int i = 0; i < n_; i++){
                for (int j = 0; j < n_; j++){
                    if (myChips_.find(state_[i][j]) != myChips_.end())
                        chips_.push_back(Chip(i, j, state_[i][j]));
                }
            }
        }

        void readStateFromInput(){
            state_.resize(n_);
            for (int i = 0; i < n_; i++){
                state_[i].resize(n_);
                for (int j = 0; j < n_; j++){
                    cin >> state_[i][j];
                }
            }
            designateChips();
        }

        void readStateFromString(string &s){
            state_.resize(n_);
            for (int i = 0; i < n_; i++){
                state_[i].resize(n_);
                for (int j = 0; j < n_; j++){
                    state_[i][j] = char(s[i * n_ + j]);
                }
            }
            designateChips();
        }

        void designateNandPlayer(int n, char player){
            n_ = n;
            player_ = player;
            if (player == 'r' or player == 'R'){
                myPawn_ = 'r';
                myPromotedPawn_ = 'R';
                opponentPawn_ = 'b';
                opponentPromotedPawn_ = 'B';
                myChips_ = {'r', 'R'};
                opponentChips_ = {'b', 'B'};
            } else { 
                myPawn_ = 'b';
                myPromotedPawn_ = 'B';
                opponentPawn_ = 'r';
                opponentPromotedPawn_ = 'R';
                myChips_ = {'b', 'B'};
                opponentChips_ = {'r', 'R'};
            }
        }

        void setState(vector<vector<char>> state) { state_ = state; }

    public:
        State() {}

        State(int n, char player){
            designateNandPlayer(n, player);
            readStateFromInput();
        }

        State(int n, char player, string state){
            designateNandPlayer(n, player);
            readStateFromString(state);
        }

        vector<pair<int, int>> queryPlayerPositions() const{
            vector<pair<int, int>> positions;
            for (auto &chip : chips_){
                if (chip.queryType() == player_){
                    positions.push_back({chip.queryX(),
                                        chip.queryY()});
                }
            }
            return positions;
        }

        Moves queryValidMoves() const{
            Moves allValidMoves;

            // first check killing moves
            for (auto &chip : chips_){
                if (chip.queryType() == player_){
                    auto currMoves = queryValidKillingMovesThisChip(chip);
                    if (debug_){
                        cout<<"Just acquired "<<currMoves.size()<<"killing moves from position ("<<chip.queryX()<<", "<<chip.queryY()<<") \n\n";
                    }

                    allValidMoves += currMoves;
                }
            }

            // only if there were no killing moves, look for non-killing moves
            // This ensures increases the scores as killing increases score
            if (allValidMoves.size() == 0){
                for (auto &chip : chips_){
                    if (chip.queryType() == player_){
                        auto currMoves = queryValidNonKillingMovesThisChip(chip);
                        if (debug_){
                            printf("Just acquired %lu non-killing moves from position (%d, %d)\n\n",
                                currMoves.size(), chip.queryX(), chip.queryY());
                        }

                        allValidMoves += currMoves;
                    }
                }
            }

            if (debug_){
                printf("returning %lu moves\n", allValidMoves.size());
            }

            return allValidMoves;
        }

        Moves queryValidNonKillingMovesThisChip(Chip chip) const{
            //Check all the valid non killing moves
            const int i = chip.queryX();
            const int j = chip.queryY();
            Moves viableMovesFromHere;
            Move emptyMoveFromHere({i, j}, {}, chip.queryIsPromoted());

            //Checks diagonal moves in all four directions

            vector<int> shifts;
            if (chip.queryIsPromoted()){
                shifts = {1, -1};
            } else {
                shifts = {chip.queryDirection()};
            }

            for (int xShift : shifts) {
                for (int yShift : {-1, 1}) {

                    pair<int, int> pos = {i + xShift, j + yShift};
                    Move new_move = emptyMoveFromHere;
                    new_move.addOneMovement(pos);

                    // If out of bound skip it
                    if (not(pos.first < n_ and pos.first >= 0 and
                            pos.second < n_ and pos.second >= 0)){
                        continue;
                    }

                    // If some chip is already present skip it
                    if (state_[pos.first][pos.second] != '_'){
                        continue;
                    }

                    // If chip has reached the end promote it
                    if (pos.first == 0 or pos.first == n_ - 1){
                        new_move.grantPromotion();
                    }

                    viableMovesFromHere.push_back(new_move);
                }
            }
            return viableMovesFromHere;
        }

        // This is an recursive function after every kill there could be multiple kill
        // so we need to use recursion to find out after every kill if we can kill again
        //This is the starter function for the recursion
        Moves queryValidKillingMovesThisChip(Chip chip) const{

            pair<int, int> initLastPos = {-1, -1};
            const int i = chip.queryX();
            const int j = chip.queryY();

            Move emptyMoveFromHere({i, j}, {}, chip.queryIsPromoted());
            Moves initialMovesFromHere =
                querySingleDirectedKills(chip, i, j, initLastPos,
                                            emptyMoveFromHere);

            if (debug_){
                printf("How many initial moves were found? %lu\n", initialMovesFromHere.size());
            }

            Moves allValidMovesThisChip;
            for (auto move : initialMovesFromHere){
                allValidMovesThisChip +=
                    queryValidKillingMovesThisChipRecursive(chip, {i, j}, move);
            }
            return allValidMovesThisChip;
        }

        //Actual recursive function to do the killing

        Moves queryValidKillingMovesThisChipRecursive(Chip &chip,
                                                            pair<int, int> lastPos, Move &initialMove) const{

            if (debug_){
                printf("recursing...  ");
                initialMove.print();
            }

            vector<vector<pair<int, int>>> new_paths;
            pair<int, int> thisPos = initialMove.queryShiftedPosition(1);
            if (debug_){
                printf("(%d, %d) \n", thisPos.first, thisPos.second);
            }

            Moves movesFromHere = querySingleDirectedKills(chip,
                                                                thisPos.first, thisPos.second, lastPos, initialMove);

            Moves allValidMovesThisChip;
            if (movesFromHere.size() == 0){
                //add the completed moves in to this 
                allValidMovesThisChip.push_back(initialMove);
            } else {
                //Add all the valid moves
                for (auto move : movesFromHere){
                    allValidMovesThisChip +=
                        queryValidKillingMovesThisChipRecursive(chip,
                                                                    move.queryShiftedPosition(2), move);
                }
            }

            return allValidMovesThisChip;
        }

        Moves querySingleDirectedKills(Chip &chip, const int i,
                                        const int j, pair<int, int> &lastPos, Move initialMove) const{

            Moves successfulKillingMoves;
            const auto killableChips = chip.queryCanKill();

            vector<int> xShifts;
            if (chip.queryIsPromoted() or initialMove.queryPromoted()){
                xShifts = {1, -1};
            } else {
                xShifts = {chip.queryDirection()};
            }

            for (int xShift : xShifts){
                if (debug_){
                    printf("checpromote %d shifted kills for moves originating from (%d, %d)\n", xShift, i, j);
                }

                for (int yShift : {-1, 1}){
                    pair<int, int> pos = {i + 2 * xShift, j + yShift * 2};

                    // If out of bound skip it
                    if (not(pos.first < n_ and pos.first >= 0 and
                            pos.second < n_ and pos.second >= 0)){
                        continue;
                    }

                    // If some chip is already present skip it
                    if (state_[pos.first][pos.second] != '_'){
                        continue;
                    }

                    if (debug_){
                        printf("shifted to: (%d, %d)\n", pos.first, pos.second);
                    }

                    pair<int, int> opponent_pos = {i + xShift, j + yShift};
                    if (killableChips.find(state_[opponent_pos.first][opponent_pos.second]) !=
                            killableChips.end() and
                        not initialMove.hasKilled(opponent_pos)){


                        Move new_move = initialMove;
                        new_move.addOneMovement(pos);
                        new_move.kill(opponent_pos);

                        // was a King created?
                        if (pos.first == 0 or pos.first == n_ - 1){
                            new_move.grantPromotion();
                        }
                        if (debug_){
                            printf("chip killed!\n");
                        }

                        successfulKillingMoves.push_back(new_move);
                    }
                }
            }
            return successfulKillingMoves;
        }

        State forecastMove(Move m){
            //Use the already present state
            State new_State = *this;
            vector<vector<char>> new_state = state_;

            // remove killed chips
            for (auto kill : m.queryKills()){
                new_state[kill.first][kill.second] = '_';
            }

            // Update the status of the current chip
            pair<int, int> originPos = m.queryOrigin();
            pair<int, int> finalPos = m.queryShiftedPosition(1);
            new_state[finalPos.first][finalPos.second] =
                new_state[originPos.first][originPos.second];
            new_state[originPos.first][originPos.second] = '_';

            //If the move caused promotion
            if (m.queryPromoted()){
                new_state[finalPos.first][finalPos.second] = myPromotedPawn_;
            }

            //Assign the new state 
            new_State.setState(new_state);
            return new_State;
        }

        string queryStateString(){
            string s;
            for (auto row : state_){
                for (auto c : row){
                    s += c;
                }
            }

            return s;
        }

        void print(){
            for (auto row : state_){
                for (auto c : row){
                    cout<<c;
                }

                cout<<endl;
            }
        }

        char queryPlayer() { return player_; }

        void switchPerspective(){

            // switches the perspective of the state
            player_ = (player_ == 'b' ? 'r' : 'b');
            designateNandPlayer(n_, player_);

            // chips_ tracks a players Peices, not the opponent
            chips_.clear();
            designateChips();
        }

        void switchPerspectiveToPlayer(char player){

            // switches the perspective of the state
            player_ = player;
            designateNandPlayer(n_, player_);

            //Clear the chips to continue the operation for this player
            chips_.clear();
            designateChips();
        }

        //Get the centrod of all the chips it is a collective and statistical representation
        //of all the coins of a particular color this can be used to evaluate how good a position is

        pair<float, float> queryCentroidForChips(set<char> chips){
            int count = 0;
            float rval = 0.;
            float cval = 0.;
            for (int i = 0; i < n_; i++){
                for (int j = 0; j < n_; j++){
                    char c = state_[i][j];
                    if (chips.find(c) != chips.end()){
                        rval += i;
                        cval += j;
                        count++;
                    }
                }
            }

            pair<float, float> centroid;
            if (count != 0){
                centroid = {rval / count, cval / count};
            }

            return centroid;
        }

        friend class stateCalculator;
    };

    //This Class is used to calculate how good/favourable a state
    //is for the current player

    class stateCalculator : State{
        private:
            bool debug_ = false;
            size_t myPromotedCount_ = 0;
            size_t myPawnsCount_ = 0;
            size_t opponentPawnsCount_ = 0;
            size_t opponentPromotedCount_ = 0;


            //Count the diff types of chips
            void prepareScore(){
                for (auto row : state_){
                    for (auto c : row){
                        if (c != '_'){
                            if (myChips_.find(c) != myChips_.end()){
                                if (c == myPromotedPawn_){
                                    myPromotedCount_++;
                                } else {
                                    myPawnsCount_++;
                                }
                            } else {
                                if (c == opponentPromotedPawn_) {
                                    opponentPromotedCount_++;
                                } else {
                                    opponentPawnsCount_++;
                                }
                            }
                        }
                    }
                }
            }

        public:
            stateCalculator(State b) : State(b){
                prepareScore();
            }

            float score(){
                //This is the score given to each state to calculate the favourability of the position
                //for the current player, higher coefficient given to king as it is a more powerful chip
                // and negative score for each chip of the opponent as it is unfavourable for us
                //Here Assigning 10x priority to the promoted pawns
                float score = (10. * myPromotedCount_ + myPawnsCount_) -
                            (10. * opponentPromotedCount_ + opponentPawnsCount_);

                // however, a win or loss should be exponentiated in value.
                if (myPromotedCount_ + myPawnsCount_ == 0){
                    score = -100.;
                }

                if (opponentPromotedCount_ + opponentPawnsCount_ == 0){
                    score = 100.;
                }

                auto c1 = queryCentroidForChips(myChips_);
                auto c2 = queryCentroidForChips(opponentChips_);

                float dist = sqrt(pow(c1.first - c2.first, 2) + pow(c1.second - c2.second, 2));
                float extraScore = (float(n_) - dist) / float(n_) / float(n_);

                if (debug_){
                    printf("based on player: %c\n", player_);
                    printf("returning score of %f based on (%zu, %zu) vs (%zu, %zu)\n", score,
                        myPromotedCount_, myPawnsCount_,
                        opponentPromotedCount_, opponentPawnsCount_);
                }

                return score + extraScore;
            }

            bool gameOver(){
                return opponentPromotedCount_ + opponentPawnsCount_ == 0 or
                    myPromotedCount_ + myPawnsCount_ == 0;
            }
};



#endif
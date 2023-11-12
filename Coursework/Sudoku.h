#ifndef SUDOKU_H
#define SUDOKU_H

#include "Searchable.h"

// TODO: Your Sudoku class goes here:
#include <vector>
using std::vector;
#include <tuple>
using std::tuple;

#include <math.h>

#include <iostream>
using std::cout;
using std::endl;

using std::pair;
using std::get;

#include "SudokuSquare.h"

class Sudoku : public Searchable {
    private:
        vector<vector<SudokuSquareSet>> options;

        vector<vector<vector<int>>> identicalH;  //[number of items][row][existing values]
        vector<vector<vector<int>>> identicalV;
        vector<vector<vector<pair<int, int>>>> identicalB;  //[number of items][box number][existing values]

        int boardSize;
        int boxSize;
        int unfilled;

    public:
        Sudoku(const int & sizeIn) 
            :boardSize(sizeIn), boxSize(sqrt(sizeIn)), unfilled(sizeIn * sizeIn){

            // fill possible values in set
            SudokuSquareSet values;

            for (int k = 1; k <= boardSize; ++k) {
                values.insert(k);
            }

            // fill options
            vector<SudokuSquareSet> v1;
            for (int j = boardSize; j >= 1; --j) {
                v1.push_back(values);
            }

            for (int i = boardSize; i >= 1; --i) {
                options.push_back(v1);
            }

            // fill identical array
            vector<vector<int>> v2;;
            vector<vector<pair<int, int>>> v3;

            for (int j = boardSize; j > 0; --j) {
                v2.push_back(vector<int>{});
                v3.push_back(vector<pair<int, int>>{});
            }

            for (int i = boxSize; i > 0; --i) {
                identicalH.push_back(v2);
                identicalV.push_back(v2);
                identicalB.push_back(v3);
            }

        }

        Sudoku (const Sudoku & other){
            *this = other;
        }

        int getSquare(const int & row, const int & col) const {
            if (options[row][col].size() == 1) {
                return *options[row][col].begin();
            }
            else {
                return -1;
            }
        }

        bool setSquare(const int & row, const int & col, const int & value) {
            int prevSize = options[row][col].size();

            if (getSquare(row, col) == -1) {
                options[row][col].clear();
                options[row][col].insert(value);
                unfilled -= 1;

                removeIdentical(row, col, prevSize);   // remove from vector of identicals
            }

            SudokuSquareSet newSet;
            newSet.insert(value);

            if (!clearRow(row, newSet, vector<int>{col})) {
                return false;
            }
            if (!clearCol(col, newSet, vector<int>{row})) {
                return false;
            }
            if (!clearBox(getBox(row, col), newSet, vector<pair<int, int>>{pair<int, int>(row, col)})) {
                return false;
            }
            
            return true;
        }

        bool updateSquare(const int & row, const int & col, const int & prevSize) {
            int newSize = options[row][col].size();
            removeIdentical(row, col, prevSize);

            if (newSize == 0) {
                return false;
            }
            else if (newSize == 1) {
                unfilled -= 1;
                return setSquare(row, col, getSquare(row, col));
            }
            else if (newSize - 2 < identicalH.size()) {
                if (!insertIdenticalH(row, col)) {
                    return false;
                }
                if (!insertIdenticalV(row, col)) {
                    return false;
                }
                if (!insertIdenticalB(row, col)) {
                    return false;
                }
            }
            
            return true;
        }

        bool isSolution() const override {
            if (unfilled == 0) {
                return true;
            }

            return false;
        }

        void write(ostream & o) const override {
            for (int i = 0; i < boardSize; ++i) {
                for (int j = 0; j < boardSize; ++j) {
                    int value = getSquare(i, j);

                    if (value != -1) {
                        
                        cout << value << " ";
                        if (value < 10) {
                            cout<< " ";
                        }
                    }
                    else {
                        cout << "__ ";
                    }
                }
                cout << endl;
            }
        }
        
        vector<unique_ptr<Searchable>> successors() const override {
            vector<unique_ptr<Searchable>> ptrSuccessors;
            bool found = false;

            for (int i = 0; i < identicalH.size() && found == false; ++i) {
                if (minSuccessors(ptrSuccessors, identicalH[i])) {
                    found = true;
                }
            }
            if (found == false) {
                for (int i = 0; i < boardSize and found == false; ++i) {    // if non exist, find upper-most left-most square
                    for (int j = 0; j < boardSize and found == false; ++j) {
                        if (getSquare(i, j) == -1) {
                            found = true;

                            // Find acceptable values
                            for (int value: options[i][j]) {
                                Sudoku* copySudoku = new Sudoku(*this);
                                unique_ptr<Searchable> ptr = unique_ptr<Searchable>(copySudoku);

                                if (copySudoku->setSquare(i, j, value)) {
                                    ptrSuccessors.push_back(std::move(ptr));
                                }
                                else {
                                    ptr.reset();
                                }
                            }
                        }
                    }
                }
            }
            
            if (ptrSuccessors.size() == 1) {
                if (ptrSuccessors.front()->isSolution()) {
                    return ptrSuccessors;
                }
                else {
                    return ptrSuccessors.front()->successors();
                }
            }
            
            return ptrSuccessors;
        }

        bool minSuccessors(vector<unique_ptr<Searchable>> & ptrSuccessors, const vector<vector<int>> & myVector) const {
            for (int i = myVector.size() - 1; i >= 0; --i) {
                if (myVector[i].size() > 0) {
                    int col = myVector[i].front();

                    for (int value : options[i][col]) {
                        Sudoku* copySudoku = new Sudoku(*this);
                        unique_ptr<Searchable> ptr = unique_ptr<Searchable>(copySudoku);

                        if (copySudoku->setSquare(i, col, value)) {
                            ptrSuccessors.push_back(std::move(ptr));
                        }
                        else {
                            ptr.reset();
                        }
                    }

                    return true;
                }
            }

            return false;
        }

        int heuristicValue() const override{
            return unfilled;
        }

        int getBox(const int & row, const int & col) const {
            int boxH = floor(row/boxSize);
            int boxV = floor(col/boxSize) * boxSize;

            return boxH + boxV;
        }

        bool clearRow(const int & row, const SudokuSquareSet & values, const vector<int> & squareExc) {
            for (int i = boardSize - 1; i >= 0; --i) {
                bool found = false;
                for (int x: squareExc) {
                    if (i == x) {
                        found = true;
                        break;
                    }
                }
                if (found == false) {
                    int prevSize = options[row][i].size();

                    if (options[row][i].erase(values)) {
                        if (!updateSquare(row, i, prevSize)) {
                            return false;
                        }
                    }
                }
                
            }

            return true;
        }

        bool clearCol(const int & col, const SudokuSquareSet & values, const vector<int> & squareExc) {
            for (int i = boardSize - 1; i >= 0; --i) {
                bool found = false;
                for (int x: squareExc) {
                    if (i == x) {
                        found = true;
                        break;
                    }
                }
                if (found == false) {
                    int prevSize = options[i][col].size();

                    if (options[i][col].erase(values)) {
                        if (!updateSquare(i, col, prevSize)) {
                            return false;
                        }
                    }
                }
            }

            return true;
        }

        bool clearBox(const int & boxIndex, const SudokuSquareSet & values, const vector<pair<int,int>> & squareExc) {
            int x = (boxIndex % boxSize);
            int y = (boxIndex / boxSize);
            x *= boxSize;
            y *= boxSize;

            for (int i = boxSize - 1; i >= 0; --i) {
                for (int j = boxSize - 1; j >= 0; --j) {
                    int newX = x + i;
                    int newY = y + j;

                    bool found = false;
                    for (pair<int, int> x: squareExc) {
                        if (newX == get<0>(x) && newY ==get<1>(x)) {
                            found = true;
                            break;
                        }
                    }
                    if (found == false) {
                        int prevSize = options[newX][newY].size();

                        if (options[newX][newY].erase(values)) {
                            if (!updateSquare(newX, newY, prevSize)) {
                                return false;
                            }
                        }
                    }
                }
            }
            return true;
        }

        bool insertIdenticalH(const int & row, const int & col) {
            int elements = options[row][col].size();
            if (elements - 2 < 0) {
                return true;
            }

            identicalH[elements - 2][row].push_back(col);

            vector<int> myVectorH;
            myVectorH.push_back(col);

            auto itr = identicalH[elements - 2][row].begin();
            int count = 1;
            for (; itr != identicalH[elements - 2][row].end(); ++itr) {
                if (*itr != col && options[row][*itr] == options[row][col]) {
                    myVectorH.push_back(*itr);
                    count += 1;

                    if (count == elements) {
                        return clearRow(row, options[row][col], myVectorH);
                    }
                }
            }
            
            return true;
        }

        bool insertIdenticalV(const int & row, const int & col) {
            int elements = options[row][col].size();
            if (elements - 2 < 0) {
                return true;
            }

            identicalV[elements - 2][col].push_back(row);

            vector<int> myVector;
            myVector.push_back(row);

            auto itr = identicalV[elements - 2][col].begin();
            int count = 1;

            for (; itr != identicalV[elements - 2][col].end(); ++itr) {
                if (*itr != row && options[*itr][col] == options[row][col]) {
                    myVector.push_back(*itr);
                    count += 1;

                    if (count == elements) {
                        return clearCol(col, options[row][col], myVector);
                    }
                }
            }

            return true;
        }

        bool insertIdenticalB(const int & row, const int & col) {
            int elements = options[row][col].size();
            if (elements - 2 < 0) {
                return true;
            }

            int boxNumber = getBox(row, col);

            identicalB[elements - 2][boxNumber].push_back(pair<int, int>(row, col));
            
            vector<pair<int, int>> myVector;
            myVector.push_back(pair<int, int>(row, col));

            auto itr = identicalB[elements - 2][boxNumber].begin();
            int count = 1;

            for (; itr != identicalB[elements - 2][boxNumber].end(); ++itr) {
                int x = get<0>(*itr);
                int y = get<1>(*itr);

                if (x != row && y != col && options[x][y] == options[row][col]) {
                    myVector.push_back(*itr);
                    count += 1;

                    if (count == elements) {
                        return clearBox(boxNumber, options[row][col], myVector);
                    }
                }
            }

            return true;
        }
        
        void removeIdenticalH(const int & row, const int & col, const int & prevSize) {
            auto itr = identicalH[prevSize - 2][row].begin();

            for (; itr != identicalH[prevSize - 2][row].end(); ++itr) {
                if (*itr == col) {
                    identicalH[prevSize - 2][row].erase(itr);
                    break;
                }
            }
        }

        void removeIdenticalV(const int & row, const int & col, const int & prevSize) {
            auto itr = identicalV[prevSize - 2][col].begin();

            for (; itr != identicalV[prevSize - 2][col].end(); ++itr) {
                if (*itr == row) {
                    identicalV[prevSize - 2][col].erase(itr);
                    break;
                }
            }
        }

        void removeIdenticalB(const int & row, const int & col, const int & prevSize) {
            int boxNumber = getBox(row, col);

            auto itr = identicalB[prevSize - 2][boxNumber].begin();

            for (; itr != identicalB[prevSize - 2][boxNumber].end(); ++itr) {
                if (get<0>(*itr) == row && get<1>(*itr) == col) {
                    identicalB[prevSize - 2][boxNumber].erase(itr);
                    break;
                }
            }
        }

        void removeIdentical(const int & row, const int & col, const int & prevSize) {
            if (prevSize >= 2 && prevSize - 2 < identicalH.size()) {
                removeIdenticalH(row, col, prevSize);
                removeIdenticalV(row, col, prevSize);
                removeIdenticalB(row, col, prevSize);
            }
        }
};

#endif

#ifndef BEST_FIRST_SEARCH_H
#define BEST_FIRST_SEARCH_H

#include "Searchable.h"

#include <memory>
using std::unique_ptr;

#include <deque>
using std::deque;

class BestFirstSearch {

protected:

    /// Make sure you increment this every time you 'expand' a node, by getting it successors and putting them on the queue        
    int nodes = 0;
    deque<unique_ptr<Searchable> > Q;
    
public:

    BestFirstSearch(std::unique_ptr<Searchable> && startFrom) {
        // TODO Put startFrom onto the queue:
        Q.push_back(std::move(startFrom));
    }
    
    int getNodesExpanded() const {
        return nodes;
    }
    
    Searchable * solve() {
            
        // TODO Implement search, returning a pointer to the solution (if one is found)
        while (!Q.empty()) {
            if (Q.front()->isSolution()) {
                return Q.front().get();
            }
                  
            ++nodes;
            
            unique_ptr<Searchable> current(std::move(Q.front()));
            Q.pop_front();
            
            vector<unique_ptr<Searchable>> successors = current->successors();

            myMergeSort(successors);
            for (auto & successor: successors) {
                Q.push_front(std::move(successor));
            }
        }
        
        return nullptr;
    }
    
    void myMergeSort(vector<unique_ptr<Searchable>> & myVector) {
        if (myVector.size() <= 1) { return;}

        int mid = myVector.size() / 2;
        vector<unique_ptr<Searchable>> vectorA;
        vector<unique_ptr<Searchable>> vectorB;

        for (int i = 0; i < myVector.size(); ++i) {
            if (i < mid) {
                vectorA.push_back(std::move(myVector[i]));
            }
            else {
                vectorB.push_back(std::move(myVector[i]));
            }
        }

        myMergeSort(vectorA);
        myMergeSort(vectorB);

        myVector.clear();
        auto itrA = vectorA.begin();
        auto itrB = vectorB.begin();
        for (; itrA != vectorA.end() || itrB != vectorB.end();) {
            if (itrA == vectorA.end()) {
                myVector.push_back(std::move(*itrB));
                ++itrB;
            }
            else if (itrB == vectorB.end()) {
                myVector.push_back(std::move(*itrA));
                ++itrA;
            }
            else {
                if (itrA->get()->heuristicValue() >= itrB->get()->heuristicValue()) {
                    myVector.push_back(std::move(*itrA));
                    ++itrA;
                }
                else {
                    myVector.push_back(std::move(*itrB));
                    ++itrB;
                }
            } 
        }
    }
};

// Do not edit below this line

#endif

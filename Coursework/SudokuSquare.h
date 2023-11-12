#ifndef SUDOKUSQUARE_H
#define SUDOKUSQUARE_H

// Do not add any #include statements to this file

class SquareSetItr {
    private:
        unsigned int value;
        unsigned int current;
    
    public:
        SquareSetItr(const unsigned int & valueIn)
            : value(valueIn){     
            
            if (value == 0) {
                current = 0;
            }
            else if ((value & 1) == 1) {
                current = 1;
            }
            else {
                current = 1;
                operator++();
            }
        }

        SquareSetItr(const unsigned int & valueIn, const int & count)
            : value(valueIn), current(count){     
        }

        unsigned int & operator*() {
            return current;
        }

        bool operator==(const SquareSetItr & other) const {
            return (current == other.current);
        }
        
        bool operator!=(const SquareSetItr & other) const {
            return !(current == other.current);
        }

        void operator++() {
            while (value != 0) {
                current += 1;
                value = value >> 1;
                
                if ((value & 1) == 1) {
                    return;
                }
            }
            current = 0;
        }
};

class SudokuSquareSet {

    // TODO: write your code here
    private:
        unsigned int values;
        int numberOfValues;

    public:
        SudokuSquareSet() {
            values = 0;
            numberOfValues = 0;
        }

        int size() const {
            return numberOfValues;
        }

        bool empty() const {
            if (size() == 0) {
                return true;
            }
            else {
                return false;
            }
        }

        void clear() {
            values = 0;
            numberOfValues = 0;
        }

        bool operator==(const SudokuSquareSet & other) const {
            return (values == other.values);
        }

        bool operator!=(const SudokuSquareSet & other) const {
            return (values != other.values);
        }

        SquareSetItr begin() const {
            return SquareSetItr(values);
        }

        SquareSetItr end() const {
            return 0;
        }

        SquareSetItr insert(const unsigned int & value) {
            unsigned int temp = (1 << (value - 1));

            if ((values & temp) == 0) {
                values += temp;
                numberOfValues += 1;
            }
            
            return find(value);
        }

        SquareSetItr find(const unsigned int & value) const {
            SquareSetItr itr = SquareSetItr(values);
            unsigned int temp = (value >> (value - 1));
            if ((temp & 1) == 1) {
                return SquareSetItr(temp, value);
            }
            
            return 0;
        }

        // return true if a value has been removed
        bool erase(const unsigned int & value) {
            unsigned int temp = 1 << (value - 1);

            if ((values & temp)  != 0) {
                values -= temp;
                numberOfValues -= 1;
                return true;
            }

            return false;
        }

        bool erase(const SudokuSquareSet & value) {
            bool change = false;

            for (unsigned int x : value) {
                if (erase(x) == true) {
                    change = true;
                }
            }

            return change;
        }

        bool erase(SquareSetItr & itr) {
            return erase(*itr);
        }
};

// Do not write any code below this line
static_assert(sizeof(SudokuSquareSet) == sizeof(unsigned int) + sizeof(int), "The SudokuSquareSet class needs to have exactly two 'int' member variables, and no others");


#endif

#ifndef CACHE_HPP
#define CACHE_HPP

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ostream>
#include <vector>

using namespace std;

class cache{
    public:
        cache();
        cache(string, string);
        void read_input(string);
        void direct_mapped();
        void set_associative();
        void fully_associative();
        //void 
    private:
        string input;
        string output;
        vector <char> operations;
        vector <long> targets;
};

#endif
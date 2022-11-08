#ifndef TRACE_READ_HPP
#define TRACE_READ_HPP

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <ostream>
#include <cmath>

using namespace std;

typedef pair<long, unsigned long> result;

class trace_read{
    public:
        trace_read();
        trace_read(string, string);
        void read_input(string);
        result always_taken();
        result always_n_taken();
        result bimodal_one_bit(int);
        result bimodal_two_bit(int);
        result gshare(int);
        result tournament();
        result btb();
        bool check_correct(int, string, int*, int);
        void print();
        void update_GHR(int, string);
        int get_GHR();
        void run_trace(string);
    private:
        long GHR;
        string input_file;
        string output_file;
        vector<long> addresses;
        vector<long> targets;
        vector<string> predictions;
};

#endif
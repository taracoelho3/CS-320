#include "cache.hpp"

using namespace std;

cache::cache(){
    //empty constructor
}

cache::cache(string input, string output){
    //string constructor 
    this->input = input;
    this->output = output;
    read_input(this->input);
}

void cache::read_input(string input){
    cout << "reading input from " << this->input << endl;
    long target;
    char operation;
    string line;

    ifstream infile(input);

    while(getline(infile, line)){
        stringstream s(line);
        s >> operation >> std::hex >> target;

        operations.push_back(operation);
        targets.push_back(target);
    }
}


int main(int argc, char *argv[]){
    if(argc != 3){
        cout << "invalid input" << endl;
    }
    else{
        string input, output;
        input = argv[1];
        output = argv[2];
        cache test(input, output);
    }
}
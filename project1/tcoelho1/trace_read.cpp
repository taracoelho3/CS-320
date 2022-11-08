#include "trace_read.hpp"

using namespace std;


trace_read::trace_read(){
    //empty val constructor
    this->GHR = 0;
}

trace_read::trace_read(string input, string output){
    this->GHR = 0;
    this->input_file = input;
    this->output_file = output;
    read_input(input_file);
}

void trace_read::read_input(string filename){
    unsigned long long addr;
    string behavior, line;
    unsigned long long target;

    ifstream infile(filename);

    while(getline(infile, line)) {

        stringstream s(line);
        s >> std::hex >> addr >> behavior >> std::hex >> target;

        addresses.push_back(addr);
        predictions.push_back(behavior);
        targets.push_back(target);
 
    }
}

result trace_read::always_taken(){
    result ret;

    long correct = 0;

    for(int i = 0; i < predictions.size(); i++){
        if(predictions.at(i).compare("T") == 0){
            correct++;
        }
    }
    ret.first = correct;
    ret.second = predictions.size();
    return ret;
}

result trace_read::always_n_taken(){
    result ret;

    long correct = 0;

    for(int i = 0; i < predictions.size(); i++){
        if(predictions.at(i).compare("NT") == 0){
            correct++;
        }
    }
    ret.first = correct;
    ret.second = predictions.size();
    return ret;
}

result trace_read::bimodal_one_bit(int table_size){
    long correct = 0;

    int table[table_size];
    //flood table with strongly predicted
    for(int i = 0; i < table_size; i++){
        table[i] = 1;
    }

    for(int j = 0; j < addresses.size(); j++){
        int index = addresses.at(j) % table_size;
        int table_prediction = table[index];
        string real_prediction = predictions.at(j);

        if(table_prediction == 1){
            if(real_prediction.compare("T") == 0){
                correct++;
            }
            else{
                table[index] = 0;
            }
        }
        else{
            if(real_prediction.compare("NT") == 0){
                correct++;
            }
            else{
                table[index] = 1;
            }
        }
    }
    
    result ret;
    ret.first = correct;
    ret.second = predictions.size();
    return ret;
}

result trace_read::bimodal_two_bit(int table_size){
    long correct = 0;

    int table[table_size];
    //flood table with strongly predicted
    for(int i = 0; i < table_size; i++){
        table[i] = 0b11;
    }

    for(int j = 0; j < addresses.size(); j++){
        long table_index = addresses.at(j) % table_size;
        int table_prediction = table[table_index];
        string real_prediction = predictions.at(j);

        switch(table_prediction){
            case 0:
                if(real_prediction.compare("NT") == 0){
                    correct++;
                }
                else{
                    table[table_index] +=1;
                }
                break;
            case 1:
                if(real_prediction.compare("NT") == 0){
                    correct++;
                    table[table_index] -=1;
                }
                else{
                    table[table_index] +=1;
                }
                break;
            case 2:
                if(real_prediction.compare("T") == 0){
                    correct++;
                    table[table_index] +=1;
                }
                else{
                    table[table_index] -=1;
                }
                break;
            case 3:
                if(real_prediction.compare("T") == 0){
                    correct++;
                }
                else{
                    table[table_index] -=1;
                }
                break;
            default:
                cout << "issue in prediction value" << endl;
        }
    }

    result ret;
    ret.first = correct;
    ret.second = predictions.size();
    return ret;
}

result trace_read::gshare(int history){
    long correct = 0;
    this->GHR = 0;

    long table[2048];
    //flood table
    for(long i = 0; i < 2048; i++){
        table[i] = 0b11;
    }

    for(int j = 0; j < addresses.size(); j++){
        long table_index = addresses.at(j) % 2048;
        long real_index = this->GHR ^ table_index;
        int table_prediction = table[real_index];
        string real_prediction = predictions.at(j);

        switch(table_prediction){
            case 0:
                if(real_prediction.compare("NT") == 0){
                    correct++;
                }
                else{
                    table[real_index] +=1;
                }
                break;
            case 1:
                if(real_prediction.compare("NT") == 0){
                    correct++;
                    table[real_index] -=1;
                }
                else{
                    table[real_index] +=1;
                }
                break;
            case 2:
                if(real_prediction.compare("T") == 0){
                    correct++;
                    table[real_index] +=1;
                }
                else{
                    table[real_index] -=1;
                }
                break;
            case 3:
                if(real_prediction.compare("T") == 0){
                    correct++;
                }
                else{
                    table[real_index] -=1;
                }
                break;
            default:
                cout << "issue in prediction value" << endl;
        }
        update_GHR(history, real_prediction);
    }

    result ret;
    ret.first = correct;
    ret.second = predictions.size();
    return ret;
}

result trace_read::tournament(){
    long correct = 0;
    this->GHR = 0;
    int table_size = 2048;
    int history = 11;

    int bimodal_table[table_size];
    int gshare_table[table_size];
    int selector_table[table_size];

    for(long i = 0; i < 2048; i++){
        bimodal_table[i] = 0b11;
        gshare_table[i] = 0b11;
        selector_table[i] = 0;
    }

    for(int j = 0; j < addresses.size(); j++){
        int bimodal_index = addresses.at(j) % table_size;
        int gshare_index = this->GHR ^ bimodal_index;
        int selector_index = bimodal_index;
        int bimodal_prediction = bimodal_table[bimodal_index];
        int gshare_prediction = gshare_table[gshare_index];
        string real_prediction = predictions.at(j);
        
        int selector_prediction = selector_table[selector_index];

        bool gshare_correct = check_correct(gshare_table[gshare_index], real_prediction, gshare_table, gshare_index);
        bool bimodal_correct = check_correct(bimodal_table[bimodal_index], real_prediction, bimodal_table, bimodal_index);

        switch(selector_prediction){
            case 0:
                //strongly favors gshare
                if(gshare_correct){
                    correct++;
                }
                else{
                    if(bimodal_correct == true){
                        selector_table[selector_index] +=1;
                    }
                }
                break;
            case 1:
                //weakly favors gshare
                if(gshare_correct){
                    if(selector_prediction == 1 && bimodal_correct == false){
                        selector_table[selector_index] -=1;
                    }
                    correct++;
                }
                else{
                    if(bimodal_correct == true){
                        selector_table[selector_index] +=1;
                    }
                }
                break;
            case 2:
                //weakly favors bimodal
                if(bimodal_correct){
                    if(selector_prediction == 2 && gshare_correct == false){
                        selector_table[selector_index] +=1;
                    }
                    correct++;
                }
                else{
                    if(gshare_correct == true){
                        selector_table[selector_index] -=1;
                    }
                }
                break;
            case 3:
                //strongly favors bimodal
                if(bimodal_correct){
                    correct++;
                }
                else{
                    if(gshare_correct == true){
                        selector_table[selector_index] -=1;
                    }
                }
                break;
            default:
                cout << "issue in prediction value" << endl;
        }
        update_GHR(history, real_prediction);
    }

    result ret;
    ret.first = correct;
    ret.second = predictions.size();
    return ret;
}

result trace_read::btb(){
    int correct = 0;
    int attempted_predictions = 0;
    int table_size = 512;

    int buffer_table[table_size];
    int bimodal_table[table_size];

    for(int i = 0; i < table_size; i++){
        bimodal_table[i] = 0b11;
        buffer_table[i] = 0b11;
    }

    for(int j = 0; j < addresses.size(); j++){
        long table_index = addresses.at(j) % table_size;
        int table_prediction = bimodal_table[table_index];
        string real_prediction = predictions.at(j);

        if(table_prediction == 2 || table_prediction == 3){
            attempted_predictions++;
            if(targets.at(table_index) == buffer_table[table_index]){
                correct++;
            }
            else{
                buffer_table[table_index] = targets.at(table_index);
            }
        }
 
        switch(table_prediction){
            case 0:
                if(real_prediction.compare("NT") == 0){
                }
                else{
                    bimodal_table[table_index] +=1;
                }
                break;
            case 1:
                if(real_prediction.compare("NT") == 0){
                    bimodal_table[table_index] -=1;
                }
                else{
                    bimodal_table[table_index] +=1;
                }
                break;
            case 2:
                
                if(real_prediction.compare("T") == 0){
                    bimodal_table[table_index] +=1;
                }
                else{
                    bimodal_table[table_index] -=1;
                }
                break;
            case 3:

                if(real_prediction.compare("T") == 0){
                    //correct
                }
                else{
                    bimodal_table[table_index] -=1;
                }
                break;
            default:
                cout << "issue in prediction value" << endl;
        }
    }
    result ret;
    ret.first = correct;
    ret.second = attempted_predictions;
    return ret;
}

bool trace_read::check_correct(int prediction, string real_prediction, int *table, int index){
    bool is_correct;
    switch(prediction){
            case 0:
                if(real_prediction.compare("NT") == 0){
                    is_correct = true;
                }
                else{
                    is_correct = false;
                    table[index] +=1;
                }
                break;
            case 1:
                if(real_prediction.compare("NT") == 0){
                    is_correct = true;
                    table[index] -=1;
                }
                else{
                    is_correct = false;
                    table[index] +=1;
                }
                break;
            case 2:
                if(real_prediction.compare("T") == 0){
                    is_correct = true;
                    table[index] +=1;
                }
                else{
                    is_correct = false;
                    table[index] -=1;
                }
                break;
            case 3:
                if(real_prediction.compare("T") == 0){
                    is_correct = true;
                }
                else{
                    is_correct = false;
                    table[index] -=1;
                }
                break;
            default:
                is_correct = false;
                cout << "issue in prediction value" << endl;
        }
        return is_correct;
}

void trace_read::print(){
    for(int i = 0;i < addresses.size(); i++){
        cout << addresses.at(i) << " " << predictions.at(i) << " " << targets.at(i) << endl;
    }
}

void trace_read::update_GHR(int history, string if_taken){
    if(if_taken.compare("T") == 0){
        //left shift and add 1
        this->GHR = (int)(this->GHR << 1);
        this->GHR = (int)(this->GHR | 1);
        this->GHR = (int)(this->GHR & (int)(pow(2, history) - 1));
    }
    else{
        //left shift and add 0
        this->GHR = (int)(GHR << 1);
        this->GHR = (int)(this->GHR & (int)(pow(2, history) - 1));
    }
}

int trace_read::get_GHR(){
    return this->GHR;
}

void trace_read::run_trace(string output){
    ofstream ofile(output);

    result ret = this->always_taken();
    ofile << ret.first << "," << ret.second << "; " << endl;

    result ret2 = this->always_n_taken();
    ofile << ret2.first << "," << ret2.second << "; " << endl;

    int sizes[7] = {16, 32, 128, 256, 512, 1024, 2048};
    vector<result> one_bit_results, two_bit_results;

    for(int i = 0; i < 7; i++){
        one_bit_results.push_back(this->bimodal_one_bit(sizes[i]));
        two_bit_results.push_back(this->bimodal_two_bit(sizes[i]));
    }

    for(int j = 0; j < one_bit_results.size(); j++){
        ofile << one_bit_results.at(j).first << "," << one_bit_results.at(j).second << "; ";
    }
    ofile << endl;
    
    for(int k = 0; k < two_bit_results.size(); k++){
        ofile << two_bit_results.at(k).first << "," << two_bit_results.at(k).second << "; ";
    }
    ofile << endl;

    vector<result> gshare_results;
    for(int l = 3; l <= 11; l++){
        gshare_results.push_back(this->gshare(l));
    }

    for(int m = 0; m < gshare_results.size(); m++){
        ofile << gshare_results.at(m).first << "," << gshare_results.at(m).second << "; ";
    }
    ofile << endl;

    result ret6 = this->tournament();
    ofile << ret6.first << "," << ret6.second << "; " << endl;

    result ret7 = this->btb();
    ofile << ret7.first << "," << ret7.second << "; " << endl;

}

int main(int argc, char *argv[]){
    if(argc != 3){
        cout << "invalid input" << endl;
    }
    else{
        string input, output;
        input = argv[1];
        output = argv[2];
        trace_read test(input, output);

        test.run_trace(output);

    }
}
/* g++ main.cpp -o main `pkg-config --cflags --libs opencv` && main*/
/* cmake ../ && make && ../main */
#include <opencv2/imgproc.hpp>

#include <iostream>
#include "InteligentAgent.h"
#include "model.h"



int main(){

    IntelligentAgent a = IntelligentAgent();
    for (int i=0; i<LEN; i++){
        for (int j=0; j<LEN; j++){
            //std::cout << "HI" << q_table[i][j] << "\n";
            std::cout << a.action(i+j) << ' ';
        }
    }


}
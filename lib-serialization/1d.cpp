#include <fstream>
#define OBS_RANGE 2

int _q_table[OBS_RANGE] = {1,2};

 
int main(){
    std::ofstream file;
    file.open("temp.txt");

    file << '{';
    for (int a=0; a<OBS_RANGE; a++){
        if (a>0) file << ", ";
        file << _q_table[a];
        
    }
    file << '}';
    file.close();
    return 0;
}

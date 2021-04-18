#include <fstream>
#define OBS_RANGE 2

int _q_table[OBS_RANGE][OBS_RANGE][OBS_RANGE] = {
    {{1,2},{1,2}},
    {{1,2},{1,2}}
};

 
int main(){
    std::ofstream file;
    file.open("temp.txt");

    file << '{';
    for (int a=0; a<OBS_RANGE; a++){
        if (a>0) file << ", ";
        file << '{';
        for (int b=0; b<OBS_RANGE; b++){
            if (b>0) file << ", ";
            file << '{';
            for (int c=0; c<OBS_RANGE; c++){
                if (c>0) file << ", ";
                file << _q_table[a][b][c];
            }
            file << '}';
        }
        file << '}';
    }
    file << '}';
    file.close();
    return 0;
}

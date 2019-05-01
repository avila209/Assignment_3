#include <iostream>
#include <fstream>
using namespace std;

int main() {
    ifstream file ("sample-jobs.dat");

    if(file.is_open()){
        cout << "File opened correctly." << endl;
    }
    else{
        cout << "Failed" << endl;
    }

    int ID[100];
    char Action[100];
    char Page[100];

    int i = 0;
    cout << "PID" << "\t" << "Act" << "\t" << "Page" << endl;

    while(!file.eof()){
        file >> ID[i] >> ws >> Action[i] >> ws;

        if(Action[i] == 'C' || Action[i] == 'T'){
            Page[i] = NULL;
        }
        else{
            file >> Page[i];
        }

        cout << ID[i] << "\t" << Action[i] << "\t" << Page[i] << endl;

        if(i == 99){
            break;
        }
        i++;
    }

    return 0;
}
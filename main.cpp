#include <iostream>
#include <fstream>

using namespace std;
int size = 0;

struct Process{
    int ID;
    bool Created;
    bool Terminated;
    int *Allocated = new int [size];
};


void FIFO(int *ID, char *Action, char *VPage, int Page[20], int size);

int main() {
    ifstream file("sample-jobs.dat");

    string line;
    while(!file.eof()){
        getline(file,line);
        size++;
    }
    file.close();

    file.open("sample-jobs.dat");
    size--;

    if(file.is_open()) cout << "File opened correctly." << endl;
    else cout << "Failed" << endl;

    int ID [size];
    char Action [size];
    char VPage [size];

    cout << "PID" << "\t" << "Act" << "\t" << "Page" << endl;

    int i = 0;
    while(!file.eof()){
        file >> ID[i] >> ws >> Action[i] >> ws;

        if(Action[i] == 'C' || Action[i] == 'T') VPage[i] = '\0';
        else file >> VPage[i];

        cout << ID[i] << "\t" << Action[i] << "\t" << VPage[i] << endl;
        i++;
    }

    cout << endl;

    int NumProcesses = 0;
    //Find all unique processes
    for (int i=0; i<size; i++){
        // Check if the picked element is already printed
        int j;
        for (j=0; j<i; j++)
            if (ID[i] == ID[j]){
                break;
            }
        // If not printed earlier, then print it
        if (i == j){
            NumProcesses++;
        }
    }

    Process Unique[NumProcesses];

    //Store unique processes
    for (int i=0; i<size; i++){
        // Check if the picked element is already printed
        int j;
        for (j=0; j<i; j++)
            if (ID[i] == ID[j]){
                break;
            }
        // If not printed earlier, then print it
        if (i == j){
            Unique->ID = ID[i];
        }
    }

    for(int i = 0; i < NumProcesses; i++){
        cout << Unique[i].ID << endl;
    }

    int Page[20];

    return 0;
}

void FIFO(int *ID, char *Action, char *VPage, int Page[20], int size){

    for(int i = 0; i < 20; i++){
        Page[i] = -1; //our integer null value equivalent
    }

    int FIFO = 0;
    for(int i = 0; i < size; i++){

    }

    cout << "PHYSICAL PAGES" << endl;
    for(int i = 0; i < 20; i++){
        if(Page[i] == -1){
            cout << i << "\t \t \t" << "FREE" << endl;
        }
        else{
            cout << i << "\t \t \t" << *ID << endl;
        }
        *ID++;
    }
}
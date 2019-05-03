#include <iostream>
#include <fstream>

using namespace std;
int size = 0;
int Page[20];

struct Process{
    int ID;

    /*
    bool Created;
    bool Terminated;
    int *Allocated = new int [size];
     */
    char Action;
    char Page;
};

void FIFO(Process Proc);

int main() {
    for(int i = 0; i < 20; i++){
        Page[i] = -1;
    }

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
    int Unique_Proc = 0;
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
            Unique[Unique_Proc].ID = ID[i];
            Unique_Proc++;
        }
    }

    for(int i = 0; i < Unique_Proc; i++){
        cout << Unique[i].ID << endl;
    }

    for(int i = 0; i < size; i++){
        int j = 0;
        while(ID[i] != Unique[j].ID){
            j++;
        }
        Unique[j].Action = Action[i];
        if(Action[i] == 'C' || Action[i] == 'T'){
            Unique[j].Page = NULL;
        }
        else{
            Unique[j].Page = VPage[i];
        }
        FIFO(Unique[j]);
    }

    cout << "PHYSICAL PAGE" << endl;
    for(int i = 0; i < 20; i++){
        cout << i << "\t \t \t";
        if(Page[i] == -1){
            cout << "FREE" << endl;
        }else{
            cout << Page[i] << endl;
        }
    }


    return 0;
}

void FIFO(Process Proc){
    for(int i = 0; i < 20; i++){
        if(Page[i] == -1 && Proc.Action == 'A' && Proc.Page == (i + '0')){
            Page[i] = Proc.ID;
        }
    }
}
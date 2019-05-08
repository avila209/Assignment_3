#include <iostream>
#include <fstream>

using namespace std;
int size = 0;
int Page[20];

struct Process{
    int ID = 0;
    bool Created = false;
    char Action = '\0';
    char Page = '\0';
};

struct Virtual_Page{
    int Pages[200];
};

struct Page_Table{
    int VPage[200];
    int PPage[20];
};

void FIFO(Process Proc);

int main() {

    for(int q = 0; q < 20; q++){
        Page[q] = -1;
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
    for (int z=0; z<size; z++){
        // Check if the picked element is already printed
        int j;
        for (j=0; j<z; j++)
            if (ID[z] == ID[j]){
                break;
            }
        // If not printed earlier, then print it
        if (z == j){
            NumProcesses++;
        }
    }

    Process Unique[NumProcesses];
    int Unique_Proc = 0;
    //Store unique processes
    for (int k=0; k<size; k++){
        // Check if the picked element is already printed
        int j;
        for (j=0; j<k; j++)
            if (ID[k] == ID[j]){
                break;
            }
        // If not printed earlier, then print it
        if (k == j){
            Unique[Unique_Proc].ID = ID[k];
            Unique_Proc++;
        }
    }

    for(i = 0; i < Unique_Proc; i++){
        cout << Unique[i].ID << endl;
    }



    cout << "\n" << "Number of unique processes: " << Unique_Proc << "\n" << endl;


    cout << "PHYSICAL PAGE" << endl;
    for(i = 0; i < 20; i++){
        cout << i << "\t \t \t";
        if(Page[i] == -1){
            cout << "FREE" << endl;
        }else{
            cout << Page[i] << endl;
        }
    }

    return 0;
}
#include <iostream>
#include <fstream>

using namespace std;
int size = 0;
int Page[20];

struct Page_Table{
    int VPage[200];
    int PPage[20];
};

struct Physical_Page{
    int ID = 0;
    bool Dirty;
};

struct Virtual_Page{
    int ID = 0;
    int Pages[200];
    Page_Table PT;
};


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

    int NumofUniqueProcesses = 0;
    for (int k=0; k<size; k++){
        // Check if the picked element is already printed
        int j;
        for (j=0; j<k; j++)
            if (ID[k] == ID[j]){
                break;
            }
        // If not printed earlier, then print it
        if (k == j){
            NumofUniqueProcesses++;
        }
    }

    int Unique_Processes[NumofUniqueProcesses];
    int count = 0;
    for (int k=0; k<size; k++){
        // Check if the picked element is already printed
        int j;
        for (j=0; j<k; j++)
            if (ID[k] == ID[j]){
                break;
            }
        // If not printed earlier, then print it
        if (k == j){
            Unique_Processes[count] = ID[k];
            count++;
        }
    }

    //Create a Virtual Page Table for each process
    Virtual_Page VirtualPage[NumofUniqueProcesses];
    cout << "List of unique process numbers:" << endl;

    for(i = 0; i < NumofUniqueProcesses; i++){
        VirtualPage->ID = Unique_Processes[i];
        cout << Unique_Processes[i] << endl;
    }

    cout << "\n" << "Number of unique processes: " << NumofUniqueProcesses << "\n" << endl;


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
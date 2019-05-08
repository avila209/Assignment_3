#include <iostream>
#include <fstream>

using namespace std;
int size = 0;

struct Page_Table{
    bool modified[200];
    int VPage[200];             //Virtual Page of process
    int PPage[20];              //Physical Page of process
};

struct Physical_Page{
    int ID = 0;                 //Current process in the page
    bool Dirty = false;         //Modified
    bool Protection = false;    //Read, Write, etc.
    int Accessed = 0;           //Number of times accessed
};

struct Virtual_Page{
    int ID = 0;                 //Unique process identification number
    int Pages[200];             //Large number of virtual pages per process
    Page_Table PT;              //Page translation table
};

struct Swap_Page{
    Virtual_Page Process;
};


int main() {
    Physical_Page PhysicalPage[20];

    for(int q = 0; q < 20; q++){
        PhysicalPage[q].ID = -1;
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
    cout << "List of unique process numbers: " << endl;

    for(i = 0; i < NumofUniqueProcesses; i++){
        VirtualPage[i].ID = Unique_Processes[i];
        for(int k = 0; k < 200; k++){
            VirtualPage[i].PT.modified[k] = false;
        }

        cout << VirtualPage[i].ID << endl;
    }

    cout << "\n" << "Number of unique processes: " << NumofUniqueProcesses << "\n" << endl;

    //*************** Load data into empty physical page table ****************************
    int pagenumber = 0, current_line = 0;
    for(i = 0; i < size; i++){
        if(pagenumber == 20) {
            current_line = size + 1;
            break;
        }
        if(Action[i] == 'A') {
            current_line = size + 1;
            PhysicalPage[pagenumber].ID = ID[i]; //Need to add checker if already filled

            //Search for virtual page with matching ID numbers.
            for(int q = 0; q < NumofUniqueProcesses; q++){
                if(VirtualPage[q].ID == ID[i]){
                    VirtualPage[q].PT.modified[VPage[i] - '0'] = true;
                    VirtualPage[q].PT.VPage[VPage[i] - '0'] = VPage[i] - '0';
                    VirtualPage[q].PT.PPage[VPage[i] - '0'] = pagenumber;
                    VirtualPage[q].Pages[VPage[i] - '0'] = VPage[i] - '0';

                    cout << "Storing Virtual page: " << VPage[i] << " into Process: " << VirtualPage[q].ID << endl;
                }
            }
            pagenumber++;
        }
    }
    //*************** Finished loading into the physical table ****************************

    cout << "Current line from the input file: " << current_line << "\n" << endl;





    cout << "PHYSICAL PAGE" << endl;
    for(i = 0; i < 20; i++){
        cout << i << "\t \t \t";
        if(PhysicalPage[i].ID == -1){
            cout << "FREE" << endl;
        }else{
            cout << PhysicalPage[i].ID << endl;
        }
    }

    cout << endl;
    cout << "VIRTUAL PAGE" << endl;
    for(i = 0; i < NumofUniqueProcesses; i++){
        cout << "Process: " << VirtualPage[i].ID << endl;
        for(int x = 0; x < 200; x++){
            if(VirtualPage[i].PT.modified[x] == true){
                cout << "\t" << "Virtual Page: " << VirtualPage[i].PT.VPage[x] << "\t Physical Page: " << VirtualPage[i].PT.PPage[x] << endl;
            }
        }
    }

    return 0;
}
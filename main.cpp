#include <iostream>
#include <fstream>

using namespace std;
int size = 0;

struct Page_Table{
    bool modified[200];         //Determining factor for print function
    int VPage[200];             //Virtual Page of process
    int PPage[20];              //Physical Page of process

    int *VPage2;
    int *PPage2;
};

struct Physical_Page{
    int ID = 0;                 //Current process in the page
    bool Dirty = false;         //Modified
    bool Read = false;          //Page has been read
    bool Write = false;         //Page has been written to
    int Accessed = 0;           //Number of times accessed
};

struct Virtual_Page{
    int ID = 0;                 //Unique process identification number
    Page_Table PT;              //Page translation table
    bool Created = false;
    bool Killed = false;
    bool Terminated = false;
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
    for(i = 0; i <= size; i++){
        //CHECK IF PAGE TABLE NOT FULL
        for(int u = 0; u < 20; u++){
            if(PhysicalPage[u].ID == -1){
                pagenumber = u;
                break;
            }
            if(u == 19){
                cout << "Page Table is now full." << endl;
                break;
            }
        }

        if(pagenumber == 20) {
            current_line = size + 1;
            break;
        }



        //ALLOCATE
        if(Action[i] == 'A') {
            current_line = size + 1;
            bool created = false;
            //Search for virtual page with matching ID numbers.
            for(int q = 0; q < NumofUniqueProcesses; q++){
                if(VirtualPage[q].ID == ID[i] && VirtualPage[q].Created){
                    VirtualPage[q].PT.modified[VPage[i] - '0'] = true;
                    VirtualPage[q].PT.VPage[VPage[i] - '0'] = VPage[i] - '0';
                    VirtualPage[q].PT.PPage[VPage[i] - '0'] = pagenumber;

                    //New additions -- Pointers
                    VirtualPage[q].PT.VPage2 = new int[200];
                    VirtualPage[q].PT.PPage2 = new int[20];

                    *(VirtualPage[q].PT.VPage2+VPage[i] - '0') = VPage[i] - '0';
                    *(VirtualPage[q].PT.PPage2+VPage[i] - '0') = pagenumber;

                    cout << "Storing Virtual page: " << VPage[i] << " into Process: " << VirtualPage[q].ID << endl;
                    created = true;
                }
            }
            if(created){
                PhysicalPage[pagenumber].ID = ID[i]; //Need to add checker if already filled
                PhysicalPage[pagenumber].Dirty = true;
                pagenumber++;
            }
        }

        //CREATE
        if(Action[i] == 'C'){
            //Search for virtual page with matching ID numbers.
            for(int q = 0; q < NumofUniqueProcesses; q++){
                if(VirtualPage[q].ID == ID[i]){
                    VirtualPage[q].Created = true;
                    VirtualPage[q].Terminated = false;
                    cout << "Creating Virtual Process: " << ID[i] << endl;
                }
            }
        }

        //TERMINATE
        if(Action[i] == 'T'){
            //Search for virtual page with matching ID numbers.
            for(int q = 0; q < NumofUniqueProcesses; q++){

                //Delete from page table where ID matches
                for(int t = 0; t < 20; t++){
                    if(PhysicalPage[t].ID == VirtualPage[q].ID){
                        PhysicalPage[t].ID = -1; //free flag

                        cout << "Removing " << VirtualPage[q].ID << " from " << t << endl;
                    }
                }


                if(VirtualPage[q].ID == ID[i] && VirtualPage[q].Created){
                    VirtualPage[q].Terminated = true;


                    delete [] VirtualPage[q].PT.VPage2;
                    delete [] VirtualPage[q].PT.PPage2;

                    //Clear the modified flags
                    for(int r = 0; r < 200; r++){
                        VirtualPage[q].PT.modified[r] = false;
                    }

                    cout << "Terminating Virtual Process: " << ID[i] << endl;
                }
            }
        }

        //READ
        if(Action[i] == 'R'){
            //Search for virtual page with matching ID numbers.
            for(int q = 0; q < NumofUniqueProcesses; q++){
                if(VirtualPage[q].ID == ID[i] && VirtualPage[q].Created){
                    //Read from page 1 of virtual process
                    //Need to utilize the PT to find virtual to physical

                    cout << "Reading from Virtual Page: " << VPage[i] << endl;
                }
            }
        }



    }
    //*************** Finished loading into the physical table ****************************

    cout << "\n" << "Current line from the input file: " << current_line << "\n" << endl;





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
            if(VirtualPage[i].PT.modified[x]){
                cout << "\t" << "Virtual Page: " << *(VirtualPage[i].PT.VPage+x) << "\t Physical Page: " << *(VirtualPage[i].PT.PPage+x) << endl;
            }
        }
    }

    return 0;
}
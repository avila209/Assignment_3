#include <iostream>
#include <fstream>

using namespace std;
int size = 0;

struct Page_Table{
    bool modified[200];         //Determining factor for print function
    bool present[200];
    int *VPage2;                //Virtual Page of process
    int *PPage2;                //Physical Page of process
};

struct Physical_Page{
    int ID = 0;                 //Current process in the page
    bool Dirty = false;         //Modified
    bool Read = false;          //Page has been read
    bool Write = false;         //Page has been written to
    int Accessed = 0;           //Number of times accessed - LRU

    int Order = 0;              //Arrival time - FIFO
};

struct Virtual_Page{
    int ID = 0;                 //Unique process identification number
    Page_Table PT;              //Page translation table
    bool Created = false;
    bool Killed = false;
    bool Terminated = false;
    bool Allocated = false;
};

struct Swap_Page{
    Virtual_Page Process;
};

void FIFO(Virtual_Page *VirtualPage, Physical_Page *PhysicalPage, int current_line, int NumofUniqueProcesses);
void ALLOCATE(Virtual_Page *VirtualPage, Physical_Page *PhysicalPage, int current_line, int NumofUniqueProcesses, int pagenumber, int ID[], int VPage[], int i);
void READ(Virtual_Page *VirtualPage, Physical_Page *PhysicalPage, int NumofUniqueProcesses, int ID[], int VPage[], int i);


void CREATE(Virtual_Page *VirtualPage, Physical_Page *PhysicalPage, int NumofUniqueProcesses, int ID[], int i);
void WRITE(Virtual_Page *VirtualPage, Physical_Page *PhysicalPage, int NumofUniqueProcesses, int ID[], int VPage[], int i);

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

    if(file.is_open()) cout << "File opened correctly." << endl;
    else cout << "Failed" << endl;

    int ID [size];
    char Action [size];
    int VPage [size];
    cout << "Size of arrays: " << size << endl;

    size--;

    cout << "PID" << "\t" << "Act" << "\t" << "Page" << endl;

    int i = 0;
    while(!file.eof()){
        file >> ID[i] >> ws >> Action[i] >> ws;

        if(Action[i] == 'C' || Action[i] == 'T') VPage[i] = '\0';
        else file >> VPage[i];

        if(i <= size){
            if(Action[i] == 'C' || Action[i] == 'T'){
                cout << ID[i] << "\t" << Action[i] << endl;
            }
            else{
                cout << ID[i] << "\t" << Action[i] << "\t" << VPage[i] << endl;
            }
        }
        i++;
    }

    cout << endl;

    int NumProcesses = 0;
    //Find all unique processes
    for (int z=0; z<=size; z++){
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
    for (int k=0; k<=size; k++){
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
    for (int k=0; k<=size; k++){
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
            VirtualPage[i].PT.present[k] = false;
        }

        cout << VirtualPage[i].ID << endl;
    }

    cout << "\n" << "Number of unique processes: " << NumofUniqueProcesses << "\n" << endl;

    //*************** Load data into empty physical page table ****************************
    int pagenumber = 0, current_line = 0; bool Full = false;
    for(i = 0; i <= size; i++){
        //CHECK IF PAGE TABLE NOT FULL
        for(int u = 0; u < 20; u++){
            if(PhysicalPage[u].ID == -1){
                pagenumber = u;
                break;
            }
            if(u == 19){
                cout << "Page Table is now full." << endl;
                Full = true;
                break;
            }
        }

        if(pagenumber == 20) {
            current_line = size + 1;
            break;
        }



        //ALLOCATE
        if(Action[i] == 'A') {
            ALLOCATE(VirtualPage, PhysicalPage,current_line, NumofUniqueProcesses, pagenumber, ID, VPage,  i);
        }

        //CREATE
        if(Action[i] == 'C'){
            CREATE(VirtualPage, PhysicalPage, NumofUniqueProcesses, ID, i);
        }

        //TERMINATE
        if(Action[i] == 'T'){
            //Search for virtual page with matching ID numbers.
            for(int q = 0; q < NumofUniqueProcesses; q++){

                //Delete from page table where ID matches
                for(int t = 0; t < 20; t++){
                    if(PhysicalPage[t].ID == VirtualPage[q].ID){
                        PhysicalPage[t].ID = -1; //free flag
                        PhysicalPage[t].Accessed = 0;
                        PhysicalPage[t].Read = false;
                        PhysicalPage[t].Write = false;
                        PhysicalPage[t].Dirty = false;
                    }
                }


                if(VirtualPage[q].ID == ID[i] && VirtualPage[q].Created){
                    VirtualPage[q].Terminated = true;
                    VirtualPage[q].Allocated = false;

                    delete [] VirtualPage[q].PT.VPage2;
                    delete [] VirtualPage[q].PT.PPage2;

                    //Clear the modified flags
                    for(int r = 0; r < 200; r++){
                        VirtualPage[q].PT.modified[r] = false;
                    }
                }
            }
        }

        //READ -
        if(Action[i] == 'R'){
            READ(VirtualPage, PhysicalPage, NumofUniqueProcesses,  ID, VPage, i);
        }

        //WRITE
        if(Action[i] == 'W'){
            WRITE(VirtualPage, PhysicalPage, NumofUniqueProcesses, ID, VPage, i);
        }

        //FREE
        if(Action[i] == 'F'){
            //Search for virtual page with matching ID numbers.
            for(int q = 0; q < NumofUniqueProcesses; q++){
                if(ID[i] == VirtualPage[q].ID){
                    int location;
                    location = *(VirtualPage[q].PT.PPage2 + VPage[i]);
                    if(location != -1){
                        PhysicalPage[location].ID = -1;
                        PhysicalPage[location].Dirty = false;
                        PhysicalPage[location].Order = 0;
                        PhysicalPage[location].Read = false;
                        PhysicalPage[location].Write = false;

                        *(VirtualPage[q].PT.PPage2 + VPage[i]) = NULL;
                        *(VirtualPage[q].PT.VPage2 + VPage[i]) = NULL;
                        VirtualPage[q].PT.modified[VPage[i]] = false;
                        VirtualPage[q].PT.present[VPage[i]] = false;
                    }
                }
            }
        }
    }
    //*************** Finished loading into the physical table ****************************

    cout << "\n" << "Current line from the input file: " << current_line << "\n" << endl;

    if(Full){
        cout << "Page table is currently full, please enter the page replacement algorithm for the remaining table entries." << endl;
        //Menu goes here, Cin, all that good stuff.
        //Need to decrement current_line before passing to functions
        FIFO(VirtualPage, PhysicalPage, current_line-1, NumofUniqueProcesses);
    }


    cout << "PHYSICAL PAGE" << endl;
    for(i = 0; i < 20; i++){
        cout << i << "\t \t \t";
        if(PhysicalPage[i].ID == -1){
            cout << "FREE" << endl;
        }else{
            cout << PhysicalPage[i].ID << "    \t Precedence: " << PhysicalPage[i].Order << endl;
        }
    }

    cout << endl;
    cout << "VIRTUAL PAGE" << endl;
    for(i = 0; i < NumofUniqueProcesses; i++){
        cout << "Process: " << VirtualPage[i].ID << endl;
        for(int x = 0; x < 200; x++){
            if(VirtualPage[i].PT.modified[x] && !VirtualPage[i].Killed){
                if(VirtualPage[i].PT.present[x]){
                    cout << "\t" << "Virtual Page: " << *(VirtualPage[i].PT.VPage2+x) << "   \t Physical Page: " << *(VirtualPage[i].PT.PPage2+x) << endl;
                }
                else{
                    cout << "\t" << "Virtual Page: " << *(VirtualPage[i].PT.VPage2+x) << "   \t Physical Page: " << "SWAP" << endl;
                }
            }
        }

        if(VirtualPage[i].Killed){
            cout << "\tKILLED" << endl;
        }
    }

    return 0;
}

void FIFO(Virtual_Page *VirtualPage, Physical_Page *PhysicalPage, int current_line, int NumofUniqueProcesses){
    //Do something...




}

void ALLOCATE(Virtual_Page *VirtualPage, Physical_Page *PhysicalPage, int current_line, int NumofUniqueProcesses, int pagenumber, int ID[], int VPage[], int i){
    current_line = size + 1;
    bool created = false;
    //Search for virtual page with matching ID numbers.
    for(int q = 0; q < NumofUniqueProcesses; q++){
        if(VirtualPage[q].ID == ID[i] && VirtualPage[q].Created){
            VirtualPage[q].PT.modified[VPage[i]] = true;
            VirtualPage[q].PT.present[VPage[i]] = true;

            if(!VirtualPage[q].Allocated){
                VirtualPage[q].PT.VPage2 = new int[200];
                VirtualPage[q].PT.PPage2 = new int[20];
            }

            *(VirtualPage[q].PT.VPage2+VPage[i]) = VPage[i];
            *(VirtualPage[q].PT.PPage2+VPage[i]) = pagenumber;

            created = true; VirtualPage[q].Allocated = true;
        }
    }
    if(created){
        PhysicalPage[pagenumber].ID = ID[i]; //Need to add checker if already filled
        PhysicalPage[pagenumber].Dirty = true;
        PhysicalPage[pagenumber].Order = i;  //Time of arrival during read process
        pagenumber++;
    }

}


void CREATE(Virtual_Page *VirtualPage, Physical_Page *PhysicalPage, int NumofUniqueProcesses, int ID[], int i){
    //Search for virtual page with matching ID numbers.
    for(int q = 0; q < NumofUniqueProcesses; q++){
        if(VirtualPage[q].ID == ID[i] && !VirtualPage[q].Killed){
            VirtualPage[q].Created = true;
            VirtualPage[q].Terminated = false;
        }
    }
}

void READ(Virtual_Page *VirtualPage, Physical_Page *PhysicalPage, int NumofUniqueProcesses, int ID[], int VPage[], int i){
    //Search for virtual page with matching ID numbers.
    for(int q = 0; q < NumofUniqueProcesses; q++){
        if(VirtualPage[q].ID == ID[i] && VirtualPage[q].Allocated){
            int P = *(VirtualPage[q].PT.PPage2 + VPage[i]);
            //Read from page # of virtual process
            //Need to utilize the PT to find virtual to physical
            if(VPage[i] == *(VirtualPage[q].PT.VPage2 + VPage[i]) && PhysicalPage[P].Write){
                //Add read flag and increase accessed of physical page.
                P = *(VirtualPage[q].PT.PPage2 + VPage[i]);
                PhysicalPage[P].Read = true;
                PhysicalPage[P].Accessed++;
            }
            else{
                //Kill the process
                //Delete from page table where ID matches
                for(int t = 0; t < 20; t++){
                    if(PhysicalPage[t].ID == VirtualPage[q].ID){
                        PhysicalPage[t].ID = -1; //free flag
                        PhysicalPage[t].Accessed = 0;
                        PhysicalPage[t].Read = false;
                        PhysicalPage[t].Write = false;
                        PhysicalPage[t].Dirty = false;
                    }
                }


                if(VirtualPage[q].ID == ID[i] && VirtualPage[q].Created){
                    VirtualPage[q].Terminated = true;
                    VirtualPage[q].Allocated = false;

                    delete [] VirtualPage[q].PT.VPage2;
                    delete [] VirtualPage[q].PT.PPage2;

                    //Clear the modified flags
                    for(int r = 0; r < 200; r++){
                        VirtualPage[q].PT.modified[r] = false;
                    }
                }

                cout << "Attempted to read from a page that hasn't been written to, Process: " << VirtualPage[q].ID << endl;
                VirtualPage[q].Killed = true;
            }
        }
    }

}

void WRITE(Virtual_Page *VirtualPage, Physical_Page *PhysicalPage, int NumofUniqueProcesses, int ID[], int VPage[], int i){
    //Search for virtual page with matching ID numbers.
    for(int q = 0; q < NumofUniqueProcesses; q++){
        if(VirtualPage[q].ID == ID[i] && VirtualPage[q].Allocated){
            int P;
            //Need to utilize the PT to find virtual to physical
            if(VPage[i] == *(VirtualPage[q].PT.VPage2 + VPage[i])){
                //Add write flag and increase accessed of physical page.
                P = *(VirtualPage[q].PT.PPage2 + VPage[i]);
                PhysicalPage[P].Write = true;
                PhysicalPage[P].Accessed++;
            }
            else{
                //Kill the process
                //Delete from page table where ID matches
                for(int t = 0; t < 20; t++){
                    if(PhysicalPage[t].ID == VirtualPage[q].ID){
                        PhysicalPage[t].ID = -1; //free flag
                        PhysicalPage[t].Accessed = 0;
                        PhysicalPage[t].Read = false;
                        PhysicalPage[t].Write = false;
                        PhysicalPage[t].Dirty = false;
                    }
                }


                if(VirtualPage[q].ID == ID[i] && VirtualPage[q].Created){
                    VirtualPage[q].Terminated = true;
                    VirtualPage[q].Allocated = false;

                    delete [] VirtualPage[q].PT.VPage2;
                    delete [] VirtualPage[q].PT.PPage2;

                    //Clear the modified flags
                    for(int r = 0; r < 200; r++){
                        VirtualPage[q].PT.modified[r] = false;
                    }
                }

                VirtualPage[q].Killed = true;
            }
        }
    }
}
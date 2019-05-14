#include <iostream>
#include <fstream>
#include "stdlib.h"

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
    int ID = 0;
    int VirtualPage;
    bool modified = false;
    bool Write = false;
    bool Read = false;
    int Accessesd = 0;
    int Order = 0;
};

void ALLOCATE(Virtual_Page *VirtualPage, Physical_Page *PhysicalPage, Swap_Page *SwapPage, int NumofUniqueProcesses, int pagenumber, int ID[], int VPage[], int i, bool Full, int Policy);
void READ(Virtual_Page *VirtualPage, Physical_Page *PhysicalPage, int NumofUniqueProcesses, int ID[], int VPage[], int i, bool Full);
void CREATE(Virtual_Page *VirtualPage, Physical_Page *PhysicalPage, int NumofUniqueProcesses, int ID[], int i);
void WRITE(Virtual_Page *VirtualPage, Physical_Page *PhysicalPage, int NumofUniqueProcesses, int ID[], int VPage[], int i, bool Full);
void TERMINATE(Virtual_Page *VirtualPage, Physical_Page *PhysicalPage, int NumofUniqueProcesses, int ID[],  int );
void FREE(Virtual_Page *VirtualPage, Physical_Page *PhysicalPage, int NumofUniqueProcesses, int ID[],  int i, int VPage[]);

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

    int Policy = 0; bool valid = false;
    while(!valid){
        cout << "Enter the swap policy you would like to proceed with from the list below:" << endl;
        cout << "\t 1. FIFO \n\t 2. LRU \n\t 3. Random" << endl;
        cout << "\t Choice: ";
        cin >> Policy;
        if(Policy == 1 || Policy == 2 || Policy == 3){
            valid = true;
        }
        else{
            cout << "Invalid input." << endl;
        }
    }



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
    Swap_Page SwapPage[200];
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
    int pagenumber = 0; bool Full = false;
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

        //ALLOCATE
        if(Action[i] == 'A') {
            ALLOCATE(VirtualPage, PhysicalPage, SwapPage, NumofUniqueProcesses, pagenumber, ID, VPage, i, Full, Policy);
        }

        //CREATE
        if(Action[i] == 'C'){
            CREATE(VirtualPage, PhysicalPage, NumofUniqueProcesses, ID, i);
        }

        if(Action[i] == 'T'){
            TERMINATE(VirtualPage, PhysicalPage, NumofUniqueProcesses, ID, i);
        }

        //READ
        if(Action[i] == 'R'){
            //Check if page is swapped out
            for(int t = 0; t < NumofUniqueProcesses; t++) {
                if(VirtualPage[t].ID == ID[i] && !VirtualPage[t].PT.present[VPage[i]]) {
                    ALLOCATE(VirtualPage, PhysicalPage, SwapPage, NumofUniqueProcesses, pagenumber, ID, VPage, i, Full, Policy);
                    break;
                }
                else{
                    break;
                }
            }

            READ(VirtualPage, PhysicalPage, NumofUniqueProcesses,  ID, VPage, i, Full);
        }

        //WRITE
        if(Action[i] == 'W'){
            //Check if page is swapped out
            for(int t = 0; t < NumofUniqueProcesses; t++) {
                if(VirtualPage[t].ID == ID[i] && !VirtualPage[t].PT.present[VPage[i]]) {
                    ALLOCATE(VirtualPage, PhysicalPage, SwapPage, NumofUniqueProcesses, pagenumber, ID, VPage, i, Full, Policy);
                    break;
                }
                else{
                    break;
                }
            }

            WRITE(VirtualPage, PhysicalPage, NumofUniqueProcesses, ID, VPage, i, Full);
        }

        //FREE
        if(Action[i] == 'F'){
            FREE(VirtualPage, PhysicalPage, NumofUniqueProcesses, ID, i, VPage);
        }
    }
    //*************** Finished loading into the physical table ****************************


    cout << "PHYSICAL PAGE" << endl;
    for(i = 0; i < 20; i++){
        cout << i << "\t \t \t";
        if(PhysicalPage[i].ID == -1){
            cout << "FREE" << endl;
        }else{
            cout << PhysicalPage[i].ID << "    \t Precedence: " << PhysicalPage[i].Order << "    \t Accessed: " << PhysicalPage[i].Accessed << endl;
        }
    }

    cout << endl;
    cout << "VIRTUAL PAGE" << endl;
    for(i = 0; i < NumofUniqueProcesses; i++){
        cout << "Process: " << VirtualPage[i].ID << endl;
        for(int x = 0; x < 200; x++){
            if(VirtualPage[i].PT.modified[x] && !VirtualPage[i].Killed){
                if(VirtualPage[i].PT.present[x]){
                    cout << "\t" << "Virtual Page: " << *(VirtualPage[i].PT.VPage2+x) << "   \t Physical Page: " << *(VirtualPage[i].PT.PPage2+x)  << "\tX: " << x << endl;
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

    cout << endl;
    cout << "SWAP" << endl;
    for(int i = 0; i < 200; i++){
        if(SwapPage[i].modified){
            cout << "\t" << "Process: " << SwapPage[i].ID << "\t" << "Virtual Page: " << SwapPage[i].VirtualPage <<  endl;
        }
    }

    return 0;
}


void ALLOCATE(Virtual_Page *VirtualPage, Physical_Page *PhysicalPage, Swap_Page *SwapPage, int NumofUniqueProcesses, int pagenumber, int ID[], int VPage[], int i, bool Full, int Policy){
    if(!Full){
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


    else if(Policy == 1){ //FIFO
        //Search for virtual page with matching ID numbers.
        for(int q = 0; q < NumofUniqueProcesses; q++){

            if(VirtualPage[q].ID == ID[i] && VirtualPage[q].Created){
                        VirtualPage[q].PT.modified[VPage[i]] = true;
                        VirtualPage[q].PT.present[VPage[i]] = true;



                        if(!VirtualPage[q].Allocated){
                            VirtualPage[q].PT.VPage2 = new int[200];
                            VirtualPage[q].PT.PPage2 = new int[20];
                        }

                        VirtualPage[q].Allocated = true;

                        //Changing code
                        *(VirtualPage[q].PT.VPage2 + VPage[i]) = VPage[i];
                        //Need to find lowest precendence case and swap.
                        int minimum = 10000;
                        for(int k = 0; k < 20; k++){
                            if(PhysicalPage[k].Order < minimum){
                                minimum = PhysicalPage[k].Order;
                            }
                        }

                        for(int k = 0; k < 20; k++) {
                            if (PhysicalPage[k].Order == minimum) {
                                int n; //stores the Process matching the lowest precedence physical page.
                                for (n = 0; n < NumofUniqueProcesses; n++) {
                                    if (VirtualPage[n].ID == PhysicalPage[k].ID) break;
                                }

                                int y; //stores the array location of virtual and physical page in TLB
                                for (y = 0; y < 200; y++) {
                                    if (*(VirtualPage[n].PT.PPage2 + y) == k && VirtualPage[n].PT.modified[y]) break;
                                }

                                //Store the matching virtual page and erase physical page in an unallocated swap
                                for (int x = 0; x < 200; x++) {
                                    if (!SwapPage[x].modified) {
                                        SwapPage[x].ID = VirtualPage[n].ID;
                                        SwapPage[x].VirtualPage = *(VirtualPage[n].PT.VPage2 + y); //**************************************
                                        SwapPage[x].modified = true;

                                        //Storing physical page flags in the swap list
                                        SwapPage[x].Order = PhysicalPage[k].Order;
                                        SwapPage[x].Read = PhysicalPage[k].Read;
                                        SwapPage[x].Write = PhysicalPage[k].Write;
                                        SwapPage[x].Accessesd = PhysicalPage[k].Accessed;

                                        *(VirtualPage[n].PT.PPage2 + y) = -1;
                                        VirtualPage[n].PT.present[y] = false;
                                        *(VirtualPage[q].PT.PPage2 + VPage[i]) = k;
                                        cout << "Y: " << y << endl;
                                        break;
                                        //Store new process into physical page
                                    }
                                }
                                break;
                            }
                        }

                        //if already in swap page list
                        bool inSwap = false;
                        int h;
                        for(h = 0; h < 200; h++){
                            if(SwapPage[h].ID == ID[i] && SwapPage[h].VirtualPage == VPage[i]){
                                if(SwapPage[h].modified){
                                    SwapPage[h].modified = false;
                                    inSwap = true;

                                    cout << "Found one in swap: h = " << h << endl;
                                    cout << "***Process: " << ID[i] << ", Write: " << SwapPage[h].Write << ", Order: " << SwapPage[h].Order
                                         << ", Accessed: " << SwapPage[h].Accessesd << endl;
                                    break;
                                }
                            }
                        }

                        for(int t = 0; t < 20; t++){
                            if(PhysicalPage[t].Order == minimum && !inSwap){
                                PhysicalPage[t].ID = ID[i];
                                PhysicalPage[t].Write = false;
                                PhysicalPage[t].Read = false;
                                PhysicalPage[t].Accessed = 0;
                                PhysicalPage[t].Dirty = true;
                                PhysicalPage[t].Order = i;
                            }
                            else if(PhysicalPage[t].Order == minimum && inSwap){
                                PhysicalPage[t].ID = ID[i];
                                PhysicalPage[t].Write = SwapPage[h].Write;
                                PhysicalPage[t].Read = SwapPage[h].Read;
                                PhysicalPage[t].Accessed = SwapPage[h].Accessesd;
                                PhysicalPage[t].Dirty = true;
                                PhysicalPage[t].Order = SwapPage[h].Order;

                                cout << "Process: " << ID[i] << " was in swap, adding swap parameters to physical page: " << t << endl;
                                cout << "Parameters: \n" << "Write: " << PhysicalPage[t].Write << "\nRead: " << PhysicalPage[t].Read << "\nAccessed: " <<
                                PhysicalPage[t].Accessed << "\nOrder: " << PhysicalPage[t].Order << endl;
                            }
                        }
                        break;
             }
        }
    }

    else if(Policy == 2){ //LRU
    //Use loop to find least recently accessed instead of lowest precedence
        //Search for virtual page with matching ID numbers.
        for(int q = 0; q < NumofUniqueProcesses; q++){
            int h ;

            if(VirtualPage[q].ID == ID[i] && VirtualPage[q].Created){
                VirtualPage[q].PT.modified[VPage[i]] = true;
                VirtualPage[q].PT.present[VPage[i]] = true;

                if(!VirtualPage[q].Allocated){
                    VirtualPage[q].PT.VPage2 = new int[200];
                    VirtualPage[q].PT.PPage2 = new int[20];
                }

                VirtualPage[q].Allocated = true;

                //Changing code
                *(VirtualPage[q].PT.VPage2 + VPage[i]) = VPage[i];
                //Need to find lowest precendence case and swap.
                int minimum = 10000;
                for(int k = 0; k < 20; k++){
                    if(PhysicalPage[k].Accessed < minimum){
                        minimum = PhysicalPage[k].Accessed;
                    }
                }

                bool inSwap = false;
                for(int k = 0; k < 20; k++) {
                    if (PhysicalPage[k].Accessed == minimum) {
                        int n; //stores the Process matching the lowest precedence physical page.
                        for (n = 0; n < NumofUniqueProcesses; n++) {
                            if (VirtualPage[n].ID == PhysicalPage[k].ID) break;
                        }

                        int y; //stores the array location of virtual and physical page in TLB
                        for (y = 0; y < 200; y++) {
                            if (*(VirtualPage[n].PT.PPage2 + y) == k && VirtualPage[n].PT.modified[y]) break;
                        }

                        //Store the matching virtual page and erase physical page in an unallocated swap
                        for (int x = 0; x < 200; x++) {
                            if (!SwapPage[x].modified) {
                                SwapPage[x].ID = VirtualPage[n].ID;
                                SwapPage[x].VirtualPage = *(VirtualPage[n].PT.VPage2 + y); //**************************************
                                SwapPage[x].modified = true;

                                //Storing physical page flags in the swap list
                                SwapPage[x].Order = PhysicalPage[k].Order;
                                SwapPage[x].Read = PhysicalPage[k].Read;
                                SwapPage[x].Write = PhysicalPage[k].Write;
                                SwapPage[x].Accessesd = PhysicalPage[k].Accessed;

                                *(VirtualPage[n].PT.PPage2 + y) = -1;
                                VirtualPage[n].PT.present[y] = false;
                                *(VirtualPage[q].PT.PPage2 + VPage[i]) = k;
                                cout << "Y: " << y << endl;
                                break;
                                //Store new process into physical page
                            }
                        }

                        for(h = 0; h < 200; h++){
                            //**************************************************************************** Add method to take process out of swap if in swap.
                            if(SwapPage[h].modified && SwapPage[h].ID == ID[i] && SwapPage[h].VirtualPage == VPage[i]){
                                SwapPage[h].modified = false;
                                inSwap = true;
                                break;
                            }
                        }

                        break;
                    }
                }

                for(int t = 0; t < 20; t++){
                    if(PhysicalPage[t].Accessed == minimum && !inSwap){
                        PhysicalPage[t].ID = ID[i];
                        PhysicalPage[t].Write = false;
                        PhysicalPage[t].Read = false;
                        PhysicalPage[t].Accessed = 0;
                        PhysicalPage[t].Dirty = true;
                        PhysicalPage[t].Order = i;
                        break;
                    }
                    else if(PhysicalPage[t].Order == minimum && inSwap){
                        PhysicalPage[t].ID = ID[i];
                        PhysicalPage[t].Write = SwapPage[h].Write;
                        PhysicalPage[t].Read = SwapPage[h].Read;
                        PhysicalPage[t].Accessed = SwapPage[h].Accessesd;
                        PhysicalPage[t].Dirty = true;
                        PhysicalPage[t].Order = SwapPage[h].Order;
                        break;
                    }
                }
                break;
            }
        }
    }

    else if(Policy == 3){ //Random
        //Search for virtual page with matching ID numbers.
        for(int q = 0; q < NumofUniqueProcesses; q++){
            if(VirtualPage[q].ID == ID[i] && VirtualPage[q].Created){
                VirtualPage[q].PT.modified[VPage[i]] = true;
                VirtualPage[q].PT.present[VPage[i]] = true;

                if(!VirtualPage[q].Allocated){
                    VirtualPage[q].PT.VPage2 = new int[200];
                    VirtualPage[q].PT.PPage2 = new int[20];
                }

                VirtualPage[q].Allocated = true;

                //Changing code
                *(VirtualPage[q].PT.VPage2 + VPage[i]) = VPage[i];
                //Need to find lowest precendence case and swap.
                int k = rand() % 20;

                        int n; //stores the Process matching random physical page
                        for (n = 0; n < NumofUniqueProcesses; n++) {
                            if (VirtualPage[n].ID == PhysicalPage[k].ID) break;
                        }

                        int y; //stores the array location of virtual and physical page in TLB
                        for (y = 0; y < 200; y++) {
                            if (*(VirtualPage[n].PT.PPage2 + y) == k && VirtualPage[n].PT.modified[y]) break;
                        }

                        //Store the matching virtual page and erase physical page in an unallocated swap
                        for (int x = 0; x < 200; x++) {
                            if (!SwapPage[x].modified) {
                                SwapPage[x].ID = VirtualPage[n].ID;
                                SwapPage[x].VirtualPage = *(VirtualPage[n].PT.VPage2 + y); //**************************************
                                SwapPage[x].modified = true;

                                //Storing physical page flags in the swap list
                                SwapPage[x].Order = PhysicalPage[k].Order;
                                SwapPage[x].Read = PhysicalPage[k].Read;
                                SwapPage[x].Write = PhysicalPage[k].Write;
                                SwapPage[x].Accessesd = PhysicalPage[k].Accessed;

                                *(VirtualPage[n].PT.PPage2 + y) = -1;
                                VirtualPage[n].PT.present[y] = false;
                                *(VirtualPage[q].PT.PPage2 + VPage[i]) = k;
                                cout << "Y: " << y << endl;
                                break;
                                //Store new process into physical page
                            }
                        }
                        // ******************************************************************** Might need to add taking out of swap when looping back again

                        //if already in swap page list
                        bool inSwap = false;
                        int h;
                        for(h = 0; h < 200; h++){
                            if(SwapPage[h].ID == ID[i] && SwapPage[h].VirtualPage == VPage[i]){
                                if(SwapPage[h].modified){
                                    SwapPage[h].modified = false;
                                    inSwap = true;
                                    break;
                                }
                            }
                        }

                        if(!inSwap){
                            PhysicalPage[k].ID = ID[i];
                            PhysicalPage[k].Write = false;
                            PhysicalPage[k].Read = false;
                            PhysicalPage[k].Accessed = 0;
                            PhysicalPage[k].Dirty = true;
                            PhysicalPage[k].Order = i;
                        }
                        else if(inSwap){
                            PhysicalPage[k].ID = ID[i];
                            PhysicalPage[k].Write = SwapPage[h].Write;
                            PhysicalPage[k].Read = SwapPage[h].Read;
                            PhysicalPage[k].Accessed = SwapPage[h].Accessesd;
                            PhysicalPage[k].Dirty = true;
                            PhysicalPage[k].Order = SwapPage[h].Order;
                        }
            }
        }





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

void READ(Virtual_Page *VirtualPage, Physical_Page *PhysicalPage, int NumofUniqueProcesses, int ID[], int VPage[], int i, bool Full){
    //Search for virtual page with matching ID numbers.
    for(int q = 0; q < NumofUniqueProcesses; q++) {
        if (VirtualPage[q].ID == ID[i] && VirtualPage[q].Allocated) {
            int P = *(VirtualPage[q].PT.PPage2 + VPage[i]);

            cout << "Bool Write: " << PhysicalPage[P].Write << ", Process: " << ID[i] << ", Line: " << i+1 << ", Physical page: " << P << endl;

            //Read from page # of virtual process
            //Need to utilize the PT to find virtual to physical
            if (VPage[i] == *(VirtualPage[q].PT.VPage2 + VPage[i]) && PhysicalPage[P].Write) {  //in physical page
                //Add read flag and increase accessed of physical page.
                P = *(VirtualPage[q].PT.PPage2 + VPage[i]);
                PhysicalPage[P].Read = true;
                PhysicalPage[P].Accessed++;
            }

            else {
                //Kill the process
                //Delete from page table where ID matches
                for (int t = 0; t < 20; t++) {
                    if (PhysicalPage[t].ID == VirtualPage[q].ID) {
                        PhysicalPage[t].ID = -1; //free flag
                        PhysicalPage[t].Accessed = 0;
                        PhysicalPage[t].Read = false;
                        PhysicalPage[t].Write = false;
                        PhysicalPage[t].Dirty = false;
                    }
                }


                if (VirtualPage[q].ID == ID[i] && VirtualPage[q].Created) {
                    VirtualPage[q].Terminated = true;
                    VirtualPage[q].Allocated = false;

                    delete[] VirtualPage[q].PT.VPage2;
                    delete[] VirtualPage[q].PT.PPage2;

                    //Clear the modified flags
                    for (int r = 0; r < 200; r++) {
                        VirtualPage[q].PT.modified[r] = false;
                    }
                }

                cout << "Attempted to read from a page that hasn't been written to,  i = " << i << " Process: " << ID[i] << endl;
                VirtualPage[q].Killed = true;
            }
        }
    }
}

void WRITE(Virtual_Page *VirtualPage, Physical_Page *PhysicalPage, int NumofUniqueProcesses, int ID[], int VPage[], int i, bool Full){
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


void TERMINATE(Virtual_Page *VirtualPage, Physical_Page *PhysicalPage, int NumofUniqueProcesses, int ID[],  int i) {
    //Search for virtual page with matching ID numbers.
    for (int q = 0; q < NumofUniqueProcesses; q++) {

        //Delete from page table where ID matches
        for (int t = 0; t < 20; t++) {
            if (PhysicalPage[t].ID == VirtualPage[q].ID) {
                PhysicalPage[t].ID = -1; //free flag
                PhysicalPage[t].Accessed = 0;
                PhysicalPage[t].Read = false;
                PhysicalPage[t].Write = false;
                PhysicalPage[t].Dirty = false;
            }
        }
        if (VirtualPage[q].ID == ID[i] && VirtualPage[q].Created) {
                VirtualPage[q].Terminated = true;
                VirtualPage[q].Allocated = false;

                delete[] VirtualPage[q].PT.VPage2;
                delete[] VirtualPage[q].PT.PPage2;

                //Clear the modified flags
                for (int r = 0; r < 200; r++) {
                    VirtualPage[q].PT.modified[r] = false;
                }
            }
        }
    }

void FREE(Virtual_Page *VirtualPage, Physical_Page *PhysicalPage, int NumofUniqueProcesses, int ID[],  int i, int VPage[]) {
    for (int q = 0; q < NumofUniqueProcesses; q++) {
        if (ID[i] == VirtualPage[q].ID) {
            int location;
            location = *(VirtualPage[q].PT.PPage2 + VPage[i]);
            if (location != -1) {
                PhysicalPage[location].ID = -1;
                PhysicalPage[location].Dirty = false;
                PhysicalPage[location].Order = 0;
                PhysicalPage[location].Read = false;
                PhysicalPage[location].Write = false;

                *(VirtualPage[q].PT.PPage2 + VPage[i]) = -1;
                *(VirtualPage[q].PT.VPage2 + VPage[i]) = -1;
                VirtualPage[q].PT.modified[VPage[i]] = false;
                VirtualPage[q].PT.present[VPage[i]] = false;
            }
        }
    }
}
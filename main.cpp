#include <iostream>
#include <fstream>
#include <queue>

using namespace std;

int main() {
    ifstream file ("sample-jobs.dat");

    string line; int size = 0;
    while(!file.eof()){
        getline(file,line);
        size++;
    }
    file.close();
    file.open("sample-jobs.dat");
    size--;

    if(file.is_open()) cout << "File opened correctly." << endl;
    else cout << "Failed" << endl;

    int *ID = new int[size];
    char *Action = new char[size];
    char *VPage = new char[size];

    cout << "PID" << "\t" << "Act" << "\t" << "Page" << endl;

    while(!file.eof()){
        file >> *ID >> ws >> *Action >> ws;

        if(*Action == 'C' || *Action == 'T') *VPage = '\0';
        else file >> *VPage;

        cout << *ID << "\t" << *Action << "\t" << *VPage << endl;

        *ID++; *Action++; *VPage++;
    }

    cout << endl;
    cout << "*******************************************************************************" << endl;
    cout << "**  0  1  2  3  4  5  6  7  8  9  10  11  12  13  14  15  16  17  18  19  20 **" << endl;
    cout << "**                                                                           **" << endl;
    cout << "*******************************************************************************" << endl;

    return 0;
}
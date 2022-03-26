#include <iostream>
#include <fstream>
#include<vector>
using namespace std;
typedef unsigned long long int uint64;

int main(int argc, char ** argv)
{
    if (argc != 2) 
    {
        printf("txt2bin file.txt\n");
        return 0;
    }
    ifstream inFile(argv[1]);
    if (!inFile) {
        cout << "error" << endl;
        return 0;
    }
    char temp[16];
    uint64 buf = 0;
    ofstream outFile("out.bin", ios::binary|ios::trunc);
    while (inFile >> temp) {
        cout << temp << endl;
        sscanf(temp, "%llx", &buf);
        cout << buf << endl;
        outFile.write((char *)&buf, sizeof(buf));
    }
    inFile.close();
    outFile.close();
    return 0;
}
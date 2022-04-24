#include <fstream>
#include <iostream>
#include <vector>
using namespace std;
typedef unsigned long long int uint64;

int main(int argc, char** argv) {
  if (argc != 3) {
    printf("useage: txt2bin neusight.txt neusight_data.bin\n");
    return 0;
  }
  ifstream inFile(argv[1]);
  if (!inFile) {
    cout << "error" << endl;
    return 0;
  }
  char temp[16];
  uint64 buf = 0;
  ofstream outFile(argv[2], ios::binary | ios::trunc);
  while (inFile >> temp) {
    sscanf(temp, "%llx", &buf);
    outFile.write((char*)&buf, sizeof(buf));
  }
  inFile.close();
  outFile.close();
  return 0;
}
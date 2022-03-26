#include <iostream>
#include <fstream>
using namespace std;
typedef unsigned char uint8;
typedef unsigned int uint32;
typedef unsigned short int uint16;

#pragma packed(1)
typedef struct
{
    uint32 timestamp;
    uint16 segment_id;
    uint8 point_id;
    uint8 channel_id;
} dot_t;
#pragma packed()

int main(int argc, char** argv)
{
    if (argc != 2) 
    {
        printf("demo out.bin\n");
        return 0;
    }
    ifstream inFile(argv[1], ios::binary);
    if (!inFile) {
        cout << "error" << endl;
        return 0;
    }
    int len = 0;
    if (inFile.is_open())
    {
        inFile.seekg(0, ios::end);
        len = inFile.tellg();
        inFile.seekg(0, ios::beg);
    }

    dot_t *dots_info = new dot_t[len/8];
    inFile.read((char *)dots_info, len);
    inFile.close();
    for (int i = 0; i < len / 8; i++)
    {
        printf("%d: channel_id %d point_id %d segment_id %d timestamp %d \n", i,
               dots_info[i].channel_id, dots_info[i].point_id, dots_info[i].segment_id, dots_info[i].timestamp);
    }

    return 0;
}
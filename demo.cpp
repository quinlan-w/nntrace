#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <map>
using namespace std;
typedef unsigned char uint8;
typedef unsigned int uint32;
typedef unsigned short int uint16;


enum EU_TYPE {
    CONV,
    CV,
    TENG,
    MAU,
    SDMA,
    UNKNOW,
};

enum EU_ID {
    CONV0 = 0,
    CONV1,
    CONV2,
    CONV3,
    CONV4,
    CONV5,
    CV0,
    CV1,
    CV2,
    TENG0,
    TENG1,
    TENG2,
    MAU0,
    SDMA0,
    SDMA1,
    SDMA2,
    UNKNOW_ID,
};

typedef struct _EU {
    EU_ID id;
    EU_TYPE type;
    string name;
    set<int> channel_ids;
} EU;

EU MC50_EU_CTRL_CHANNAL_LIST[17] = {
    {CONV0, CONV, "conv0(ctrl)", {10}},
    {CONV1, CONV, "conv1(ctrl)", {11}},
    {CONV2, CONV, "conv2(ctrl)", {12}},
    {CONV3, CONV, "conv3(ctrl)", {13}},
    {CONV4, CONV, "conv4(ctrl)", {14}},
    {CONV5, CONV, "conv5(ctrl)", {15}},
    {CV0, CV, "cv0(ctrl)", {0}},
    {CV1, CV, "cv1(ctrl)", {16}},
    {CV2, CV, "cv2(ctrl)", {48}},
    {TENG0, TENG, "teng0(ctrl)", {34}},
    {TENG1, TENG, "teng1(ctrl)", {41}},
    {TENG2, TENG, "teng2(ctrl)", {57}},
    {MAU0, MAU, "mau(ctrl)", {52}},
    {SDMA0, SDMA, "sdma0(ctrl)", {39}},
    {SDMA1, SDMA, "sdma1(ctrl)", {46}},
    {SDMA2, SDMA, "sdma2(ctrl)", {62}},
    {UNKNOW_ID, UNKNOW, "unknow", {}},
};

EU MC50_EU_EXE_CHANNAL_LIST[17] = {
    {CONV0, CONV, "conv0(exe)", {4,20,26,32}},
    {CONV1, CONV, "conv1(exe)", {5,21,27,33}},
    {CONV2, CONV, "conv2(exe)", {6,22,28,53}},
    {CONV3, CONV, "conv3(exe)", {7,23,29,54}},
    {CONV4, CONV, "conv4(exe)", {8,24,30,55}},
    {CONV5, CONV, "conv5(exe)", {9,25,31,56}},
    {CV0, CV, "cv0(exe)", {1,2,3}},
    {CV1, CV, "cv1(exe)", {17,18,19}},
    {CV2, CV, "cv2(exe)", {49,50,51}},
    {TENG0, TENG, "teng0(exe)", {35,36,37,38}},
    {TENG1, TENG, "teng1(exe)", {42,43,44,45}},
    {TENG2, TENG, "teng2(exe)", {58,59,60,61}},
    {MAU0, MAU, "mau(exe)", {}},
    {SDMA0, SDMA, "sdma0(exe)", {40}},
    {SDMA1, SDMA, "sdma1(exe)", {47}},
    {SDMA2, SDMA, "sdma2(exe)", {63}},
    {UNKNOW_ID, UNKNOW, "unknow", {}},
};


typedef struct _dot_t
{
    uint32 timestamp;
    uint16 segment_id;
    uint8 point_id;
    uint8 channel_id;
} dot_t;
typedef struct _slice
{
    string name;
    string cat;
    string pid;
    string tid;
    int dur;
    int ts;
} slice;

typedef struct _nntrace
{
    /* data */
    dot_t *data_info;
    int points_num;
    vector<slice> slices;
} nntrace;

static EU channel2eu (int chn) {
    // ctrl
    for (int i = 0;i < sizeof(MC50_EU_CTRL_CHANNAL_LIST)/sizeof(EU);++i) {
        if (MC50_EU_CTRL_CHANNAL_LIST[i].channel_ids.find(chn) != MC50_EU_CTRL_CHANNAL_LIST[i].channel_ids.end()) {
            return MC50_EU_CTRL_CHANNAL_LIST[i];
        }
    }
    // exe
    // for (int i = 0;i < sizeof(MC50_EU_EXE_CHANNAL_LIST)/sizeof(EU);++i) {
    //     if (MC50_EU_EXE_CHANNAL_LIST[i].channel_ids.find(chn) != MC50_EU_EXE_CHANNAL_LIST[i].channel_ids.end()) {
    //         return MC50_EU_EXE_CHANNAL_LIST[i];
    //     }
    // }

    return MC50_EU_CTRL_CHANNAL_LIST[UNKNOW_ID];
}

static void segments2slices(nntrace *trace) {
    map<int, map<int, set<int>>> segments_map;
    for (int i = 0;i < trace->points_num; ++i) {
        segments_map[trace->data_info[i].channel_id][trace->data_info[i].segment_id].insert(trace->data_info[i].timestamp);
    }
    for (auto chn_iter = segments_map.begin(); chn_iter != segments_map.end();++chn_iter) {
        for (auto seg_iter = chn_iter->second.begin();  seg_iter != chn_iter->second.end(); ++seg_iter) {
            EU eu = channel2eu(chn_iter->first);
            if (eu.type == UNKNOW) continue;
            slice sli;
            sli.name = to_string(chn_iter->first) + "-" + to_string(seg_iter->first);
            sli.cat = to_string(eu.type);
            sli.pid = "npu";
            sli.tid = eu.name;
            sli.ts = *(seg_iter->second.begin());
            sli.dur = *seg_iter->second.rbegin() - *seg_iter->second.begin();
            trace->slices.push_back(sli);
        }
    }
    // 处理slices的重叠的情况
    // set<string> tid_name;
    // for (auto iter = trace->slices.begin(); iter != trace->slices.end(); ++iter) {
    //     tid_name.insert(iter->tid);
    // }
    // for (auto iter = tid_name.begin();iter != tid_name.end();++iter) {
    //     for (auto iner_it = trace->slices.begin(); iner_it != trace->slices.end(); ++iner_it) {
    //         if (iner_it->tid == *iter) {
    //         }
    //     }
    // }

}

inline static void insert_trace_json_head(vector<string> &json) {
    json.push_back(string("{\"traceEvents\": [\n"));
}

inline static void insert_trace_json_body(vector<string> &json, slice &sli) {
    char buffer[100];
    sprintf(buffer, "{\"name\": \"%s\", \"cat\": \"%s\", \"ph\": \"X\", \"pid\": \"%s\", \"tid\": \"%s\", \"dur\": %d, \"ts\": %d}, \n", sli.name.data(), sli.cat.data(), sli.pid.data(), sli.tid.data(), sli.dur, sli.ts);
    string line(buffer);
    json.push_back(line);
}

inline static void insert_trace_json_back(vector<string> &json) {
    if (json.size() > 1) {
        string last_one = *(json.rbegin());
        json.pop_back();
        for (auto iter = last_one.rbegin(); iter != last_one.rend(); ++iter) {
            if (*iter == ',') {
                *iter = ' ';
                json.push_back(last_one);
                break;
            }
        }
    }
    json.push_back("],\"displayTimeUnit\": \"ns\"}");
}

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

    nntrace *new_trace = new nntrace;
    new_trace->data_info = dots_info;
    new_trace->points_num = len/8;
    segments2slices(new_trace);



    for (auto iter = new_trace->slices.begin(); iter != new_trace->slices.end();++iter)
    {
        printf("name: %s, cat %s, pid %s, tid %s, dur %d, ts %d \n", iter->name.data(), \
                                                                     iter->cat.data(),\
                                                                     iter->pid.data(),\
                                                                     iter->tid.data(),\
                                                                     iter->dur,\
                                                                     iter->ts);

    }
    vector<string> json;
    insert_trace_json_head(json);
    for (auto iter = new_trace->slices.begin(); iter != new_trace->slices.end();++iter) {
        insert_trace_json_body(json, *iter);
    }
    insert_trace_json_back(json);

    ofstream outFile("demo.json", ios::trunc);
    for (auto iter = json.begin(); iter != json.end();++iter) {
        outFile.write(iter->data(), iter->size());
    }
    outFile.close();
    return 0;
}
#pragma once
#include <string>
#include <vector>
#include <set>
#include <map>
#include <string.h>

#include "neusight.h"
using namespace std;
typedef unsigned char uint8;
typedef unsigned int uint32;
typedef unsigned short int uint16;

enum DATA_TYPE
{
    UINT8,
    UINT16,
};

typedef struct _tensor
{
    string name;
    DATA_TYPE data_type;
    uint32 n;
    uint32 c;
    uint32 h;
    uint32 w;
    uint32 stride;
    bool is_ocm;
} tensor_t;

typedef struct _job
{
    string name;
    tensor_t input_tensor;  //默认一个rdma channel只读一个tensor
    tensor_t output_tensor; //默认一个wdma channel只写一个tensor
} job_t;

typedef struct _dot_t
{
    uint32 timestamp;
    uint16 segment_id;
    uint8 point_id;
    uint8 channel_id;
} dot_t;

typedef struct _args
{
    string name;
    uint32 value;
} args_t;

typedef struct _slice
{
    string name;
    string cat;
    string pid;
    string tid;
    string ph;
    int dur;
    int ts;
    args_t args;
} slice_t;

typedef struct _nntrace
{
    /* data */
    dot_t *data_info;
    job_t *jobs_info;
    int points_num;
    vector<slice_t> slices;
} nntrace;

static void segments2slices(nntrace *trace)
{
    map<int, map<int, set<int>>> segments_map;
    for (int i = 0; i < trace->points_num; ++i)
    {
        segments_map[trace->data_info[i].channel_id][trace->data_info[i].segment_id].insert(trace->data_info[i].timestamp);
    }
    for (auto chn_iter = segments_map.begin(); chn_iter != segments_map.end(); ++chn_iter)
    {
        for (auto seg_iter = chn_iter->second.begin(); seg_iter != chn_iter->second.end(); ++seg_iter)
        {
            CHANNEL chn = MC50_NEUSIGHT_CHANNELS[chn_iter->first];
            if (chn.id >= 64)
                continue;
            slice_t sli;
            sli.name = to_string(chn_iter->first) + "-" + to_string(seg_iter->first);
            sli.cat = to_string(chn.id);
            sli.pid = "npu";
            sli.ph = "X";
            sli.tid = chn.name;
            sli.ts = *(seg_iter->second.begin());
            sli.dur = *seg_iter->second.rbegin() - *seg_iter->second.begin(); // ns?
            trace->slices.push_back(sli);

            // hack job info
            if (chn.name.find("dma") != chn.name.npos || chn.name.find("bkw") != chn.name.npos)
            {
                slice_t job_bandwidth_st;
                job_bandwidth_st.ph = "C";
                job_bandwidth_st.name = "ocm";
                job_bandwidth_st.pid = "bandwidth";
                job_bandwidth_st.args.name = "ocm";
                job_bandwidth_st.args.value = *seg_iter->second.rbegin() - *seg_iter->second.begin();
                job_bandwidth_st.tid = "ocm";
                job_bandwidth_st.ts = *(seg_iter->second.begin());
                job_bandwidth_st.dur = *seg_iter->second.rbegin() - *seg_iter->second.begin(); // ns?
                trace->slices.push_back(job_bandwidth_st);

                slice_t job_bandwidth_end;
                job_bandwidth_end.ph = "C";
                job_bandwidth_end.name = "ocm";
                job_bandwidth_end.pid = "bandwidth";
                job_bandwidth_end.args.name = "ocm";
                job_bandwidth_end.args.value = 0;
                job_bandwidth_end.tid = "ocm";
                job_bandwidth_end.ts = *(seg_iter->second.rbegin());
                trace->slices.push_back(job_bandwidth_end);
            }
        }
    }
}

static void insert_trace_json_head(vector<string> &json)
{
    json.push_back(string("{\"traceEvents\": [\n"));
}

static void insert_trace_json_body(vector<string> &json, slice_t &sli)
{
    char buffer[128];
    if (sli.ph == "X")
    {
        sprintf(buffer, "{\"name\": \"%s\", \"cat\": \"%s\", \"ph\": \"%s\", \"pid\": \"%s\", \"tid\": \"%s\", \"dur\": %d, \"ts\": %d}, \n", sli.name.data(), sli.cat.data(), sli.ph.data(), sli.pid.data(), sli.tid.data(), sli.dur, sli.ts);
    }
    if (sli.ph == "C")
    {
        sprintf(buffer, "{\"name\": \"%s\", \"ph\": \"%s\", \"pid\": \"%s\", \"dur\": %d, \"ts\": %d, \"args\": {\"%s\": %d}}, \n", sli.name.data(), sli.ph.data(), sli.pid.data(), sli.dur, sli.ts, sli.args.name.data(), sli.args.value);
    }
    string line(buffer);
    json.push_back(line);
}

static void insert_trace_json_back(vector<string> &json)
{
    if (json.size() > 1)
    {
        string last_one = *(json.rbegin());
        json.pop_back();
        for (auto iter = last_one.rbegin(); iter != last_one.rend(); ++iter)
        {
            if (*iter == ',')
            {
                *iter = ' ';
                json.push_back(last_one);
                break;
            }
        }
    }
    json.push_back("],\"displayTimeUnit\": \"ns\"}");
}

vector<string> get_nntrace(char *neusight_data, int len)
{
    // TODO: CHECK DATA
    int dots_num = len / sizeof(dot_t);
    dot_t *dots_info = (dot_t *)neusight_data;
    nntrace *new_trace = new nntrace;
    vector<string> json;

    new_trace->data_info = dots_info;
    new_trace->points_num = dots_num;

    segments2slices(new_trace);

    // log
    //  for (auto iter = new_trace->slices.begin(); iter != new_trace->slices.end();++iter)
    //  {
    //      printf("name: %s, cat %s, pid %s, tid %s, dur %d, ts %d \n",      iter->name.data(), \
    //                                                                  iter->cat.data(),\
    //                                                                  iter->pid.data(),\
    //                                                                  iter->tid.data(),\
    //                                                                  iter->dur,\
    //                                                                  iter->ts);

    // }

    insert_trace_json_head(json);
    for (auto iter = new_trace->slices.begin(); iter != new_trace->slices.end(); ++iter)
    {
        insert_trace_json_body(json, *iter);
    }
    insert_trace_json_back(json);

    return json;
}
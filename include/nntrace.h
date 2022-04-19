#pragma once
#include <string>
#include <vector>
#include <set>
#include <map>
#include <math.h>
#include <string.h>

#include "neusight.h"
using namespace std;
typedef unsigned char uint8;
typedef unsigned int uint32;
typedef unsigned short int uint16;
#define ALIGN_DOWN(x, align) ((x) & ~((align)-1))


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
    uint32 stride = 0;
    bool is_ocm;
} tensor_t;

typedef struct _job
{
    string name;
    tensor_t input_tensor;  //默认一个rdma channel只读一个tensor
    tensor_t output_tensor; //默认一个wdma channel只写一个tensor
} job_t;

typedef struct _jobs_header
{
    uint32 version;
    uint32 keys_offset;
    uint32 keys_size;
    uint32 jobs_offset;
    uint32 jobs_size;
} jobs_header;


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

// typedef struct _nntrace
// {
//     uint32 dots_num;
//     map<int, job_t> job_map;
//     map<int, map<int, set<int>>> segments_map;
// } nntrace;
uint32 get_dots_num(const char *neusight_data, uint32 neusight_buf_capacity) {
    dot_t *dots_info = (dot_t *)neusight_data;
    uint32 aligned_capacity = ALIGN_DOWN(neusight_buf_capacity, sizeof(dot_t));
    uint32 dots_num_max = aligned_capacity / sizeof(dot_t);
    uint32 steps = (uint32)sqrt(dots_num_max);
    uint32 sub_step = steps;

    uint32 real_dots_idx = 0;
    for (int i = 0; i < steps; ++i) {
        if ( i == steps - 1) {
            sub_step = dots_num_max - i * steps;
        }
        for(int j =0;j< sub_step;++j) {
            if ((dots_info[i].channel_id || dots_info[i].point_id || dots_info[i].segment_id || dots_info[i].timestamp) == 0) break;
            real_dots_idx = i * steps + j;

            if (real_dots_idx == dots_num_max) {
                printf("neusight buf is full!\n");
            }
        }
    }

    return real_dots_idx + 1;
}

static void parser_neusight_data(const char * neusight_data, uint32 neusight_buf_capacity, map<int, map<int, set<int>>> &dots_map) {
    uint32 dots_num = get_dots_num(neusight_data, neusight_buf_capacity);
    dot_t * dots_info = (dot_t *) neusight_data;
    for (int i = 0; i < dots_num; ++i)
    {
        dots_map[dots_info[i].channel_id][dots_info[i].segment_id].insert(dots_info[i].timestamp);
    }
}

static void parser_jobs_data(const char * jobs_data, map<uint32, job_t> &jobs_map) {
    if (!jobs_data) return;
    jobs_header * header = (jobs_header *)jobs_data;
    // TODO: CHECK
    uint32 * keys = (uint32 *)(jobs_data + header->keys_offset);
    job_t * jobs = (job_t*)(jobs_data + header->jobs_offset);
    uint32 num = header->jobs_size / (sizeof(job_t));
    for (int i = 0;i < num;++i) {
        jobs_map[keys[i]] = jobs[i];
    }
}

static void gen_slices(map<int, map<int, set<int>>> &dots_map, map<uint32, job_t> &job_map, vector<slice_t> &slices)
{

    for (auto chn_iter = dots_map.begin(); chn_iter != dots_map.end(); ++chn_iter)
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
            slices.push_back(sli);

            // hack job info
            uint32 key = (chn.id & 0xFF)  << 16 | ( seg_iter->first & 0xFFFF) << 0;
            printf("key: %08x\n", key);
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
                slices.push_back(job_bandwidth_st);

                slice_t job_bandwidth_end;
                job_bandwidth_end.ph = "C";
                job_bandwidth_end.name = "ocm";
                job_bandwidth_end.pid = "bandwidth";
                job_bandwidth_end.args.name = "ocm";
                job_bandwidth_end.args.value = 0;
                job_bandwidth_end.tid = "ocm";
                job_bandwidth_end.ts = *(seg_iter->second.rbegin());
                slices.push_back(job_bandwidth_end);
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


vector<string> get_nntrace(char *neusight_data, uint32 neusight_buf_capacity, char *jobs_data = nullptr)
{
    dot_t *dots_info = (dot_t *)neusight_data;
    map<uint32, job_t> jobs_map;
    map<int, map<int, set<int>>> dots_map;
    vector<slice_t> slices;
    vector<string> json;

    parser_neusight_data(neusight_data, neusight_buf_capacity, dots_map);
    parser_jobs_data(jobs_data, jobs_map);
    gen_slices(dots_map, jobs_map, slices);

    // log
    for (auto iter = slices.begin(); iter != slices.end();++iter)
    {
        printf("name: %s, cat %s, pid %s, tid %s, dur %d, ts %d \n",      iter->name.data(), \
                                                                    iter->cat.data(),\
                                                                    iter->pid.data(),\
                                                                    iter->tid.data(),\
                                                                    iter->dur,\
                                                                    iter->ts);

    }

    insert_trace_json_head(json);
    for (auto iter = slices.begin(); iter != slices.end(); ++iter)
    {
        insert_trace_json_body(json, *iter);
    }
    insert_trace_json_back(json);

    return json;
}
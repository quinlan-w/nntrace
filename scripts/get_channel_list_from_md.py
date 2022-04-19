"""
befor:
| channel id | signal              | description    |
| ---------- | ------------------- | -------------- |
| 0          | cv_dsight[0][0]     | cv0单元ctrl通道    |
| 1          | cv_dsight[0][1]     | cv0单元rdma0通道   |
| 2          | cv_dsight[0][2]     | cv0单元rdma1通道   |
| 3          | cv_dsight[0][3]     | cv0单元wdma通道    |
| 4          | conv_ch1_dsight[0]  | conv0单元bkw通道   |
| 5          | conv_ch1_dsight[1]  | conv1单元bkw通道   |
| 6          | conv_ch1_dsight[2]  | conv2单元bkw通道   |
| 7          | conv_ch1_dsight[3]  | conv3单元bkw通道   |
| 8          | conv_ch1_dsight[4]  | conv4单元bkw通道   |
| 9          | conv_ch1_dsight[5]  | conv5单元bkw通道   |
| 10         | conv_ch0_dsight[0]  | conv0单元ctrl通道  |
| 11         | conv_ch0_dsight[1]  | conv1单元ctrl通道  |
| 12         | conv_ch0_dsight[2]  | conv2单元ctrl通道  |
| 13         | conv_ch0_dsight[3]  | conv3单元ctrl通道  |
| 14         | conv_ch0_dsight[4]  | conv4单元ctrl通道  |
| 15         | conv_ch0_dsight[5]  | conv5单元ctrl通道  |
| 16         | cv_dsight[1][0]     | cv1单元ctrl通道    |
| 17         | cv_dsight[1][1]     | cv1单元rdma0通道   |
| 18         | cv_dsight[1][2]     | cv1单元rdma1通道   |
| 19         | cv_dsight[1][3]     | cv1单元wdma通道    |
| 20         | conv_ch3_dsight[0]  | conv0单元rdma1通道 |
| 21         | conv_ch3_dsight[1]  | conv1单元rdma1通道 |
| 22         | conv_ch3_dsight[2]  | conv2单元rdma1通道 |
| 23         | conv_ch3_dsight[3]  | conv3单元rdma1通道 |
| 24         | conv_ch3_dsight[4]  | conv4单元rdma1通道 |
| 25         | conv_ch3_dsight[5]  | conv5单元rdma1通道 |
| 26         | conv_ch2_dsight[0]  | conv0单元rdma0通道 |
| 27         | conv_ch2_dsight[1]  | conv1单元rdma0通道 |
| 28         | conv_ch2_dsight[2]  | conv2单元rdma0通道 |
| 29         | conv_ch2_dsight[3]  | conv3单元rdma0通道 |
| 30         | conv_ch2_dsight[4]  | conv4单元rdma0通道 |
| 31         | conv_ch2_dsight[5]  | conv5单元rdma0通道 |
| 32         | conv_ch4_dsight[0]  | conv0单元wdma通道  |
| 33         | conv_ch4_dsight[1]  | conv1单元wdma通道  |
| 34         | teng_dsight[0][0]   | teng0单元ctrl通道  |
| 35         | teng_dsight[0][1]   | teng0单元dma0通道  |
| 36         | teng_dsight[0][2]   | teng0单元dma1通道  |
| 37         | teng_dsight[0][3]   | teng0单元dma2通道  |
| 38         | teng_dsight[0][4]   | teng0单元dma3通道  |
| 39         | teng_dsight[0][5]   | sdma0单元ctrl通道  |
| 40         | teng_dsight[0][6]   | sdma0单元dma通道   |
| 41         | teng_dsight[1][0]   | teng1单元ctrl通道  |
| 42         | teng_dsight[1][1]   | teng1单元dma0通道  |
| 43         | teng_dsight[1][2]   | teng1单元dma1通道  |
| 44         | teng_dsight[1][3]   | teng1单元dma2通道  |
| 45         | teng_dsight[1][4]   | teng1单元dma3通道  |
| 46         | teng_dsight[1][5]   | sdma1单元ctrl通道  |
| 47         | teng_dsight[1][6]   | sdma1单元dma通道   |
| 48         | cv_dsight[2][0]     | cv2单元ctrl通道    |
| 49         | cv_dsight[2][1]     | cv2单元rdma0通道   |
| 50         | cv_dsight[2][2]     | cv2单元rdma1通道   |
| 51         | cv_dsight[2][3]     | cv2单元wdma通道    |
| 52         | mau_dsight          | mau单元ctrl通道    |
| 53         | conv_ch4_dsight[2]  | conv2单元wdma通道  |
| 54         | conv_ch4_dsight[3]  | conv3单元wdma通道  |
| 55         | conv_ch4_dsight[4]  | conv4单元wdma通道  |
| 56         | conv_ch4_dsight[5]  | conv5单元wdma通道  |
| 57         | teng_dsight[2][0]   | teng2单元ctrl通道  |
| 58         | teng_dsight[2][1]   | teng2单元dma0通道  |
| 59         | teng_dsight[2][2]   | teng2单元dma1通道  |
| 60         | teng_dsight[2][3]   | teng2单元dma2通道  |
| 61         | teng_dsight[2][4]   | teng2单元dma3通道  |
| 62         | teng_dsight[2][5]   | sdma2单元ctrl通道  |
| 63         | teng_dsight[2][6]   | sdma2单元dma通道   |
| 64         | conv_ch5_asight[2]  | conv2单元累加通道    |
| 65         | conv_ch5_asight[3]  | conv3单元累加通道    |
| 66         | conv_ch5_asight[4]  | conv4单元累加通道    |
| 67         | conv_ch5_asight[5]  | conv5单元累加通道    |
| 68         | conv_ch5_asight[1]} | conv1单元累加通道    |
| 69         | teng_asight[0]      | teng0单元累加通道    |
| 70         | teng_asight[1]      | teng1单元累加通道    |
| 71         | teng_asight[2]      | teng2单元累加通道    |
| 72         | conv_ch5_asight[0]  | conv0单元累加通道    |
| 73         | cv_asight[0]        | cv0单元累加通道      |
| 74         | cv_asight[1]        | cv1单元累加通道      |
| 75         | cv_asight[2]        | cv2单元累加通道      |

after:

"""
import argparse

from yaml import parse

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="translate markdown file to neusight channel list"
    )
    parser.add_argument("-o", "--code")
    parser.add_argument("-m", "--markdown")
    args = parser.parse_args()
    c_file = open(args.code, "w")
    f = open(args.markdown)
    lines = f.readlines()
    for line in lines[2:]:
        new_line = line.replace("单元", "/").replace("通道", "/").replace("累加", "count")
        _, channel_id, _signal, description, _ = new_line.split("|")
        eu_name, sub_name, _ = description.split("/")
        eu_id = eu_name.upper()
        eu_type = eu_name.upper()[0:-1]
        name = eu_name + "_" + sub_name
        wline = (
            "{"
            + channel_id.strip()
            + ", "
            + eu_id
            + ", "
            + eu_type
            + ", "
            + '"'
            + name.strip()
            + '"'
            + "}"
            + ",\n"
        )
        c_file.write(wline)
    c_file.close()
    f.close()

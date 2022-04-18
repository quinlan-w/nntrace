#!/usr/bin/bash
./txt2bin $1 neusight_data.bin
./bin2json neusight_data.bin $2
rm neusight_data.bin
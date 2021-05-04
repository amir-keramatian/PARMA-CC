#! /bin/bash

MinPts=500
Epsilon=0.001

pushd ../../executables/Baseline/; rm build --r; mkdir build; cd build; cmake ../; make; popd
executable_baseline="../../executables/Baseline/build/baseline_exec "

outputDir="./baseline"
mkdir -p ${outputDir}
mkdir -p ${outputDir}/labeling
mkdir -p ${outputDir}/stats

item=" -f ../../datasets/geolife/geolife_subset.txt -m ${MinPts} -e ${Epsilon} -O ${outputDir}"

command_baseline=$executable_baseline$item
$command_baseline

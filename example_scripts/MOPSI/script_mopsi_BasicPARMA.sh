#! /bin/bash

for method in "FlatPARMA" "HierarchicalPARMA"
do
    MinPts=500
    Epsilon=0.1

    pushd ../../executables/${method}/; rm build --r; mkdir build; cd build; cmake ../; make; popd
    executable_PARMA="../../executables/${method}/build/parma_exec "

    pushd ../../executables/Baseline/; rm build --r; mkdir build; cd build; cmake ../; make; popd
    executable_baseline="../../executables/Baseline/build/baseline_exec "

    pushd ../../executables/RandIndex/; rm build --r; mkdir build; cd build; cmake ../; make; popd
    executable_randindex="../../executables/RandIndex/build/omputeAccuracy_fast "
    
    outputDir="./${method}_results"
    mkdir -p ${outputDir}
    mkdir -p ${outputDir}/labeling
    mkdir -p ${outputDir}/stats
    
    for K in 2 3 4 5 10 15 20 30 36 40 50 60 70
    do
	S=$K
	item=" -f ../../datasets/mopsi/mopsi_subset.txt -m ${MinPts} -e ${Epsilon} -a 1 -K ${K} -S ${S} -O ${outputDir}"
	command_PARMA=$executable_PARMA$item
	
	for R in 0 1 2 3 4 5 6 7 8 9
	do
	    $command_PARMA
	done

	command_accuracy="${executable_randindex}./baseline/labeling/mopsi_subset.txt_baseline.idx ${outputDir}/labeling/mopsi_subset.txt_${method}_K_${K}_S_${S}.idx ${outputDir}/stats/accuracy.txt"
	$command_accuracy
    done
done

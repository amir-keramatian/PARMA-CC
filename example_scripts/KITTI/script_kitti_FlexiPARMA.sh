#! /bin/bash

for method in "FlexiFlatPARMA" "FlexiXHierarchicalPARMA" "FlexiAgileHierarchicalPARMA"
do
    
    MinPts=10
    Epsilon=0.7

    pushd ../../executables/${method}/; rm build --r; mkdir build; cd build; cmake ../; make; popd
    executable_PARMA="../../executables/${method}/build/parma_exec "

    pushd ../../executables/Baseline/; rm build --r; mkdir build; cd build; cmake ../; make; popd
    executable_baseline="../../executables/Baseline/build/baseline_exec "

    pushd ../../executables/RandIndex/; rm build --r; mkdir build; cd build; cmake ../; make; popd
    executable_randindex="../../executables/RandIndex/build/omputeAccuracy_fast "
    
    for S in 20 40 70 100 140
    do
	outputDir="./${method}_results/${S}splits"

	mkdir -p ${outputDir}
	mkdir -p ${outputDir}/labeling
	mkdir -p ${outputDir}/stats

	performBaseline=1
	
	for K in 2 3 4 5 10 15 20 30 36 40 50 60 70
	do
	    for scan in {1000..1043}
	    do
		item=" -f ../../datasets/kitti/scan${scan} -m ${MinPts} -e ${Epsilon} -a 1 -K ${K} -S ${S} -O ${outputDir}"

		command_PARMA=$executable_PARMA$item
		$command_PARMA

		if [[ $performBaseline -eq 1  ]]
		then
		    command_baseline=$executable_baseline$item
		    $command_baseline
		fi

		command_accuracy="${executable_randindex}${outputDir}/labeling/scan${scan}_baseline.idx ${outputDir}/labeling/scan${scan}_${method}_K_${K}_S_${S}.idx ${outputDir}/stats/accuracy.txt"
		$command_accuracy
		
	    done
	    performBaseline=0
	done
    done
done

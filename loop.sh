#!/bin/bash


function lbp_algo 
{
	if [ -z "$1" ] || [ -z "$2" ]; then
		echo error: in $0
		exit
	fi
	lbp_dataset=$1
	lbp_threshold=$2
	./LBP "$lbp_dataset"_Training.csv "$lbp_dataset"_Testing.csv $lbp_threshold
}


function eigen_algo 
{
	if [ -z "$1" ] || [ -z "$2" ] || [ -z "$3" ] || [ -z "$4" ]; then
		echo error: in $0
		exit
	fi
	eigen_dataset=$1
	eigen_threshold=$2
	eigen_finalThreshold=$3
	eigen_step=$4
	./EigenFaces "$eigen_dataset"_Training.csv "$eigen_dataset"_Testing.csv $eigen_threshold $eigen_finalThreshold $eigen_step
}



if [ -z "$1" ] || [ -z "$2" ] || [ -z "$3" ] || [ -z "$4" ] || [ -z "$5" ] || [ -z "$6" ]; then
	echo usage: $0 algo dataset Repetetion initial_threshold final_threshold step
	echo algo is either lbp or eigen
	exit
fi

algo=$1
dataset=$2
repeat=$3
initial_threshold=$4
final_threshold=$5
step=$6
threshold=$initial_threshold
counter=1

#if [ "$algo" == "all" ]; then
	#echo Compiling Both Files
	#make all
#elif [ "$algo" == "lbp"	]; then
	#echo Compiling LBP File
	#make lbp
#elif [ "$algo" == 'eigen' ]; then
	#echo Compiling EigenFaces File
	#make eigen
#fi

while [ $counter -le $repeat ];do
		echo Iteration $counter of $repeat
		if [ "$algo" == 'eigen' ]; then
			eigen_algo $dataset $threshold $final_threshold $step
		else 
			while [ $threshold -le $final_threshold ]; do
				echo -e "\t Threshold $threshold"
				if [ "$algo" == "lbp" ]; then
					lbp_algo $dataset $threshold
				fi
				let threshold=threshold+step
			done
			let threshold=initial_threshold
		fi
		echo -e "\t Done with $counter iteration"
		let counter=counter+1
done



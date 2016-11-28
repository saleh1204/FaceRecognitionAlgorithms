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
	if [ -z "$1" ] || [ -z "$2" ]; then
		echo error: in $0
		exit
	fi
	eigen_dataset=$1
	eigen_threshold=$2
	./EigenFaces "$eigen_dataset"_Training.csv "$eigen_dataset"_Testing.csv $eigen_threshold
}



if [ -z "$1" ] || [ -z "$2" ] || [ -z "$3" ] || [ -z "$4" ] || [ -z "$5" ] || [ -z "$6" ]; then
	echo usage: $0 algo dataset Repetetion initial_threshold final_threshold step
	echo algo is either lbp,eigen,all
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

if [ "$algo" == "all" ]; then
	echo Compiling Both Files
	#make all
elif [ "$algo" == "lbp"	]; then
	echo Compiling LBP File
	#make lbp
elif [ "$algo" == 'eigen' ]; then
	echo Compiling EigenFaces File
	#make eigen
fi

while [ $counter -le $repeat ];do
	echo Iteration $counter of $repeat
	while [ $threshold -le $final_threshold ]; do
		echo -e "\t Threshold $threshold"
		if [ "$algo" == "all" ]; then
			lbp_algo $dataset $threshold
			eigen_algo $dataset $threshold
		elif [ "$algo" == "lbp"	]; then
			lbp_algo $dataset $threshold
		elif [ "$algo" == 'eigen' ]; then
			eigen_algo $dataset $threshold
		fi
		let threshold=threshold+step
	done
	echo -e "\t Done with one iteration"
	let threshold=initial_threshold
	let counter=counter+1
done



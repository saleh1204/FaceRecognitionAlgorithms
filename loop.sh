#!/bin/bash


function lbp_algo 
{
	if [ -z "$1" ]; then
		echo error: in $0
		exit
	fi
	lbp_dataset=$1
	./LBP $lbp_dataset
}


function eigen_algo 
{
	if [ -z "$1" ]; then
		echo error: in $0
		exit
	fi
	eigen_dataset=$1
	./EigenFaces $eigen_dataset
}



if [ -z "$1" ] && [ -z "$2" ] && [ -z "$3" ]; then
	echo usage: $0 algo dataset Repetetion
	echo algo is either lbp,eigen,all
	exit
fi

algo=$1
dataset=$2
repeat=$3
counter=0
if [ "$algo" == "all" ]; then
	echo Compiling Both Files
	make all
elif [ "$algo" == "lbp"	]; then
	echo Compiling LBP File
	make lbp
elif [ "$algo" == 'eigen' ]; then
	echo Compiling EigenFaces File
	make eigen
fi

while [ $counter -lt $repeat ];do
	echo Iteration $counter of $repeat
	if [ "$algo" == "all" ]; then
		lbp_algo $dataset
		eigen_algo $dataset
	elif [ "$algo" == "lbp"	]; then
		lbp_algo $dataset
	elif [ "$algo" == 'eigen' ]; then
		eigen_algo $dataset
	fi
	
	let counter=counter+1
done



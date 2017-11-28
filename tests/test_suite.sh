#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

T1="$(${DIR}/testChealpixNeighboursNest | md5sum | awk '{ print $1}')"
T2="$(md5sum ${DIR}/data/tests_neighbours_nside16_nested.out | awk '{ print $1}')"
if [ "${T1}" != "${T2}" ]
then 
	echo "Test for testChealpixNeighboursNest has failed"
	false;
else
	true
fi

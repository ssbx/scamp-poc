#!/bin/bash

STATUS=0

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

echo "=> testing begin"
T1="$(${DIR}/testChealpixNeighboursNest | md5sum | awk '{ print $1}')"
T2="$(md5sum ${DIR}/data/tests_neighbours_nside16_nested.out | awk '{ print $1}')"
if [ "${T1}" != "${T2}" ]
then 
	echo "=> Test for testChealpixNeighboursNest has failed"
	STATUS=1
else
	echo "=> Succesfull test testChealpixNeighboursNest"
fi
echo "=> testing end"
exit $STATUS

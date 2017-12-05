#!/bin/bash

STATUS=0

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

echo "=> Test suite start"

echo "==> Running testChealpixNeighboursNest"
T1="$(${DIR}/testChealpixNeighboursNest | md5sum | awk '{ print $1}')"
T2="$(md5sum ${DIR}/data/tests_neighbours_nside16_nested.out | awk '{ print $1}')"
if [ "${T1}" != "${T2}" ]
then 
	printf "%-50s %30s\n" "===> Test for testChealpixNeighboursNest" "FAILED"
	STATUS=1
else
	printf "%-50s %30s\n" "===> Test for testChealpixNeighboursNest" "SUCCESS"
fi

echo "==> Running testSingleCatCrossmatch"
${DIR}/testSingleCatCrossmatch ${DIR}/data/fitscat/data2.fits.cat > /dev/null
if [ $? -gt 0 ]
then 
	printf "%-50s %30s\n" "===> Test for testSingleCatCrossmatch" "FAILED"
	STATUS=1
else
	printf "%-50s %30s\n" "===> Test for testSingleCatCrossmatch" "SUCCESS"
fi

echo "==> Running testChealpixsphereAvltree"
${DIR}/testChealpixsphereAvltree > /dev/null
if [ $? -gt 0 ]
then 
	printf "%-50s %30s\n" "===> Test for testChealpixsphereAvltree" "FAILED"
	STATUS=1
else
	printf "%-50s %30s\n" "===> Test for testChealpixsphereAvltree" "SUCCESS"
fi

echo "=> Test suite end"
exit $STATUS


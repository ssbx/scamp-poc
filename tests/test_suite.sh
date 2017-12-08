#!/bin/bash

STATUS=0

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

echo "=> Test suite start"


echo "==> Running testChealpixNeighboursNest"
T1="$(${DIR}/testChealpixNeighboursNest | md5sum | awk '{ print $1}')"
T2="$(md5sum ${DIR}/data/tests_neighbours_nside16_nested.out | awk '{ print $1}')"
if [ "${T1}" != "${T2}" ]
then 
	printf "%-70s %10s\n" "===> Test for testChealpixNeighboursNest" "FAILED"
	STATUS=1
else
	printf "%-70s %10s\n" "===> Test for testChealpixNeighboursNest" "SUCCESS"
fi

echo "==> Running testAngdist"
${DIR}/testAngdist > /dev/null
if [ $? -gt 0 ]
then 
    printf "%-70s %10s\n" "===> Test for testAngdist" "FAILED"
    STATUS=1
else
    printf "%-70s %10s\n" "===> Test for testAngdist" "SUCCESS"
fi

echo "==> Running testChealpixsphereAvltree"
${DIR}/testChealpixsphereAvltree > /dev/null
if [ $? -gt 0 ]
then 
	printf "%-70s %10s\n" "===> Test for testChealpixsphereAvltree" "FAILED"
	STATUS=1
else
	printf "%-70s %10s\n" "===> Test for testChealpixsphereAvltree" "SUCCESS"
fi


echo "==> Running testSingleCatCrossmatchAvltree"
${DIR}/testSingleCatCrossmatch ${DIR}/data/fitscat/data2.fits.cat avl > /dev/null
if [ $? -gt 0 ]
then 
	printf "%-70s %10s\n" "===> Test for testSingleCatCrossmatch Avltree/Neighbors version" "FAILED"
	STATUS=1
else
	printf "%-70s %10s\n" "===> Test for testSingleCatCrossmatch Avltree/Neighbors version" "SUCCESS"
fi


echo "==> Running testSingleCatCrossmatchBigarray"
${DIR}/testSingleCatCrossmatch ${DIR}/data/fitscat/data2.fits.cat > /dev/null
if [ $? -gt 0 ]
then 
	printf "%-70s %10s\n" "===> Test for testSingleCatCrossmatch Bigarray/Neighbors version" "FAILED"
	STATUS=1
else
	printf "%-70s %10s\n" "===> Test for testSingleCatCrossmatch Bigarray/Neighbors version" "SUCCESS"
fi

echo "==> Running testCrossmatchNumber"
${DIR}/testCrossmatchNumber > /dev/null
if [ $? -gt 0 ]
then 
    printf "%-70s %10s\n" "===> Test for testCrossmatchNumber" "FAILED"
    STATUS=1
else
    printf "%-70s %10s\n" "===> Test for testCrossmatchNumber" "SUCCESS"
fi

echo "==> Running testCrossmatchLimit"
${DIR}/testCrossmatchLimit > /dev/null
if [ $? -gt 0 ]
then 
    printf "%-70s %10s\n" "===> Test for testCrossmatchLimit" "FAILED"
    STATUS=1
else
    printf "%-70s %10s\n" "===> Test for testCrossmatchLimit" "SUCCESS"
fi


echo "=> Test suite end"


exit $STATUS

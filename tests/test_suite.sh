#!/bin/bash

STATUS=0

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

echo "=> testing begin"

echo "==> Running testChealpixNeighboursNest"
T1="$(${DIR}/testChealpixNeighboursNest | md5sum | awk '{ print $1}')"
T2="$(md5sum ${DIR}/data/tests_neighbours_nside16_nested.out | awk '{ print $1}')"
if [ "${T1}" != "${T2}" ]
then 
	echo "===> Test for testChealpixNeighboursNest FAILED"
	STATUS=1
else
	echo "===> Test for testChealpixNeighboursNest SUCCESS"
fi

echo "==> Running testSingleCatCrossmatch"
${DIR}/testSingleCatCrossmatch ${DIR}/data/fitscat/data2.fits.cat > /dev/null 2>&1
if [ $? -gt 0 ]
then 
	echo "===> Test for testSingleCatCrossmatch FAILED"
	STATUS=1
else
	echo "===> Test for testSingleCatCrossmatch SUCCESS "
fi

echo "=> testing end"
exit $STATUS

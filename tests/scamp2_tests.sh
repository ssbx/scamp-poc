#!/bin/bash

SRCDIR=$(cd $(dirname ${BASH_SOURCE[0]}) && pwd)

echo "Crossmatch two ASCII files..."
$SRCDIR/../src/scamp2 -vvt -f 1.0 $SRCDIR/data/ascii/data*.txt
if [ $? -eq 0 ]; then
    echo "...done"
else
    echo "...done with ERRORS"
fi
#
echo "Crossmatch two FITS LDAC catalogs files..."
$SRCDIR/../src/scamp2 -vvt -f 1.0 $SRCDIR/data/fitscat/*.cat
if [ $? -eq 0 ]; then
    echo "...done"
else
    echo "...done with ERRORS"
fi

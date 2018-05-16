#!/bin/bash

# Runs Newton on tests in directories listed in $TESTDIRS
# Caution: the name of the test files should be different, even when
#          they're in different directories; otherwise, one's output
#          will be overwritten by the other.
# Puts the result in Examples/cprover/tests/regression/print_hull.html

trap "exit" INT

shopt -s nullglob

NEWTON_DIR="$(pwd)"

SUITE="$NEWTON_DIR/Examples/cprover/tests/regression"
TESTDIRS=( $NEWTON_DIR/Examples/cprover/tests/c4b_print_hull $NEWTON_DIR/Examples/cprover/tests/STAC/polynomial/PH $NEWTON_DIR/Examples/cprover/tests/STAC/canonical $NEWTON_DIR/Examples/cprover/tests/STAC/E3Model $NEWTON_DIR/Examples/cprover/tests/PrintHullRegressions  $NEWTON_DIR/Examples/cprover/tests/strings_numeric_PH )
#TESTDIRS=( $NEWTON_DIR/Examples/cprover/tests/tmp_single_printhull )
#TESTDIRS=( $NEWTON_DIR/Examples/cprover/tests/c4b_print_hull )

NEWTON="$NEWTON_DIR/_build64/Examples/cprover/NewtonOcaml"

# The outputs are saved here
OUTDIR="$SUITE/outputs"
# The input tests are copied here
INDIR="$SUITE/inputs"
RESULT="$OUTDIR/__result_PH.out"
SCRIPT="$SUITE/toHTML_printHull.py"

#TIMEOUT="10m"
TIMEOUT="20m"

mkdir -p $OUTDIR
mkdir -p $INDIR
rm -f $RESULT

echo "${#TESTDIRS[@]} directories detected."

for directory in ${TESTDIRS[@]}; do

	TESTS=( $directory/*.c )
	echo "${#TESTS[@]} tests found in $directory"
	echo "__DIRECTORY $directory" >> $RESULT

	i=1
	for testf in ${TESTS[@]}; do
		outfile="$OUTDIR/$(basename $testf).out"
		infile="$INDIR/$(basename $testf)"
		#rm -f $outfile
		rm -f $infile
		cp $testf $infile
		
		echo -n "Running test $i of ${#TESTS[@]} ..."
		
		start=$(date +%s%N)
		cd $NEWTON_DIR
		#eval "timeout $TIMEOUT $NEWTON -cra_newton_basic -cra-forward-inv -cra-split-loops -cra-disable-simplify --test=$RESULT $testf &> $outfile"
        #
        rm -f $outfile
        #COMMAND="$NEWTON -cra_newton_basic -cra-forward-inv -cra-split-loops" # until 2018-03-23
        COMMAND="$NEWTON -cra_newton_basic -cra-split-loops"
        echo $COMMAND >> $outfile
        echo "" >> $outfile
		eval "timeout $TIMEOUT $COMMAND --test=$RESULT $testf &>> $outfile"
        #
		#eval "timeout $TIMEOUT $NEWTON -cra_newton_basic -cra-forward-inv -cra-split-loops --test=$RESULT $testf &> $outfile"
		success=$?
		if (($success==124)); then
			echo "__TIMEOUT" >> $RESULT
			echo -ne "\e[31mTimeout\e[0m"
		elif (($success!=0)); then
			echo "__EXCEPTION" >> $RESULT
			echo -ne "\e[31mException\e[0m"
		else
			echo "" >> $RESULT
		fi
		end=$(date +%s%N)
		len=$(expr $end - $start)
		echo "__NTIME $len" >> $RESULT
		echo " Done"
			
		let i+=1
	done
done

echo -n "Generating HTML ... "
cd $SUITE
python $SCRIPT || exit 1
echo "Done. Please see print_hull.html"

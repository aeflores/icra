#!/bin/bash
#
# This script is used to run ICRA on a Java JAR file by passing that file
#   through Grammatech's Inspector SpaceTime (IST) tool.  The current version 
#   of this script assumes that the user has an installation of IST at
#   the version corresponding to the 2018/07/18 E6 STAC VM.

# join_by taken from: https://stackoverflow.com/questions/1527049/join-elements-of-an-array
function join_by { local d=$1; shift; echo -n "$1"; shift; printf "%s" "${@/#/$d}"; }

source /bat0/stac/Code/CSFE_JAVA_API/IST/engagement/IST_settings/config.sh

if [ "$#" -lt 1 ]; then
   echo "USAGE: java_icra.sh [<ICRA parameters>...] <JAR file>"
   exit 0
fi

ARG_JAR=${@: -1}
MORE_ARGS=${@:1:$(($#-1))}
DEFAULT_ARGS="--ICRA_args +bound-all time_cost"

if [[ "$#" -le 1 ]]; then
    echo "Using default ICRA arguments: $DEFAULT_ARGS"
    MORE_ARGS=$DEFAULT_ARGS
else
    MORE_ARGS="--ICRA_args $(join_by " " $MORE_ARGS)"
    echo "Using ICRA argument list: $MORE_ARGS"
fi

JAR=$(basename $ARG_JAR)
EXT=${JAR#*.}
if [ "$EXT" != "jar" ]; then
    echo "Expected file extension of last argument to be 'jar', got '$EXT' instead."
    exit 1
fi
STEM=${JAR%%.jar}

TEMP_DIR=$(mktemp -d)
touch $TEMP_DIR/IJtemp.txt
if [ "x$TEMP_DIR" == "x" ]; then
    exit 1
fi

cp $ARG_JAR $TEMP_DIR
cd $TEMP_DIR

# This should match the config mentioned in step 2 below
SUBPROBLEM_METHOD=2_time

ist_reset $JAR &> /dev/null
ist_set_problem_jar $JAR &> /dev/null
# IST STEP 1
# OLD: run_IST.py $PROBLEM_JAR --configFile $GRAMMATECH_HOME/IST/engagement/IST_settings/1_cg_construction.properties --run_cgb_only --work_dir $WORK_DIR --use_env_vars || exit 1
echo ">>> IST Step 1 (call graph construction) at $(date +%H:%M:%S)"
run_IST.py $PROBLEM_JAR --configFile $GRAMMATECH_HOME/IST/engagement/IST_settings/1_cg_construction.properties --frontend callgraph || exit 1

ist_set_final_inclusion_file $WORK_DIR/1_cg_construction/final_inclusion_file.txt &> /dev/null || exit 1

# Old step 2
#ist_run_step2 || exit 1

# Replacement step 2
CONFIG=/bat0/stac/Code/IST/engagement/IST_settings/${SUBPROBLEM_METHOD}.properties
#CONFIG_NAME=`basename $CONFIG .properties`
#OUTPUT_DIR=$WORK_DIR/2_IST_subproblems/
#run_IST.py $PROBLEM_JAR --configFile $CONFIG --run_subproblems --work_dir $OUTPUT_DIR --use_env_vars --ist_config "subproblem.limit = 20"
echo ">>> IST Step 2 (sub-problem construction) at $(date +%H:%M:%S)"
run_IST.py $PROBLEM_JAR --configFile $CONFIG --frontend analysis || exit 1

# Old step 4 (now skipped)
#run_IST.py $PROBLEM_JAR --configFile $GRAMMATECH_HOME/IST/engagement/IST_settings/${SUBPROBLEM_METHOD}.properties --run_subproblems --work_dir $WORK_DIR  --use_env_vars || exit 1

#NEW_SUBPROBLEM=$TEMP_DIR/$STEM/${SUBPROBLEM_METHOD}/subproblem0_0.csv
#ist_set_subproblem_file $TEMP_DIR/$STEM/${SUBPROBLEM_METHOD}/subproblem0_0.csv
#rm $NEW_SUBPROBLEM

# Create a new subproblem file containing all methods from the subproblem other than main
NEW_SUBPROBLEM=$TEMP_DIR/$STEM/${SUBPROBLEM_METHOD}/subproblem_new.csv
echo "CLASS_LOADER,PACKAGE,CLASS,METHOD,CONTEXT,IS_ENTRYPOINT" > $NEW_SUBPROBLEM
FOUND=0
while read CSV_LINE; do
      CSV_LINE=${CSV_LINE/$'\r'}
      if [[ ($CSV_LINE == *$STEM* ) && ( $CSV_LINE != *"<init>"* ) && ( $CSV_LINE != *",main("* ) ]]; then
          if [[ $CSV_LINE == *",entryPoint("* ]]; then
              TRUE_LINE=${CSV_LINE/%,false/,true}
              echo "$TRUE_LINE" >> $NEW_SUBPROBLEM
              FOUND=1
          else
              echo "$CSV_LINE" >> $NEW_SUBPROBLEM
          fi
      fi
done < $TEMP_DIR/$STEM/${SUBPROBLEM_METHOD}/log_subproblems/full_subproblem.csv
echo "# end of subproblem" >> $NEW_SUBPROBLEM
if [ $FOUND -eq 0 ]; then
    echo "java_icra.sh ERROR: Could not find an entryPoint method in the target JAR"
    exit 1
fi

ist_set_subproblem_file $NEW_SUBPROBLEM

# run ICRA
#run_IST.py $PROBLEM_JAR --configFile $GRAMMATECH_HOME/IST/engagement/IST_settings/5_conversion.properties --work_dir `dirname $SUBPROBLEM_FILE` --subproblems_dir `dirname $SUBPROBLEM_FILE` --run_conversion --subproblem_files $SUBPROBLEM_FILE --run_ocaml_conversion --run_ICRA --use_env_vars --ICRA_args +bound-all time_cost
# Old step 5:
#run_IST.py $PROBLEM_JAR --configFile $GRAMMATECH_HOME/IST/engagement/IST_settings/5_conversion.properties --work_dir `dirname $SUBPROBLEM_FILE` --subproblems_dir `dirname $SUBPROBLEM_FILE` --run_conversion --subproblem_files $SUBPROBLEM_FILE --run_ocaml_conversion --run_ICRA --use_env_vars $MORE_ARGS |& tee "$TEMP_DIR/final_ist_step_output.txt"
# New ICRA step (which is now called IST step 3)
echo ">>> IST Step 3 (resource bound analysis using ICRA) at $(date +%H:%M:%S)"
run_IST.py $PROBLEM_JAR --configFile $GRAMMATECH_HOME/IST/engagement/IST_settings/5_conversion.properties --backend conversion ocaml_conversion icra --manual_subproblems $SUBPROBLEM_FILE $MORE_ARGS |& tee "$TEMP_DIR/final_ist_step_output.txt"


while read OUTPUT_LINE; do
      if [[ ( $OUTPUT_LINE == *"ICRA output is"* ) ]]; then
          ICRA_OUTPUT_FILE=${OUTPUT_LINE:22}
          echo "Recapitulating ICRA output from: $ICRA_OUTPUT_FILE"
          echo "-------------------------------------------------------------------"
          cat "$ICRA_OUTPUT_FILE"
          break
      fi
      #else
      #    #echo "Non-matching line: $OUTPUT_LINE"
      #fi
done < "$TEMP_DIR/final_ist_step_output.txt"
echo ">>> ICRA run complete at $(date +%H:%M:%S)"

#ist_help

if [ -f "$TEMP_DIR/IJtemp.txt" ]; then
    echo "Not yet deleting $TEMP_DIR"
    #rm -rf $TEMP_DIR
fi



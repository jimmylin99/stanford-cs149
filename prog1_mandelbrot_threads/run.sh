#!/bin/bash

# --- GOSSIP
# This is a good example of automatic
# text-based and file-based workflow
# using pure shell script
#
# I have learnt some of techniques
# from GPT-based AI, which is helpful
# I believe a combination of
#   prior knowledge in shell script
#   my well-documented examples (like this)
#   GPT-based AI
# will be a good boost for similar work
# --- GOSSIP END

# --- USAGE
# To run this shell script,
# we can get an array of data
# which represents the speedup scale
# with different number of threads.
#
# The result will be stored in a file also
# which can be used by another python jupyter
# to (hopefully) elegantly display the result
#
# Usage: ./run.sh -t 4
# for more options, see codes below
# --- USAGE END

max_thread=8
single_trial=false
file_name="result.txt"

# Parse command-line options
# the colon means that the option will
# have a parameter followed,
# which stored in $OPTARG
while getopts ":t:f:sm" opt; do
  case $opt in
    t) max_thread="$OPTARG";;
    f) file_name="$OPTARG";;
    s) single_trial=true;;
    m) make;;
    \?) echo "Invalid option: -$OPTARG" >&2;;
  esac
done

# This function calculates the execution time of mandelbrot program
# with exactly N threads
# Usage: cal_time <thread-num-N>
cal_time () {
  # the arguments of a function in shell is
  # merely these positional arguments like $1
  mandelbrot_output=$(./mandelbrot -t "$1")
  number=$(echo "$mandelbrot_output" | awk 'END {print $1}' | sed 's/[()x]//g')
  # to return a string from a function
  # just use echo
  # this is because the return keyword
  # can only take effect on numeric values
  echo "$number"
}

# only run with the specified thread num
# and only displayed in console
# used for debugging or demo
if [ $single_trial = true ]; then
  output=$(cal_time "$max_thread")
  echo "$i" "$output"
  exit 0
fi

# execute the program with different threads
# and store the results to a file
# ---
# clean the file first
# by emptying its size to 0 (this is just one way out of many)
truncate -s 0 "$file_name"
# do the loop with different threads
for i in $(seq 1 "$max_thread")
do
    output=$(cal_time "$i")
    # use tee to both write to standard output and files
    # use `tldr tee` to see more
    command="echo $i $output | tee -a $file_name"
    # eval is a good companion to a string-based command
    eval "$command"
done
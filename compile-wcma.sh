#!/bin/sh

# todo:
# -help function
# -write soething for #sthg (give it as argument for g++ as concatenated string)

# A POSIX variable
OPTIND=1         # Reset in case getopts has been used previously in the shell.

# Initialize our own variables:
output_file=""
input_file=""

while getopts "h?oi:" opt; do
    case "$opt" in
    h|\?)
        #show_help
        exit 0
        ;;
    o)  output_file=$OPTARG
        ;;
    i)  input_file=$OPTARG
        ;;

	*) #sthg
		;;
    esac
done

shift $((OPTIND-1))

[ "$1" = "--" ] && shift

g++ -fopenmp -std=gnu++11 -I/usr/include/eigen3/ -I/usr/local/include/libcmaes/ -L/usr/local/lib/ -o  $output_file $input_file -lcmaes


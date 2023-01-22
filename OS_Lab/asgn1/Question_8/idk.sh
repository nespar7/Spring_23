#!/bin/bash

# Assign the filename to a variable
file="main.csv"

# Create the file if it doesn't exist
touch $file

# Check if there are any flags passed in
while getopts ":c:n:s:h" opt; do
  case $opt in
    c)
      category=$OPTARG
      # Get the total amount spent in the given category
      total=$(cat $file | grep $category | awk -F "," '{sum+=$3} END {print sum}')
      echo "Total amount spent on $category: $total"
      ;;
    n)
      name=$OPTARG
      # Get the total amount spent by the given name
      total=$(cat $file | grep $name | awk -F "," '{sum+=$3} END {print sum}')
      echo "Total amount spent by $name: $total"
      ;;
    s)
      column=$OPTARG
      # Sort the file by the given column
      sort -t " " -k $column $file -o $file
      ;;
    h)
      # Show the help prompt
      echo "Expense Tracker"
      echo "Usage: sh Assgn1_8_<groupno>.sh [-c category] [-n name] [-s column] [record]"
      echo "Inserts a new record in main.csv and performs various operations based on the flags passed in."
      echo "Flags:"
      echo "  -c category: Prints the total amount spent in the given category"
      echo "  -n name: Prints the total amount spent by the given name"
      echo "  -s column: Sorts the csv by the given column"
      echo "  -h: Shows this help prompt"
      exit 0
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      exit 1
      ;;
    :)
      echo "Option -$OPTARG requires an argument." >&2
      exit 1
      ;;
  esac
done

# Shift the options to get the record
shift $((OPTIND-1))

# Check if the user passed in a record
if [ $# -eq 0 ]; then
  echo "No record passed in."
else
  # Insert the record into the file
  echo "$@" >> $file
  echo "Inserted $@ in $file"
fi

# By default, sort the file by the date column
sort -t "," -k 1 $file -o $file

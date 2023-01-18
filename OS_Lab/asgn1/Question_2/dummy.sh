#!/bin/bash

# Input file containing usernames
input_file="usernames.txt"

# Output file for validation results
output_file="validation_results.txt"

# Validation function for a single username
validate_username() {
  local username="$1"
  # Add validation rules here, for example:
  if [[ "$username" =~ ^[a-zA-Z0-9._-]{3,}$ ]]; then
    echo "YES"
  else
    echo "NO"
  fi
}

# Read input file and validate each username
while read -r username; do
  validate_username "$username" >> "$output_file"
done < "$input_file"

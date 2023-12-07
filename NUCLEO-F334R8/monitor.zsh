#!/bin/zsh

previous_ts=0

filter_and_save() {
  filtered_line=$(echo "$1" | tr -dc '[:print:]\n') # remove non-printable characters
  timestamp=$(date +"[%Y-%m-%d %H:%M:%S]")
  current_ts=$(perl -MTime::HiRes=time -e 'printf "%.6f\n", time')
  if [[ $previous_ts != 0 ]]; then
    diff=$(printf " (%06.3f)" $((current_ts - previous_ts)))
  fi
  previous_ts=$current_ts
  filtered_line="$timestamp$diff $filtered_line"

  echo "$filtered_line" >> debug.processed.log
  echo "$filtered_line"
}

# ctrl+c to delete debug.log
trap 'rm debug.log; exit' INT

rm -f debug.processed.log

while IFS= read -r line; do
  filter_and_save "$line"
done

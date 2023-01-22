input_dir=$1
output_dir=$2

[ -d "$output_dir" ] || mkdir $output_dir

for file in $input_dir/*; do
    IFS=$'\n'
    for line in $(cat $file); do
        first_letter=${line:0:1}
        echo $line >> $output_dir/$first_letter.txt
    done
done

for file in $output_dir/*; do
  sort $file -o $file
done

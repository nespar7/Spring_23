[ -d "$2" ] || mkdir $2

for file in $1/*; do
    IFS=$'\n'
    for line in $(cat $file); do
        first_letter=${line:0:1}
        echo $line >> $2/$first_letter.txt
    done < $file
done

for file in $2/*; do
  sort $file -o $file
done

grep -ivf fruits.txt usernames.txt|egrep -v '^[^A-Za-z]|^.{0,4}$|^.{21,}$|[^A-Za-z0-9]|^[^0-9]*$' > validation_results.txt 

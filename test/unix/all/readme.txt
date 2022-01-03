# DB env set
source ./start.sh
 or
eval `cat ./start.sh`

# execute of test
./farert > log.txt
or 
./farert | tee log.txt

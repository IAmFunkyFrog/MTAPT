MEMORY_SIZE=32

make

echo "Run experiment with $MEMORY_SIZE GB loading into RAM:"

bash ./top_info.sh &
PID1=$!
./build/experiment -v -p 1 -s $MEMORY_SIZE &
PID2=$!

wait $PID2
kill -9 $PID1

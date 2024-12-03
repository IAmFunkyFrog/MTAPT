while [ 1 ]
do
    echo "[log] Top output"
    top -bn1 | grep MiB
    sleep 2
done

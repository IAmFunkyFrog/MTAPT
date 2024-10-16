function run_with_args() {
    local flush_mode=$1
    local need_endline=$2
    local in_file=$3
    local endline

    if [ "$need_endline" = "YES" ]; then
        endline=-e
    fi
    local experiment_name="flush_mode_${flush_mode}_endline_${need_endline}_in_file_${in_file}"

    echo "Experiment $experiment_name..."
    if [ "$in_file" = "YES" ]; then
        local filename="experiment_${experiment_name}"
        build/experiment -f $flush_mode $endline &> $EXPERIMENT_DIR/$filename
        echo "Done"
        return
    fi

    build/experiment -f $flush_mode $endline
    echo ""
    echo "Done"
}

if [ -z $EXPERIMENT_DIR]; then
    EXPERIMENT_DIR=experiment
fi

mkdir -p $EXPERIMENT_DIR
make

echo "Run experiment:"

for in_file in NO YES; do
    for flush_mode in NO EACH_PRINT BATCH; do
        for need_endline in NO YES; do
            run_with_args $flush_mode $need_endline $in_file
            echo ""
        done
    done
done
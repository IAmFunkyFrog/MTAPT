# Memory_Overcommit

### Запуск
Для запуска эксперимента достаточно запустить скрипт run_experiment.sh
```
bash ./run_experiment.sh
```

Также можно отдельно собрать утилиту для запуска эксперимента с помощью make
```
make
```

И далее можно будет запустить утилиту с помощью команды
```
./build/experiment
```

### Эксперимент
Был проведен следующий эксперимент: попробовать выделить с помощью malloc 32 гигабайта, и если выделиться, попробовать записать в эту память некоторые значения.

##### Операционная система
Эксперимент проводился на WSL с дистрибутивом Ubuntu
```
$ uname -a
Linux HOME-PC 5.10.16.3-microsoft-standard-WSL2 #1 SMP Fri Apr 2 22:23:49 UTC 2021 x86_64 x86_64 x86_64 GNU/Linux
```

Колиество доступной памяти на компьютере:
```
$ top -bn1 | grep MiB
MiB Mem :   7563.2 total,   6992.6 free,    447.0 used,    123.5 buff/cache
MiB Swap:   2048.0 total,   1624.6 free,    423.4 used.   6926.9 avail Mem 
```

##### Результат
Получились следующие результаты:
1. Если оверкоммит памяти отключен (`echo 0 > /proc/sys/vm/overcommit_memory`), то память даже не выделяется и malloc возвращает `null`

2. Если оверкоммит включен (`echo 1 > /proc/sys/vm/overcommit_memory`), то память выделяется, однако в тот момент, когда память заканчивается, операционная система убивает программу

```
Run experiment with 32 GB loading into RAM:
Loaded pages: 0 GB / 32 GB
[log] Top output
MiB Mem :   7563.2 total,   6915.3 free,    529.2 used,    118.6 buff/cache
MiB Swap:   2048.0 total,   1638.8 free,    409.2 used.   6847.1 avail Mem 
Loaded pages: 1 GB / 32 GB
[log] Top output
MiB Mem :   7563.2 total,   4974.5 free,   2470.1 used,    118.7 buff/cache
MiB Swap:   2048.0 total,   1639.0 free,    409.0 used.   4906.2 avail Mem 
Loaded pages: 2 GB / 32 GB
Loaded pages: 3 GB / 32 GB
[log] Top output
MiB Mem :   7563.2 total,   3836.0 free,   3608.5 used,    118.7 buff/cache
MiB Swap:   2048.0 total,   1639.0 free,    409.0 used.   3767.8 avail Mem 
Loaded pages: 4 GB / 32 GB
Loaded pages: 5 GB / 32 GB
[log] Top output
MiB Mem :   7563.2 total,   1781.9 free,   5662.6 used,    118.7 buff/cache
MiB Swap:   2048.0 total,   1639.0 free,    409.0 used.   1713.7 avail Mem 
Loaded pages: 6 GB / 32 GB
Loaded pages: 7 GB / 32 GB
[log] Top output
MiB Mem :   7563.2 total,    109.9 free,   7347.7 used,    105.6 buff/cache
MiB Swap:   2048.0 total,   1222.9 free,    825.1 used.     35.1 avail Mem 
[log] Top output
./run_experiment.sh: line 12: 23221 Killed                  ./build/experiment -v -p 1 -s $MEMORY_SIZE
```
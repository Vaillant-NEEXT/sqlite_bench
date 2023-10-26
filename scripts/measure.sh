#!/bin/sh

topp() (
    if [ -n "$O" ]; then
        $* &
    else
        $* &>/dev/null &
    fi
    pid="$!"
    trap exit SIGINT
    i=1
    top -b n 1 -d "${T:-0.1}" -n1 -p "$pid"

    while true; do
        top -b n 1 -d "${T:-0.1}" -n1 -p "$pid"  | tail -1
        echo "$i ----------------------------------------------"
        i=$(($i + 1))
    done
)

T=0.2 O=1 topp ./sqlite_bench_EXE "-p /home/sspan/dev/temp/sqlite_bench/db"
#T=0.1 O=1 topp ./sqlite_bench_EXE "-p /data -q 1 -f 1698050051 -t 1700000000"
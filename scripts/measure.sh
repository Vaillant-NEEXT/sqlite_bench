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
    top -b n1 -d "${T:-0.1}" -n1 -p "$pid"

    while true; do
        top -b n1 -d "${T:-0.1}" -n1 -p "$pid"  | tail -1
        printf "$i "
        i=$(($i + 1))
    done
)

#T=0.2 O=1 topp ./app/app "-p /home/sspan/dev/temp/sqlite_bench/db"
T=0.1 O=1 topp ./app "-p /data"
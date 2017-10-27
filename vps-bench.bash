#!/bin/bash
set -e
set -x

JOBS="$1"

time git clone --depth 1 --branch v4.10 "git://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git"
(cd linux
make defconfig
time make -j"$JOBS"
)

time wget "https://www.sqlite.org/2017/sqlite-amalgamation-3210000.zip"
time unzip sqlite-*.zip

time gcc -O2 index.c sqlite/sqlite3.c -Isqlite -DSQLITE_ENABLE_FTS5 -o index
time ./index database.sqlite linux

time gcc -O2 search.c sqlite/sqlite3.c -Isqlite -DSQLITE_ENABLE_FTS5 -o search
time ./search database.sqlite frobnicate
time ./search database.sqlite static

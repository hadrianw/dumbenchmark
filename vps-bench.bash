#!/bin/bash
set -e
set -x

JOBS="$1"

time git clone --depth 1 --branch v4.10 "git://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git"
(cd linux
make defconfig
time make -j"$JOBS"
)

wget "https://www.sqlite.org/2017/sqlite-amalgamation-3210000.zip"
unzip sqlite-*.zip

time gcc -O2 -Wall -Wextra index.c sqlite-*/sqlite3.c -I sqlite-*/ -DSQLITE_ENABLE_FTS5 -DSQLITE_OMIT_LOAD_EXTENSION -lpthread -lm -o index
time ./index database.sqlite linux

time gcc -O2 -Wall -Wextra search.c sqlite-*/sqlite3.c -I sqlite-*/ -DSQLITE_ENABLE_FTS5 -DSQLITE_OMIT_LOAD_EXTENSION -lpthread -lm -o search
time ./search database.sqlite frobnicate
time ./search database.sqlite static

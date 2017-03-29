#!/bin/sh
for i in `seq 1 9`
do
    tmp=`expr $i \* 100000 ` 
    echo $tmp
    time ./test $tmp 0 >> construct_dns_log
    echo "\n">>construct_dns_log
done
for j in `seq 1 10`
do
    tmp=`expr $i \* 1000000 ` 
    time ./test $tmp 0 >> construct_dns_log
    echo "\n">>construct_dns_log
done




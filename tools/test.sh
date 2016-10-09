#!/bin/sh

i=0

while [ $i != 100 ]
do
	./a.out 
	grep -v "[[:space:]]0$" bb_counter.txt > bb1.txt
	./a.out 
	grep -v "[[:space:]]0$" bb_counter.txt > bb2.txt

	diff bb1.txt bb2.txt

	echo $i

	i=`expr $i + 1`
done

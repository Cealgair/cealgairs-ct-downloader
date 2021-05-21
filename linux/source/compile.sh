#!/bin/bash
for i in $(ls | grep .c)
do
	echo "Compiling $i..."
	gcc $i -o ../${i%.c} -Wall --pedantic
done
for i in $(ls bin| grep .c)
do
	echo "Compiling bin/$i..."
	gcc bin/$i -o ../bin/${i%.c} -Wall --pedantic
done

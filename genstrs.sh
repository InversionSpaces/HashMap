#!/bin/bash

for i in {1..50000}
do
	str="$(head /dev/urandom | tr -dc A-Za-z0-9 | head -c $(($RANDOM % 100 + 5)))"
	echo "$str" >> $1
done


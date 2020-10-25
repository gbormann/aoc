#!/usr/bin/bash

touch img25x6.bmp;
truncate -s 0 img25x6.bmp;

for i in $(seq 0 99);
do
	byte_range="$(( 150 * i + 1 ))-$(( 150 * (i + 1) ))";
	cut -b $byte_range -n ../img25x6.sif|sed -r "s/([0-2])/\1\n/g" > col.bmp;
	paste -d"," img25x6.bmp col.bmp > tmp.bmp;
	mv tmp.bmp img25x6.bmp;
done

tr -d "2" < img25x6.bmp|tr -d ","|cut -b 1-1 -n|tr -d "\n"|tr "0" " "|tr "1" "*" > tmp.bmp;

truncate -s 0 img25x6.bmp;
for i in $(seq 0 5);
do
	byte_range="$(( 25 * i + 1 ))-$(( 25 * (i + 1) ))";
	cut -b $byte_range -n tmp.bmp >> img25x6.bmp;
done

rm -f col.bmp tmp.bmp;

cat img25x6.bmp;

#!/usr/bin/bash

min_nr_zeros=151
for i in $(seq 0 99);
do
	byte_range="$(( 150 * i + 1 ))-$(( 150 * (i + 1) ))";
	cut -b $byte_range -n ../img25x6.sif > tmp.sif;
	nr_zeros=$(tr -d "12" < tmp.sif|tr -d "\n"|wc -c);
	nr_ones=$(tr -d "02" < tmp.sif|tr -d "\n"|wc -c);
	nr_twos=$(tr -d "01" < tmp.sif|tr -d "\n"|wc -c);
	nr_pixels=$((nr_zeros+nr_ones+nr_twos));
	if [[ "$nr_pixels" -ne 150 ]];
	then
		echo "Line not exactly 150!: $nr_pixels";
		echo "    at byte range $byte_range";
		echo "    #0: $nr_zeros";
		echo "    #1: $nr_ones";
		echo "    #2: $nr_twos";
		cat tmp.sif;
		exit 1;
	fi
	if [[ "$nr_zeros" -lt "$min_nr_zeros" ]];
	then
		byte_range_min=$byte_range;
		nr_ones_min=$nr_ones;
		nr_twos_min=$nr_twos;
		min_nr_zeros=$nr_zeros;
	fi
done

echo "Byte range with minimal number of zeros ($min_nr_zeros): $byte_range_min";
echo "    checksum=$((nr_ones_min * $nr_twos_min))";

rm -f tmp.sif;

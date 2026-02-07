#!/bin/bash

cache_file=~/.cache/Qtractor/qtractor_vst2_scan.cache

if [[ ! -e $cache_file ]]; then
	echo "$cache_file does not exists" >&2
	exit 1
fi

for P in ~/.vst/*; do
	if grep -qw $P $cache_file; then
		echo $P' - already exists'
		continue
	fi

	echo $P' - adding to cache'
	L=$(/usr/lib/qtractor/qtractor_plugin_scan <<< "VST2:$P" 2>/dev/null | grep ^VST2)
	echo $L >> $cache_file
done

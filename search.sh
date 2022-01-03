#!/bin/bash

search="$1";
result=$(grep -IlR "${search}" ./Source/; find ./Source/ -name "*${search}*");

if [[ "$2" == "--edit" ]]; then
	files=$(echo $result | sort -u);
	vim $files;
elif [[ "$2" == "--verbose" ]]; then
	result=$(grep -IR "${search}" ./Source/; find ./Source/ -name "*${search}*");
	echo "$result" | sort -u;
else
	echo "$result" | sort -u;
fi;


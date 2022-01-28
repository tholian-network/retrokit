#!/bin/bash

BASE_COMMIT="fbd7f1a910";


if [[ -d "$1" ]] || [[ -f "$1" ]]; then

	git diff "$BASE_COMMIT" "$1";

else
	echo "Please provide a file or folder path.";
	exit 1;
fi;


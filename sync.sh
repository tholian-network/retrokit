#!/bin/bash

CURL_BIN=`which curl`;
BASE_PATH="https://raw.githubusercontent.com/WebKit/WebKit/HEAD";
FILE_PATH="$1";



sync_path() {

	# File doesn't exist. If it's a valid filename, touch it.
	if [[ ! -e "$1" ]]; then

		base="$(basename $1)";
		if [[ "$base" == *"."* ]]; then
			touch "$1";
		fi;

	fi;

	if [[ -f "$1" ]]; then

		path="$1";
		folder="$(dirname $path)";

		if [[ ! -d "$folder" ]]; then
			mkdir -p "$folder";
		fi;

		if [[ -d "$folder" ]]; then

			curl --fail --progress-bar "$BASE_PATH/$path" --output "$path";

			if [[ "$?" == "22" ]]; then
				echo "> Removing $path";
				rm "$path";
			fi;

		fi;

	fi;

}



if [[ "$CURL_BIN" != "" ]]; then

	for path in "$@";
	do
		echo "> Synchronizing $path";

		sync_path "$path";

	done;

else
	echo "Please install cURL.";
	exit 1;
fi;


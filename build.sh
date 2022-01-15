#!/bin/bash

cmake -DPORT=GTK -DCMAKE_BUILD_TYPE=Release -DENABLE_MINIBROWSER=ON -GNinja;
exit_code="$?";

if [[ "$exit_code" == "0" ]]; then
	ninja;
fi;


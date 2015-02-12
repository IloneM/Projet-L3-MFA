#!/bin/bash

if [[ ! $LD_LIBRARY_PATH == *"/usr/local/lib/"* ]]
then
	export LD_LIBRARY_PATH=/usr/local/lib/:$LD_LIBRARY_PATH
fi

`$1`


#!/bin/sh

if [ ! -d "datas" ]; then
	mkdir datas
fi

yaourt -S eigen jsoncpp


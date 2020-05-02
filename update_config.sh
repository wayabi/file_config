#!/bin/sh

while [ 1 ]:
do
	curl 127.0.0.1/config > config0
	mv config0 config
done

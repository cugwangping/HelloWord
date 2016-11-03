#!/bin/bash

network_login()
{
	result=1
	count=0	
	while(($count < 3))
	do
		echo "call wifi-connection:"
		. ./wifi-connection.sh
		#echo $?
		if [ $? -eq 0 ]
		then
			let result=0
			break
		else
			let count=count+1
		fi
	done
	return $result
}

network_login
#echo $?
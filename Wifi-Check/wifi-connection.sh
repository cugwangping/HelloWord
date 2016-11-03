#!/bin/bash

wifi_connection()
{
	# Which Interface do you want to check/fix
	wlan='wlan0'
	##################################################################
	echo
	echo "Starting WiFi check for $wlan"
	date
	echo
	# We can perform check
	echo "Performing Network check for $wlan"
	restart_up_limit=5
	i=0
	result=1
	while (($i < $restart_up_limit))
	do
		if ifconfig $wlan | grep "inet addr:" ; then
		    echo "Network is Okay"
		    device_name=`hostname`
		    curl -H "User-Agent: WiFi_Check" "http://202.114.201.114/ip_report.php?device=$device_name"
		    # ping_cug_edu_cn=`ping -c 3 58.48.110.155|awk 'NR==7 {print $4}'`
		    ping -q -c5 58.48.110.155 > /dev/null
		    if [ $? -eq 0 ]
		    then
			echo "Network logged in"
			let result=0
			break
		    else
			echo "Network not logged in"
			echo "Wireless connection attempt..."
			((i++))
			# Ö»ÐèÒªÎÞÏßÍøµÄµØÖ·
			ipaddress=`ifconfig -a|grep inet|grep -v 127.0.0.1|grep -v inet6|awk '{print $2}'|tr -d "addr:"`
			curl "http://202.114.205.115/lanmanwxapp/network_login.php?username=wyc&ip_address=$ipaddress&channel_id=Channel3&login_type=normal&frequently=0"
			let result=0
			break
		    fi
		else
		    echo "Network connection down! Attempting reconnection."
		    ((i++))
		    ifdown $wlan
		    sleep 5
		    ifup $wlan
		    #ifconfig $wlan | grep "inet addr"
		fi
	done
	return $result
}

wifi_connection
#echo $?

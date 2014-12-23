#!/bin/bash
/etc/init.d/networking restart
ifconfig eth0 down
ifconfig eth1 down

ifconfig eth0 up
ifconfig eth0 172.16.50.254/24
ifconfig eth1 up
ifconfig eth1 172.16.51.253/24

echo 0 > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts
echo 1 > /proc/sys/net/ipv4/ip_forward

route add default gw 172.16.51.254

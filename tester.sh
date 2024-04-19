#!/bin/bash

PORTA=9776
let I=00
LOG="tests/log"

mkdir -p tests

# test GET
curl -i -X GET localhost:"$PORTA"/ > $LOG"$I"
curl -X GET localhost:"$PORTA"/ > $LOG"$I".content
let I++
curl -i -X GET localhost:"$PORTA"/webserv > $LOG"$I"
curl -X GET localhost:"$PORTA"/webserv > $LOG"$I".content
let I++
curl -i -X GET localhost:"$PORTA"/webserv/assets > $LOG"$I"
curl -X GET localhost:"$PORTA"/ > $LOG"$I".content
let I++
curl -i -X GET localhost:"$PORTA"/htmls/ > $LOG"$I"
curl -X GET localhost:"$PORTA"/ > $LOG"$I".content
let I++

/usr/bin/echo -n -e "\033[31mContinue tests? (y/n)\033[0m"
read -n 1 -r
if [[ ! $REPLY =~ ^[Yy]$ ]]
then
	exit 0
fi

# test POST
curl -i -X POST -H "Content-Type: text/plain" -d "testing $I" localhost:"$PORTA"/TEST_FILE.txt > $LOG"$I"
curl -X POST -H "Content-Type: text/plain" -d "testing $I" localhost:"$PORTA"/TEST_FILE.txt > $LOG"$I".content
let I++
curl -i -X POST -H "Content-Type: text/plain" -d "testing $I" localhost:"$PORTA"/webserv/TEST_FILE.txt > $LOG"$I"
curl -X POST -H "Content-Type: text/plain" -d "testing $I" localhost:"$PORTA"/webserv/TEST_FILE.txt > $LOG"$I".content
let I++
curl -i -X POST -H "Content-Type: text/plain" -d "testing $I" localhost:"$PORTA"/webserv/assets/TEST_FILE.txt > $LOG"$I"
curl -X POST -H "Content-Type: text/plain" -d "testing $I" localhost:"$PORTA"/webserv/assets/TEST_FILE.txt > $LOG"$I".content
let I++
curl -i -X POST -H "Content-Type: text/plain" -d "testing $I" localhost:"$PORTA"/htmls/TEST_FILE.txt > $LOG"$I"
curl -X POST -H "Content-Type: text/plain" -d "testing $I" localhost:"$PORTA"/htmls/TEST_FILE.txt > $LOG"$I".content
let I++

/usr/bin/echo -e -n "\033[31mWARNING: are you sure you want to test delete? (y/n)\033[0m"
read -n 1 -r
if [[ ! $REPLY =~ ^[Yy]$ ]]
then
	exit 0
fi

curl -i -X DELETE localhost:"$PORTA"/TEST_FILE.txt > $LOG"$I"
curl -X POST -H "Content-Type: text/plain" -d "testing $I" localhost:"$PORTA"/TEST_FILE.txt > /dev/null
curl -X DELETE localhost:"$PORTA"/TEST_FILE.txt > $LOG"$I".content
let I++
curl -i -X DELETE localhost:"$PORTA"/webserv/TEST_FILE.txt > $LOG"$I"
curl -X POST -H "Content-Type: text/plain" -d "testing $I" localhost:"$PORTA"/webserv/TEST_FILE.txt > /dev/null
curl -X DELETE localhost:"$PORTA"/webserv/TEST_FILE.txt > $LOG"$I".content
let I++
curl -i -X DELETE localhost:"$PORTA"/webserv/assets/TEST_FILE.txt > $LOG"$I"
curl -X POST -H "Content-Type: text/plain" -d "testing $I" localhost:"$PORTA"/webserv/assets/TEST_FILE.txt > /dev/null
curl -X DELETE localhost:"$PORTA"/webserv/assets/TEST_FILE.txt > $LOG"$I".content
let I++
curl -i -X DELETE localhost:"$PORTA"/htmls/TEST_FILE.txt > $LOG"$I"
curl -X POST -H "Content-Type: text/plain" -d "testing $I" localhost:"$PORTA"/htmls/TEST_FILE.txt > /dev/null
curl -X DELETE localhost:"$PORTA"/htmls/TEST_FILE.txt > $LOG"$I".content
let I++

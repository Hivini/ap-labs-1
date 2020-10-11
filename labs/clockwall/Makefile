
include ../../common.mk

server1:
	go run clock2.go -port=8010 -TZ=US/Eastern

server2:
	go run clock2.go -port=8020 -TZ=Asia/Tokyo

server3:
	go run clock2.go -port=8030 -TZ=Europe/London

client:
	go run clockWall.go -NewYork=localhost:8010 -Tokyo=localhost:8020 -London=localhost:8030
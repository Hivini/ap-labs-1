
include ../../common.mk

servers:
	TZ=US/Eastern    go run clock2.go -port 8010 &
	TZ=Asia/Tokyo    go run clock2.go -port 8020 &
	TZ=Europe/London go run clock2.go -port 8030

client:
	go run clockWall.go NewYork=localhost:8010 Tokyo=localhost:8020 London=localhost:8030
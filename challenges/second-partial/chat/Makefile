include ../../../common.mk

server:
	go run server.go -host localhost -port 9000

client1:
	go run client.go -user user1 -server localhost:9000

client2:
	go run client.go -user user2 -server localhost:9000

client3:
	go run client.go -user user3 -server localhost:9000
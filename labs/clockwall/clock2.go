// Clock2 is a concurrent TCP server that periodically writes the time.
package main

import (
	"flag"
	"fmt"
	"io"
	"log"
	"net"
	"time"
)

func handleConn(c net.Conn) {
	defer c.Close()
	for {
		locT := time.Now().Local()
		_, err := io.WriteString(c, fmt.Sprintf("%s\n", locT.Format("15:04:05")))
		if err != nil {
			return // e.g., client disconnected
		}
		time.Sleep(1 * time.Second)
	}
}

func main() {
	port := flag.Int("port", 9090, "Port where the server will run locally")
	flag.Parse()

	// Only allow ports between 1024 and 49152, set to default otherwise.
	if *port <= 1024 || *port >= 49152 {
		fmt.Printf("Invalid port, reassigned to port 9090...\n")
		*port = 9090
	}

	fmt.Printf("Server running on localhost:%d\n", *port)
	listener, err := net.Listen("tcp", fmt.Sprintf("localhost:%d", *port))
	if err != nil {
		log.Fatal(err)
	}
	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Print(err) // e.g., connection aborted
			continue
		}
		go handleConn(conn)
	}
}

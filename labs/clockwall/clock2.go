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

func locationTime(t time.Time, locName string) time.Time {
	loc, err := time.LoadLocation(locName)
	if err != nil {
		// Recall the same function but with a valid default location
		log.Fatal(err)
		return locationTime(t, "UTC")
	}
	return t.In(loc)
}

func handleConn(c net.Conn, tz string) {
	defer c.Close()
	for {
		locT := locationTime(time.Now(), tz)
		_, err := io.WriteString(c, fmt.Sprintf("%s %s\n", locT.Location(), locT.Format("15:04:05")))
		if err != nil {
			return // e.g., client disconnected
		}
		time.Sleep(1 * time.Second)
	}
}

func main() {
	port := flag.Int("port", 9090, "Port where the server will run locally")
	tz := flag.String("TZ", "", "Timezone where the clock will be running")
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
		go handleConn(conn, *tz) // handle connections concurrently
	}
}

// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 227.

// Netcat is a simple read/write client for TCP servers.
package main

import (
	"bufio"
	"flag"
	"fmt"
	"io"
	"log"
	"net"
	"os"
)

//!+
func main() {
	user := flag.String("user", "", "Identifier for the server")
	address := flag.String("server", "", "Address of the IRC server")
	flag.Parse()

	if *user == "" {
		log.Fatalf("Username must be specified with -user flag\n")
	}

	if *address == "" {
		log.Fatalf("Server address must be specified with -server flag\n")
	}

	conn, err := net.Dial("tcp", *address)
	if err != nil {
		log.Fatal(err)
	}

	// Send the username to the IRC server
	fmt.Fprintln(conn, *user)

	done := make(chan struct{})
	reader := bufio.NewReader(os.Stdin)
	loop := true
	go func() {
		buf := make([]byte, 1024)
		for {
			n, err := conn.Read(buf)
			if err != nil {
				if err != io.EOF {
					fmt.Println("read error:", err)
				}
				break
			}
			if n > 0 {
				fmt.Printf("\r%s%s > ", string(buf[:n]), *user)
			}
		}
		fmt.Printf("\r")
		log.Println("Connection to channel closed")
		loop = false
		done <- struct{}{} // signal the main goroutine
	}()
	mustCopy(conn, reader, *user, &loop)
	conn.Close()
	<-done // wait for background goroutine to finish
}

//!-

func mustCopy(conn io.Writer, reader *bufio.Reader, user string, loop *bool) {
	for *loop {
		fmt.Printf("%s > ", user)
		r, _ := reader.ReadString('\n')
		_, err := fmt.Fprint(conn, r)
		if err != nil {
			break
		}
	}
}

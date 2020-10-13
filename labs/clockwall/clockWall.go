package main

import (
	"fmt"
	"io"
	"log"
	"net"
	"os"
	"strings"
	"sync"
)

type _TimeServer struct {
	city string
	ip   string
}

func main() {
	args := os.Args[1:]
	if len(args) < 1 {
		log.Fatalln("No arguments provided, e.g. usage: go run clockWall.go Tokyo=localhost:8080")
	}

	var servers []*_TimeServer
	for i := range args {
		srv, err := processArg(args[i])
		if err != nil {
			fmt.Printf("%s, ignored.\n", err)
			continue
		}
		servers = append(servers, srv)
	}

	if len(servers) < 1 {
		log.Fatalln("No valid arguments where provided for the servers. e.g. argument: Tokyo=localhost:8080")
	}

	var wg sync.WaitGroup
	wg.Add(len(servers))

	for _, svr := range servers {
		go doConnection((*svr).ip, (*svr).city, &wg)
	}

	wg.Wait()
}

func processArg(arg string) (*_TimeServer, error) {
	spl := strings.Split(arg, "=")
	if len(spl) < 2 {
		return nil, fmt.Errorf("Invalid argument found: [%s]", arg)
	}
	return &_TimeServer{spl[0], spl[1]}, nil
}

func doConnection(addr string, city string, wg *sync.WaitGroup) {
	conn1, err := net.Dial("tcp", addr)
	if err != nil {
		fmt.Printf("- IGNORING. Couldn't connect to '%s' with address '%s'\n", city, addr)
		wg.Done()
		return
	}
	defer conn1.Close()
	if err == nil {
		printTime(city, conn1)
	}
	wg.Done()
}

func printTime(city string, src io.Reader) {
	buffer := make([]byte, 256)
	for {
		_, err := src.Read(buffer)
		if err != nil {
			if err != io.EOF {
				fmt.Println("read error:", err)
			}
			break
		}
		fmt.Printf("%s\t: %s", city, buffer)
	}
}

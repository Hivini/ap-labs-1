package main

import (
	"flag"
	"io"
	"log"
	"net"
	"os"
	"sync"
)

func main() {
	newYorkAddr := flag.String("NewYork", "", "Address where the US/Eastern server is running")
	tokyoAddr := flag.String("Tokyo", "", "Address where the Asia/Tokyo server is running")
	londonAddr := flag.String("London", "", "Address where the Europe/London server is running")
	flag.Parse()

	var wg sync.WaitGroup
	wg.Add(3)
	if *newYorkAddr == "" {
		log.Fatalf("No address provided for New York server: %s", *newYorkAddr)
	} else {
		go doConnection(*newYorkAddr, "Couldn't connect to the New York server", &wg)
	}

	if *tokyoAddr == "" {
		log.Fatalf("No address provided for Tokyo server: %s", *tokyoAddr)
	} else {
		go doConnection(*tokyoAddr, "Couldn't connect to the Tokyo server", &wg)
	}

	if *londonAddr == "" {
		log.Fatalf("No address provided for London server: %s", *londonAddr)
	} else {
		go doConnection(*londonAddr, "Couldn't connect to the London server", &wg)
	}

	wg.Wait()
}

func doConnection(addr string, errMsg string, wg *sync.WaitGroup) {
	conn1, err := net.Dial("tcp", addr)
	if err != nil {
		log.Fatalf("Couldn't connect to %s", addr)
	}
	defer conn1.Close()
	if err == nil {
		printTime(os.Stdout, conn1)
	}
	wg.Done()
}

func printTime(dst io.Writer, src io.Reader) {
	if _, err := io.Copy(dst, src); err != nil {
		log.Fatal(err)
	}
}

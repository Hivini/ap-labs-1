// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 254.
//!+

// Chat is a server that lets clients chat with each other.
package main

import (
	"bufio"
	"flag"
	"fmt"
	"log"
	"net"
	"sort"
	"strings"
	"time"
)

//!+broadcaster
type client chan<- string // an outgoing message channel

type message struct {
	user client
	msg  string
}

type userClient struct {
	user   client
	name   string
	joined time.Time
	ip     string
	conn   net.Conn
}

var (
	currentAdmin = userClient{}
	entering     = make(chan userClient)
	leaving      = make(chan userClient)
	messages     = make(chan message) // all incoming client messages
	svrPrefix    = "irc-server > "
	timeLoc      = "America/Mexico_City"
)

func broadcaster(clients *map[userClient]bool) {
	for {
		select {
		case msg := <-messages:
			// Broadcast incoming message to all
			// clients' outgoing message channels.
			for cli := range *clients {
				if cli.user != msg.user {
					cli.user <- msg.msg
				}
			}

		case cli := <-entering:
			(*clients)[cli] = true

		case cli := <-leaving:
			delete(*clients, cli)
			close(cli.user)
		}
	}
}

//!-broadcaster

//!+handleConn
func handleConn(conn net.Conn, clients *map[userClient]bool) {
	var tmpMsg string

	ch := make(chan string) // outgoing client messages
	go clientWriter(conn, ch)

	// Get user (first message)
	input := bufio.NewScanner(conn)
	input.Scan()
	who := input.Text()
	if userExists(who, clients) {
		ch <- fmt.Sprintf("%sUsername already exists, please change.", svrPrefix)
		close(ch)
		conn.Close()
		fmt.Printf("%sDenied login to [%s] with ip [%s], username already exists.\n", svrPrefix, who, conn.RemoteAddr().String())
		return
	}

	// Message user about log in status
	ch <- fmt.Sprintf("%sWelcome to the Simple IRC Server", svrPrefix)
	ch <- fmt.Sprintf("%sYour user [%s] is successfully logged", svrPrefix, who)
	// Broadcast new connection
	tmpMsg = fmt.Sprintf("%sNew connected user [%s]", svrPrefix, who)
	messages <- message{ch, tmpMsg}
	fmt.Println(tmpMsg)

	// Create user
	ip := strings.Split(conn.RemoteAddr().String(), ":")
	usr := userClient{ch, who, time.Now(), ip[0], conn}

	// If no clients are connected, convert the first user as admin.
	if len(*clients) < 1 {
		currentAdmin = usr
		ch <- fmt.Sprintf("%sCongrats, you were the first user.", svrPrefix)
		ch <- fmt.Sprintf("%sYou're the new IRC Server ADMIN", svrPrefix)
		fmt.Printf("%s[%s] was promoted as the channel ADMIN\n", svrPrefix, who)
	}

	// Add to current clients
	entering <- usr

	for input.Scan() {
		words := strings.Split(input.Text(), " ")
		if len(words) > 0 {
			switch words[0] {
			case "/users":
				var msg strings.Builder
				for key := range *clients {
					msg.WriteString(fmt.Sprintf("%s%s - connected since %s\n", svrPrefix, key.name, key.joined.Format("01-02-2006 15:04:05")))
				}
				rs := msg.String()
				ch <- rs[:len(rs)-1]
			case "/msg":
				if len(words) > 2 {
					var found bool
					found = false
					for cl := range *clients {
						if cl.name == words[1] {
							cl.user <- fmt.Sprintf("From %s: %s", who, strings.Join(words[2:], " "))
							found = true
							break
						}
					}
					if !found {
						ch <- fmt.Sprintf("%sUser '%s' not found.", svrPrefix, words[1])
					}
					continue
				}
				ch <- fmt.Sprint("Wrong usage of command. Example: /msg <username> <msg>")
			case "/time":
				locat, err := time.LoadLocation(timeLoc)
				if err != nil {
					panic(err)
				}
				t := time.Now().In(locat)
				ch <- fmt.Sprintf("%sLocal Time: %s %s", svrPrefix, t.Location(), t.Format("15:04"))
			case "/user":
				if len(words) > 1 {
					var found bool
					found = false
					for cl := range *clients {
						if cl.name == words[1] {
							ch <- fmt.Sprintf("%susername: %s, IP: %s Connected since: %s",
								svrPrefix,
								cl.name,
								cl.ip,
								cl.joined.Format("01-02-2006 15:04:05"))
							found = true
							break
						}
					}
					if !found {
						ch <- fmt.Sprintf("%sUser '%s' not found.", svrPrefix, words[1])
					}
					continue
				}
				ch <- fmt.Sprintf("%sWrong usage of command. Example: /user <username>", svrPrefix)
			case "/kick":
				if currentAdmin == usr {
					if len(words) > 1 {
						if usr.name == words[1] {
							ch <- fmt.Sprintf("%sYou cannot kick yourself!", svrPrefix)
							continue
						}
						var found bool
						found = false
						for cl := range *clients {
							if cl.name == words[1] {
								cl.user <- fmt.Sprintf("%sYou're kicked from this channel", svrPrefix)
								cl.user <- fmt.Sprintf("%sBad language is not allowed on this channel", svrPrefix)
								tmpMsg = fmt.Sprintf("%s[%s] was kicked from channel for bad language policy violation",
									svrPrefix,
									cl.name)
								messages <- message{nil, tmpMsg}
								fmt.Println(tmpMsg)
								cl.conn.Close()
								found = true
								break
							}
						}
						if !found {
							ch <- fmt.Sprintf("%sUser '%s' not found.", svrPrefix, words[1])
						}
						continue
					}
					ch <- fmt.Sprintf("%sWrong usage of command. Example: /kick <username>", svrPrefix)
					continue
				}
				ch <- fmt.Sprintf("%sYou do not have permissions to kick users!", svrPrefix)
			default:
				messages <- message{ch, fmt.Sprintf("%s > %s", who, input.Text())}
			}
		}
	}
	// NOTE: ignoring potential errors from input.Err()
	leaving <- usr
	err := conn.Close()
	// If the connection was already closed means that it has been kicked
	if err == nil {
		tmpMsg = fmt.Sprintf("%s[%s] left channel", svrPrefix, who)
		messages <- message{ch, tmpMsg}
		fmt.Println(tmpMsg)
	}

	if currentAdmin == usr {
		currentAdmin = userClient{}
		if len(*clients) > 0 {
			chooseNewAdmin(clients)
		}
	}
}

func userExists(username string, clients *map[userClient]bool) bool {
	for key := range *clients {
		if key.name == username {
			return true
		}
	}
	return false
}

func chooseNewAdmin(clients *map[userClient]bool) {
	v := make([]userClient, 0, len(*clients))
	// Get the current userClients
	for key := range *clients {
		v = append(v, key)
	}
	// Sort them by joined time.
	sort.SliceStable(v, func(i, j int) bool {
		return v[i].joined.Before(v[j].joined)
	})
	// The first one is the new admin
	currentAdmin = v[0]
	v[0].user <- fmt.Sprintf("%sCongrats! You are the new admin.", svrPrefix)
	fmt.Printf("%s[%s] was promoted as the channel ADMIN\n", svrPrefix, v[0].name)
}

func clientWriter(conn net.Conn, ch <-chan string) {
	for msg := range ch {
		fmt.Fprintln(conn, msg) // NOTE: ignoring network errors
	}
}

//!-handleConn

//!+main
func main() {
	address := flag.String("host", "localhost", "Address where the server listen to. Defaults to localhost.")
	port := flag.Int("port", -1, "Port where the server will run on the address.")
	flag.Parse()

	if *port == -1 {
		log.Fatalf("Add -port flag where the server will run.")
	}
	// Only allow ports between 1024 and 49152.
	if *port <= 1024 || *port >= 49152 {
		fmt.Printf("Port must be between 1024 and 49152, reassigned to port 9090...\n")
		*port = 9090
	}

	svrAddr := fmt.Sprintf("%s:%d", *address, *port)
	listener, err := net.Listen("tcp", svrAddr)
	if err != nil {
		log.Fatal(err)
	}
	fmt.Printf("%sSimple IRC Server started at %s\n", svrPrefix, svrAddr)
	fmt.Printf("%sReady for receiving new clients\n", svrPrefix)
	clients := make(map[userClient]bool)
	go broadcaster(&clients)
	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Print(err)
			continue
		}
		go handleConn(conn, &clients)
	}
}

//!-main

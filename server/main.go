package main

import (
	"flag"

	"github.com/praydog/AutomataMP/server/automatamp"
)

func main() {
	mode := flag.String("mode", "server", "server or client")
	flag.Parse()

	if *mode == "server" {
		server := automatamp.CreateServer()
		server.Run()
	} else if *mode == "client" {
		mock := automatamp.CreateMockClient()
		mock.Run()
	}
}

package main

import (
	"flag"

	"github.com/praydog/AutomataMP/server/automatamp"
)

func main() {
	mode := flag.String("mode", "server", "server or client")
	flag.Parse()

	if *mode == "server" {
		automatamp.CreateServer()
	} else if *mode == "masterserver" {
		server := automatamp.CreateMasterServer()
		server.Run()
	} else if *mode == "client" {
		mock := automatamp.CreateMockClient()
		mock.Run()
	}
}

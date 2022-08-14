# AutomataMP Server

## Command line options
* `--mode` - The mode of the server. Can be `server`, `masterserver`, or `client`. Defaults to `server`.

## JSON Configuration
The server's configuration is stored in `./server.json`.

`server.json` contains the following fields:
* `password` - The password required to connect to the server.
* `masterServer` - The address of the master server. Defaults to `https://niermaster.praydog.com`
* `name` - The name of the server. Defaults to `AutomataMP Server`

## Ports
The ports used by the server are:
* `6969` for the server
* `80` for the master server (web server)

## Installation

### Windows

(If Scoop is installed)

* `scoop install go`
* `scoop install gcc`
* `go build -o server.exe main.go`

### Linux

* `sudo apt-get install golang`
* `sudo apt-get install libenet-dev`
* `go build -o server main.go`
* `chmod +x server`

### Docker
Untested.
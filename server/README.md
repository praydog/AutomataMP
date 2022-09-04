# AutomataMP Server

## Command line options
* `-mode` - The mode of the server. Can be `server`, `masterserver`, or `client`. Defaults is `server`.

## JSON Configuration
The server configuration files are `./server.json` and `./masterserver.json`.

`server.json` contains the following fields:
* `password` - The password required to connect to the server. Default empty
* `masterServer` - The address of the master server. Default `http://localhost`
* `name` - The name of the server. Default `AutomataMP Server`
* `port` - Port to host the listen server on. Default `6969`

`masterserver.json`:
* `address` - Address to host the master listen server on. Default `localhost`
* `port` - Port to host the master listen server on. Default `80`

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
WIP - Requires secure hosting (https)
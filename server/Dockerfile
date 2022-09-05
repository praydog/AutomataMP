FROM golang:1.18

EXPOSE 80

WORKDIR /usr/src/app

# pre-copy/cache go.mod for pre-downloading dependencies and only redownloading them in subsequent builds if they change
COPY automatamp ./automatamp
COPY go.mod ./
COPY go.sum ./
COPY main.go ./
COPY masterserver.json ./
COPY server.json ./

RUN apt-get update
RUN apt-get install -y libenet-dev
RUN apt-get install -y pkg-config

RUN go mod download && go mod verify
RUN go build -o server main.go

CMD ["/usr/src/app/server", "-mode", "masterserver"]
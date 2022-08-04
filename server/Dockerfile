FROM golang:1.18

EXPOSE 6969

WORKDIR /usr/src/app

# pre-copy/cache go.mod for pre-downloading dependencies and only redownloading them in subsequent builds if they change
COPY go.mod ./
COPY go.sum ./
COPY server.go ./
COPY server.json ./
COPY nier ./nier

RUN apt-get update
RUN apt-get install -y libenet-dev
RUN apt-get install -y pkg-config

RUN go mod download && go mod verify
RUN go build server.go

CMD ["/usr/src/app/server"]
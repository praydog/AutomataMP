package core

import "github.com/codecat/go-libs/log"

func check(err error) {
	if err != nil {
		panic(err)
	}
}

func handlepanic() {
	if a := recover(); a != nil {
		log.Info("RECOVER", a)
	}
}

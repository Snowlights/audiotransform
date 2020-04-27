package main

import (
	"github.com/Snowlights/audiotransform/processor"
	"net"
	"net/http"
)

func main(){

	addr, router := processor.InitRouter()

	tcpAddr, err := net.ResolveIPAddr("tcp",addr)
	if err != nil{
		panic(err)
		return
	}

	netListen, err := net.Listen(tcpAddr.Network(),tcpAddr.String())
	if err != nil{
		panic(err)
		return
	}

	go func(){
		err := http.Serve(netListen,nil) //handle todo choose your handle
		if err != nil{
			panic(err)
		}
	}()

}

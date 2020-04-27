package processor

import (
	"github.com/julienschmidt/httprouter"
)

func InitRouter() (string,interface{}){
	router := httprouter.New()



	return ":9101",router
}
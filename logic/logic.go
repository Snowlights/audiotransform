package logic
/*
#cgo CFLAGS: -I ../include
#cgo LDFLAGS: -L /home/perfect/audio/audiotransform -ltransform -lmsc -ldl -lrt -lpthread
#include<stdlib.h>
#include<string.h>
 */
import "C"

import (
	"fmt"
	"io/ioutil"
	"os/exec"
	"github.com/facebookgo/runcmd"
	"unsafe"
)

func doLogin() error {
	fun := "logic.doLogin -->"

	C.iat_logout()
	err := C.iat_login()
	if err != 0 {
		return fmt.Errorf("%s iat login failed, errcode: %d", fun, err)
	}


	return nil
}

func TransformToWAV(fpath string) (string,error){
	fun := "logic.TransformToWAV -->"
	outpath := fpath + ".wav"
	cmdSlice := []string{}
	cmdSlice = append(cmdSlice,"-y","-i",fpath,"-b:a","16k","-ar","16000","-ac","1",outpath)
	cmd := exec.Command("ffmpeg",cmdSlice...)

	streams, err := runcmd.Run(cmd)
	if err != nil{
		fmt.Printf("%s trans to wav failed, error: %s, stdout: %s, stderr: %s",
			fun, err, streams.Stdout(),streams.Stderr())
		return "",err
	}
	return outpath,nil
}

func TranslateAudio(fpath string) (string,error){
	fun := "logic.TranslateAudio -->"

	wavFIle, err := TransformToWAV(fpath)
	if err != nil{
		return "",err
	}

	audioBuffer, err := ioutil.ReadFile(wavFIle)
	if err != nil{
		fmt.Printf("%v read file error %v",fun,err)
		return "", err
	}

	session := C.CString("sub = iat, domain = iat, language = zh_ch, accent = mandarin, sample_rate = 16000, result_type = plain, result_encoding = utf8")
	defer C.free(unsafe.Pointer(session))

	var errCode C.int
	errCode = 0
	var length C.int

	buf := C.malloc(C.size_t(4096))
	C.memset(buf,0,4096)
	defer C.free(buf)

	var sid *C.char
	retry := 0;
	for ;retry<2;retry++{
		sid = C.iat_begin_session(session,&errCode)
		if errCode != 0{
			doLogin()
			continue
		}
		break
	}
	defer  C.iat_end_session(sid)

	if errCode != 0{
		return "",fmt.Errorf("begin session failed errcode %d",errCode)
	}

	errCode = C.iat_transform(sid,(*C.char)(unsafe.Pointer(&audioBuffer[0])),(C.int)(len(audioBuffer)),(*C.char)(buf),&length)
	if errCode != 0{
		return "",fmt.Errorf("transform audio failed errcode %d" ,errCode)
	}

	b := C.GoBytes(unsafe.Pointer(buf),length)

	return string(b[:]),nil
}

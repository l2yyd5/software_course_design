package main

import (
	"./src/api32"
	"./src/dataProcessing"
	"./src/router"
	"fmt"
	"github.com/gin-contrib/cors"
	"github.com/gin-gonic/gin"
	"net/http"
	"os"
	"os/exec"
	"path/filepath"
	"syscall"
	"time"
)

var ProcessInfo []dataProcessing.Level1
var MessageBoxInfo []dataProcessing.Level1
var HeapInfo []dataProcessing.Level1
var FileInfo []dataProcessing.Level1
var RegeditInfo []dataProcessing.Level1
var NetInfo []dataProcessing.Level1
var MemoryInfo []dataProcessing.Level1
var WarningInfo []dataProcessing.Level1
var ProgramDirPath string
var heapCheck = make(map[string]dataProcessing.HeapCheck)
var execfile string

var end = 0
var start = 0
var status = false

func getdata() {
	for true {
		dataProcessing.GetData(heapCheck,
			&ProgramDirPath,
			&(ProcessInfo[0].Lable1name),
			&MessageBoxInfo,
			&HeapInfo,
			&FileInfo,
			&RegeditInfo,
			&NetInfo,
			&MemoryInfo,
			&WarningInfo,
			&start,
			&end,
			&status,
		)
		time.Sleep(1000 * time.Millisecond)
	}
}

func main() {
	startupinfo := new(syscall.StartupInfo)
	processinformation := new(syscall.ProcessInformation)

	ProgramDirPath = ""
	dataProcessing.ProcessInfoInit(&ProcessInfo)
	dataProcessing.MessageBoxInfoInit(&MessageBoxInfo)
	dataProcessing.HeapInfoInit(&HeapInfo)
	dataProcessing.FileInfoInit(&FileInfo)
	dataProcessing.RegeditInfoInit(&RegeditInfo)
	dataProcessing.NetInfoInit(&NetInfo)
	dataProcessing.MemoryInfoInit(&MemoryInfo)
	dataProcessing.WarningInfoInit(&WarningInfo)

	r := gin.Default()

	r.Use(cors.Default())

	r.GET("/GeneralAction", func(c *gin.Context) {
		action := c.Query("action")
		if action == "start" {
			file, ok := c.GetQuery("file")
			if ok {
				_, err := os.Stat(file)
				if err == nil {
					filename, _ := syscall.UTF16PtrFromString("E:\\homework\\detours-cmake-master\\build\\bin\\DEBUG\\runwithdll.exe")
					execfile = filepath.Base(file)
					commandLine, _ := syscall.UTF16PtrFromString(file)
					ProgramDirPath = filepath.Dir(file)
					si := new(syscall.StartupInfo)
					pi := new(syscall.ProcessInformation)
					err := syscall.CreateProcess(filename, commandLine, nil, nil, false, api32.CREATE_NEW_CONSOLE, nil, nil, si, pi)
					if err != nil {
						c.JSON(http.StatusExpectationFailed, gin.H{"status": "failed"})
					} else {
						status = true
						c.JSON(http.StatusOK, gin.H{"status": "success"})
						dataProcessing.ProcessInfoInit(&ProcessInfo)
						dataProcessing.MessageBoxInfoInit(&MessageBoxInfo)
						dataProcessing.HeapInfoInit(&HeapInfo)
						dataProcessing.FileInfoInit(&FileInfo)
						dataProcessing.RegeditInfoInit(&RegeditInfo)
						dataProcessing.NetInfoInit(&NetInfo)
						dataProcessing.MemoryInfoInit(&MemoryInfo)
						dataProcessing.WarningInfoInit(&WarningInfo)
						start = 0
						end = 0
						go getdata()
						go dataProcessing.GetprocessInfo(&status, startupinfo, processinformation, &ProcessInfo)
					}
				} else {
					c.JSON(http.StatusExpectationFailed, gin.H{"status": "failed"})
				}
			} else {
				c.JSON(http.StatusExpectationFailed, gin.H{"status": "failed"})
			}
		} else if action == "stop" {
			cmd := exec.Command("taskkill.exe", "/F", "/IM", execfile)
			output, _ := cmd.CombinedOutput()
			fmt.Println(string(output))
			fmt.Println(cmd)
			status = false
			c.JSON(http.StatusOK, gin.H{"status": "stopped success"})
		}
	})

	r.GET("/ProcessAction", router.InfoHandler(&ProcessInfo))
	r.GET("/MessageBoxAction", router.InfoHandler(&MessageBoxInfo))
	r.GET("/HeapAction", router.InfoHandler(&HeapInfo))
	r.GET("/FiAction", router.InfoHandler(&FileInfo))
	r.GET("/RegeditIAction", router.InfoHandler(&RegeditInfo))
	r.GET("/NetAction", router.InfoHandler(&NetInfo))
	r.GET("/MemAction", router.InfoHandler(&MemoryInfo))
	r.GET("/WarningAction", router.InfoHandler(&WarningInfo))

	s := &http.Server{
		Addr:           ":23333",
		Handler:        r,
		ReadTimeout:    10 * time.Second,
		WriteTimeout:   10 * time.Second,
		MaxHeaderBytes: 1 << 20,
	}
	s.ListenAndServe()
}

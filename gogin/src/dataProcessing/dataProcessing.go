package dataProcessing

import (
	"../api32"
	"../getId"
	"bytes"
	"fmt"
	"path/filepath"
	"strings"
	"syscall"
	"time"
	"unsafe"
)

var TYPENAME = [22]string{"", "MessageBoxW", "MessageBoxA", "HeapCreate", "HeapDestroy", "HeapFree", "CreateFile", "WriteFile", "ReadFile", "RegCreateKeyEx", "RegOpenKeyEx", "RegSetValueEx", "RegDeleteValue", "RegDeleteKey", "RegCloseKey", "socket", "bind", "send", "connect", "recv", "memcpy", "CopyFile"}

type HeapCheck struct {
	CreateNum  int
	FreeNum    int
	DestroyNum int
}

type SharedInfo struct {
	Atype  uint16
	Nums   uint16
	Ttime  syscall.Systemtime
	Names  [10][100]byte
	Values [10][100]byte
}

type SendInfo struct {
	Typename string
	Argc     int
	Time     string
	Argv     map[string]string
}

type ModuleName struct {
	Name [256]byte
}

type Level3 struct {
	Id         string `json:"id"`
	Label3name string `json:"label"`
}

type Level2 struct {
	Id         string   `json:"id"`
	Label2name string   `json:"label"`
	Children2  []Level3 `json:"children"`
}

type Level1 struct {
	Id         string   `json:"id"`
	Lable1name string   `json:"label"`
	Children1  []Level2 `json:"children"`
}

func NewSendInfo() *SendInfo {
	return &SendInfo{
		Argv: make(map[string]string),
	}
}

func RetJson(data *SharedInfo) *SendInfo {
	retSendInfo := NewSendInfo()
	retSendInfo.Typename = TYPENAME[data.Atype]
	retSendInfo.Argc = int(data.Nums)
	retSendInfo.Time = fmt.Sprintf("%d-%d-%d-%d-%d-%d", data.Ttime.Year, data.Ttime.Month, data.Ttime.Day, data.Ttime.Hour, data.Ttime.Minute, data.Ttime.Second)
	for i := 0; i < retSendInfo.Argc; i++ {
		n1 := bytes.IndexByte(data.Names[i][:], 0)
		n2 := bytes.IndexByte(data.Values[i][:], 0)
		retSendInfo.Argv[string(data.Names[i][:n1])] = string(data.Values[i][:n2])
	}

	return retSendInfo
}

func LastLevel(data *SendInfo) *Level2 {
	retLevel := new(Level2)
	retLevel.Label2name = fmt.Sprintf("%s: %s", data.Typename, data.Time)
	retLevel.Id = getId.UniqueId()
	for k, v := range data.Argv {
		var tmpS Level3
		tmpS.Label3name = fmt.Sprintf("%s: %s", k, v)
		tmpS.Id = getId.UniqueId()
		retLevel.Children2 = append(retLevel.Children2, tmpS)
	}
	return retLevel
}

func ProcessInfoInit(Info *[]Level1) {
	*Info = (*Info)[:0]
	*Info = append(*Info, Level1{Id: "0", Lable1name: "test.exe", Children1: []Level2{}})
	*Info = append(*Info, Level1{Id: "1", Lable1name: "ProcessInfo", Children1: []Level2{}})
	*Info = append(*Info, Level1{Id: "2", Lable1name: "ModuleInfo", Children1: []Level2{}})
}

func MessageBoxInfoInit(Info *[]Level1) {
	*Info = (*Info)[:0]
	*Info = append(*Info, Level1{Id: "0", Lable1name: "MessageBoxW", Children1: []Level2{}})
	*Info = append(*Info, Level1{Id: "1", Lable1name: "MessageBoxA", Children1: []Level2{}})
}

func HeapInfoInit(Info *[]Level1) {
	*Info = (*Info)[:0]
	*Info = append(*Info, Level1{Id: "0", Lable1name: "HeapCreate", Children1: []Level2{}})
	*Info = append(*Info, Level1{Id: "1", Lable1name: "HeapDestory", Children1: []Level2{}})
	*Info = append(*Info, Level1{Id: "2", Lable1name: "HeapFree", Children1: []Level2{}})
}

func FileInfoInit(Info *[]Level1) {
	*Info = (*Info)[:0]
	*Info = append(*Info, Level1{Id: "0", Lable1name: "CreateFile", Children1: []Level2{}})
	*Info = append(*Info, Level1{Id: "1", Lable1name: "WriteFile", Children1: []Level2{}})
	*Info = append(*Info, Level1{Id: "2", Lable1name: "ReadFile", Children1: []Level2{}})
	*Info = append(*Info, Level1{Id: "3", Lable1name: "CopyFile", Children1: []Level2{}})
}

func RegeditInfoInit(Info *[]Level1) {
	*Info = (*Info)[:0]
	*Info = append(*Info, Level1{Id: "0", Lable1name: "RegCreateKeyEX", Children1: []Level2{}})
	*Info = append(*Info, Level1{Id: "1", Lable1name: "RegOpenKeyEX", Children1: []Level2{}})
	*Info = append(*Info, Level1{Id: "2", Lable1name: "RegSetValueEX", Children1: []Level2{}})
	*Info = append(*Info, Level1{Id: "3", Lable1name: "RegDeleteValue", Children1: []Level2{}})
	*Info = append(*Info, Level1{Id: "4", Lable1name: "RegDeleteKey", Children1: []Level2{}})
	*Info = append(*Info, Level1{Id: "5", Lable1name: "RegCloseKey", Children1: []Level2{}})
}

func NetInfoInit(Info *[]Level1) {
	*Info = (*Info)[:0]
	*Info = append(*Info, Level1{Id: "0", Lable1name: "Socket", Children1: []Level2{}})
	*Info = append(*Info, Level1{Id: "1", Lable1name: "Bind", Children1: []Level2{}})
	*Info = append(*Info, Level1{Id: "2", Lable1name: "Send", Children1: []Level2{}})
	*Info = append(*Info, Level1{Id: "3", Lable1name: "Connect", Children1: []Level2{}})
	*Info = append(*Info, Level1{Id: "4", Lable1name: "Recv", Children1: []Level2{}})
}

func MemoryInfoInit(Info *[]Level1) {
	*Info = (*Info)[:0]
	*Info = append(*Info, Level1{Lable1name: "Memcpy", Children1: []Level2{}})
}

func WarningInfoInit(Info *[]Level1) {
	*Info = (*Info)[:0]
	*Info = append(*Info, Level1{Id: "0", Lable1name: "HeapWarning", Children1: []Level2{}})
	*Info = append(*Info, Level1{Id: "1", Lable1name: "FileWarning", Children1: []Level2{}})
	*Info = append(*Info, Level1{Id: "2", Lable1name: "RegeditWarning", Children1: []Level2{}})
}

func GetData(HeapMap map[string]HeapCheck,
	ProgramDirPath *string,
	itname *string,
	MessageBoxInfo *[]Level1,
	HeapInfo *[]Level1,
	FileInfo *[]Level1,
	RegeditInfo *[]Level1,
	NetInfo *[]Level1,
	MemoryInfo *[]Level1,
	WarningInfo *[]Level1,
	start *int,
	end *int,
	status *bool) {
	api32.GetNum(end)
	if *end > *start {
		for i := *start; i < *end; i++ {
			if !(*status) {
				continue
			}
			a := new(SharedInfo)
			api32.GetSeg(unsafe.Pointer(a), i)
			b := RetJson(a)
			if a.Atype == 1 {
				(*MessageBoxInfo)[0].Children1 = append((*MessageBoxInfo)[0].Children1, *LastLevel(b))
			} else if a.Atype == 2 {
				(*MessageBoxInfo)[1].Children1 = append((*MessageBoxInfo)[1].Children1, *LastLevel(b))
			} else if a.Atype == 3 {
				(*HeapInfo)[0].Children1 = append((*HeapInfo)[0].Children1, *LastLevel(b))
				HeapMap[b.Argv["hFile"]] = HeapCheck{CreateNum: 1, FreeNum: 0, DestroyNum: 0}
			} else if a.Atype == 4 {
				(*HeapInfo)[1].Children1 = append((*HeapInfo)[1].Children1, *LastLevel(b))
				if _, ok := HeapMap[b.Argv["hHeap"]]; !ok {
					Wlabel := fmt.Sprintf("The program destroyed a non-existent heap!!! %s", b.Time)
					c := Level2{Label2name: Wlabel, Id: getId.UniqueId()}
					(*WarningInfo)[0].Children1 = append((*WarningInfo)[0].Children1, c)
				} else {
					hHeap := HeapMap[b.Argv["hHeap"]]
					hHeap.DestroyNum += 1
					HeapMap[b.Argv["hHeap"]] = hHeap
					if hHeap.DestroyNum > hHeap.CreateNum {
						Wlabel := fmt.Sprintf("The program repeat destroyed a heap!!! %s", b.Time)
						c := Level2{Label2name: Wlabel, Id: getId.UniqueId()}
						(*WarningInfo)[0].Children1 = append((*WarningInfo)[0].Children1, c)
					}
				}
			} else if a.Atype == 5 {
				(*HeapInfo)[2].Children1 = append((*HeapInfo)[2].Children1, *LastLevel(b))
				if _, ok := HeapMap[b.Argv["hHeap"]]; !ok {
					Wlabel := fmt.Sprintf("The program free a non-existent heap!!! %s", b.Time)
					c := Level2{Label2name: Wlabel, Id: getId.UniqueId()}
					(*WarningInfo)[0].Children1 = append((*WarningInfo)[0].Children1, c)
				} else {
					hHeap := HeapMap[b.Argv["hHeap"]]
					hHeap.FreeNum += 1
					HeapMap[b.Argv["hHeap"]] = hHeap
					if hHeap.FreeNum > hHeap.CreateNum {
						Wlabel := fmt.Sprintf("The program repeat free a heap!!! %s", b.Time)
						c := Level2{Label2name: Wlabel, Id: getId.UniqueId()}
						(*WarningInfo)[0].Children1 = append((*WarningInfo)[0].Children1, c)
					}
				}
			} else if a.Atype == 6 {
				(*FileInfo)[0].Children1 = append((*FileInfo)[0].Children1, *LastLevel(b))
				dstPath := b.Argv["lpFileName"]
				dstPath = strings.Replace(dstPath, "/", "\\", -1)
				if filepath.IsAbs(dstPath) {
					if !strings.HasPrefix(dstPath, *ProgramDirPath) {
						Wlabel := fmt.Sprintf("The program is modifying your other directory file %s !!! %s", dstPath, b.Time)
						c := Level2{Label2name: Wlabel, Id: getId.UniqueId()}
						(*WarningInfo)[1].Children1 = append((*WarningInfo)[1].Children1, c)
					}
				} else {
					dstPath = filepath.Join(*ProgramDirPath, dstPath)
					if !strings.HasPrefix(dstPath, *ProgramDirPath) {
						Wlabel := fmt.Sprintf("The program is modifying your other directory file %s !!! %s", dstPath, b.Time)
						c := Level2{Label2name: Wlabel, Id: getId.UniqueId()}
						(*WarningInfo)[1].Children1 = append((*WarningInfo)[1].Children1, c)
					}
				}
				if *itname != "test.exe" {
					if strings.Index(dstPath, *itname) != -1 {
						Wlabel := fmt.Sprintf("Self-replication Warning!!! %s", b.Time)
						c := Level2{Label2name: Wlabel, Id: getId.UniqueId()}
						(*WarningInfo)[1].Children1 = append((*WarningInfo)[1].Children1, c)
						continue
					}
				}
				if strings.HasSuffix(dstPath, "exe") {
					Wlabel := fmt.Sprintf("The program is modifying your exe file %s !!! %s", dstPath, b.Time)
					c := Level2{Label2name: Wlabel, Id: getId.UniqueId()}
					(*WarningInfo)[1].Children1 = append((*WarningInfo)[1].Children1, c)
				} else if strings.HasSuffix(dstPath, "dll") {
					Wlabel := fmt.Sprintf("The program is modifying your dll file %s !!! %s", dstPath, b.Time)
					c := Level2{Label2name: Wlabel, Id: getId.UniqueId()}
					(*WarningInfo)[1].Children1 = append((*WarningInfo)[1].Children1, c)
				} else if strings.HasSuffix(dstPath, "ocx") {
					Wlabel := fmt.Sprintf("The program is modifying your ocx file %s !!! %s", dstPath, b.Time)
					c := Level2{Label2name: Wlabel, Id: getId.UniqueId()}
					(*WarningInfo)[1].Children1 = append((*WarningInfo)[1].Children1, c)
				}
			} else if a.Atype == 7 {
				(*FileInfo)[1].Children1 = append((*FileInfo)[1].Children1, *LastLevel(b))
			} else if a.Atype == 8 {
				(*FileInfo)[2].Children1 = append((*FileInfo)[2].Children1, *LastLevel(b))
			} else if a.Atype == 9 {
				(*RegeditInfo)[0].Children1 = append((*RegeditInfo)[0].Children1, *LastLevel(b))
			} else if a.Atype == 10 {
				(*RegeditInfo)[1].Children1 = append((*RegeditInfo)[1].Children1, *LastLevel(b))
				hKey := b.Argv["lpSubKey"]
				hKey = strings.Replace(hKey, "/", "\\", -1)
				if strings.EqualFold(hKey, "Software\\Microsoft\\Windows\\CurrentVersion\\Run") ||
					strings.EqualFold(hKey, "Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce") ||
					strings.EqualFold(hKey, "Software\\Microsoft\\Windows\\CurrentVersion\\RunServices") ||
					strings.EqualFold(hKey, "Software\\Microsoft\\Windows\\CurrentVersion\\RunServicesOnce") ||
					strings.EqualFold(hKey, "Software\\Microsoft\\Windows\\CurrentVersion\\RunOnceEx") ||
					strings.EqualFold(hKey, "Software\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon\\Userinit") {
					Wlabel := fmt.Sprintf("The program is modifying your startup !!! %s", b.Time)
					c := Level2{Label2name: Wlabel, Id: getId.UniqueId()}
					(*WarningInfo)[2].Children1 = append((*WarningInfo)[2].Children1, c)
				}
			} else if a.Atype == 11 {
				(*RegeditInfo)[2].Children1 = append((*RegeditInfo)[2].Children1, *LastLevel(b))
			} else if a.Atype == 12 {
				(*RegeditInfo)[3].Children1 = append((*RegeditInfo)[3].Children1, *LastLevel(b))
			} else if a.Atype == 13 {
				(*RegeditInfo)[4].Children1 = append((*RegeditInfo)[4].Children1, *LastLevel(b))
			} else if a.Atype == 14 {
				(*RegeditInfo)[5].Children1 = append((*RegeditInfo)[5].Children1, *LastLevel(b))
			} else if a.Atype == 15 {
				(*NetInfo)[0].Children1 = append((*NetInfo)[0].Children1, *LastLevel(b))
			} else if a.Atype == 16 {
				(*NetInfo)[1].Children1 = append((*NetInfo)[1].Children1, *LastLevel(b))
			} else if a.Atype == 17 {
				(*NetInfo)[2].Children1 = append((*NetInfo)[2].Children1, *LastLevel(b))
			} else if a.Atype == 18 {
				(*NetInfo)[3].Children1 = append((*NetInfo)[3].Children1, *LastLevel(b))
			} else if a.Atype == 19 {
				(*NetInfo)[4].Children1 = append((*NetInfo)[4].Children1, *LastLevel(b))
			} else if a.Atype == 20 {
				(*MemoryInfo)[0].Children1 = append((*MemoryInfo)[0].Children1, *LastLevel(b))
			} else if a.Atype == 21 {
				c := *LastLevel(b)
				(*FileInfo)[3].Children1 = append((*FileInfo)[3].Children1, c)
				Wlabel := fmt.Sprintf("CopyFile Warning: %s", b.Time)
				c.Label2name = Wlabel
				c.Id = getId.UniqueId()
				(*WarningInfo)[1].Children1 = append((*WarningInfo)[1].Children1, c)
			}
			api32.GetNum(end)
		}
		*start = *end
		if *end >= 990 {
			*start = 0
			*end = 0
			api32.ClearSeg()
		}
	}
}

func getProcessInfo(pi *syscall.ProcessInformation) *[]Level2 {
	var ret []Level2
	labelname := fmt.Sprintf("Process:= %d", pi.Process)
	ret = append(ret, Level2{getId.UniqueId(), labelname, nil})
	labelname = fmt.Sprintf("Thread:= %d", pi.Thread)
	ret = append(ret, Level2{getId.UniqueId(), labelname, nil})
	labelname = fmt.Sprintf("ProcessId:= %d", pi.ProcessId)
	ret = append(ret, Level2{getId.UniqueId(), labelname, nil})
	labelname = fmt.Sprintf("ThreadId:= %d", pi.ThreadId)
	ret = append(ret, Level2{getId.UniqueId(), labelname, nil})
	return &ret
}

func GetprocessInfo(status *bool,
	si *syscall.StartupInfo,
	pi *syscall.ProcessInformation,
	ProcessInfo *[]Level1) {
	for true {
		time.Sleep(2000 * time.Millisecond)
		if *status {
			api32.GetProInfo(si, pi)
			var moduleNum int
			api32.GetModuleNum(&moduleNum)
			var a ModuleName
			api32.GetModule(unsafe.Pointer(&a), 0)
			n1 := bytes.IndexByte(a.Name[:], 0)
			name1 := string(a.Name[:n1])
			(*ProcessInfo)[0].Lable1name = name1
			(*ProcessInfo)[1].Children1 = *getProcessInfo(pi)
			for i := 1; i < moduleNum-5; i++ {
				api32.GetModule(unsafe.Pointer(&a), i)
				n1 = bytes.IndexByte(a.Name[:], 0)
				name1 = string(a.Name[:n1])
				(*ProcessInfo)[2].Children1 = append((*ProcessInfo)[2].Children1, Level2{getId.UniqueId(), name1, nil})
			}
			break
		}
	}
}

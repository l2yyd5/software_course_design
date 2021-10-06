package api32

import (
	"syscall"
	"unsafe"
)

const CREATE_NEW_CONSOLE = 0x00000010

var (
	dllsample = syscall.NewLazyDLL("E:\\homework\\detours-cmake-master\\build\\bin\\DEBUG\\dllsample.dll")
	procGetSeg = dllsample.NewProc("GetSeg")
	procGetProInfo = dllsample.NewProc("GetProInfo")
	procGetNum = dllsample.NewProc("GetNum")
	procClearSeg = dllsample.NewProc("ClearSeg")
	procGetModuleNum = dllsample.NewProc("GetModuleNum")
	procGetModule = dllsample.NewProc("GetModule")
)

func GetSeg(data unsafe.Pointer, i int){
	syscall.Syscall6(procGetSeg.Addr(), 2,
		uintptr(data),
		uintptr(i),
		0, 0, 0, 0,
	)
}

func GetModule(data unsafe.Pointer, i int){
	syscall.Syscall6(procGetModule.Addr(), 2,
		uintptr(data),
		uintptr(i),
		0, 0, 0, 0,
	)
}

func GetProInfo(si *syscall.StartupInfo, pi *syscall.ProcessInformation){
	syscall.Syscall6(procGetProInfo.Addr(), 2,
		uintptr(unsafe.Pointer(si)),
		uintptr(unsafe.Pointer(pi)),
		0, 0, 0, 0,
	)
}

func GetNum(i *int){
	syscall.Syscall6(procGetNum.Addr(), 1,
		uintptr(unsafe.Pointer(i)),
		0, 0, 0, 0, 0,
	)
}

func GetModuleNum(i *int){
	syscall.Syscall6(procGetModuleNum.Addr(), 1,
		uintptr(unsafe.Pointer(i)),
		0, 0, 0, 0, 0,
	)
}

func ClearSeg(){
	syscall.Syscall6(procClearSeg.Addr(), 0,
		0, 0, 0, 0, 0, 0,
	)
}
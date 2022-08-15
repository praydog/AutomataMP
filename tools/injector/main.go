package main

// import windows stuff
import (
	"flag"
	"fmt"
	"path/filepath"
	"strings"
	"unsafe"

	peparser "github.com/saferwall/pe"
	"golang.org/x/sys/windows"
)

func pidFromName(name string) uint32 {
	name = strings.ToLower(name)

	pe := &windows.ProcessEntry32{}
	pe.Size = uint32(unsafe.Sizeof(*pe))

	snapshot, err := windows.CreateToolhelp32Snapshot(windows.TH32CS_SNAPPROCESS, 0)

	if err != nil {
		panic(err)
	}

	defer windows.CloseHandle(snapshot)

	for {
		if err := windows.Process32Next(snapshot, pe); err != nil {
			break
		}

		processName := windows.UTF16ToString(pe.ExeFile[:])
		processName = strings.ToLower(processName)

		if processName == name {
			return pe.ProcessID
		}
	}

	return 0
}

func findRemoteModule(pid uint32, name string, is32bit bool) *windows.ModuleEntry32 {
	name = strings.ToLower(name)

	me := &windows.ModuleEntry32{}
	me.Size = uint32(unsafe.Sizeof(*me))

	flag := windows.TH32CS_SNAPMODULE

	if is32bit {
		flag |= windows.TH32CS_SNAPMODULE32
	}

	snapshot, err := windows.CreateToolhelp32Snapshot(uint32(flag), pid)

	if err != nil {
		panic(err)
	}

	defer windows.CloseHandle(snapshot)

	for {
		if err := windows.Module32Next(snapshot, me); err != nil {
			break
		}

		processName := windows.UTF16ToString(me.Module[:])
		processName = strings.ToLower(processName)

		fmt.Printf("%s\n", processName)

		if processName == name {
			return me
		}
	}

	return nil
}

func allocRemoteString(process windows.Handle, str string) uintptr {
	stringBytes := []byte(str)

	kernel32 := windows.NewLazySystemDLL("kernel32.dll")
	VirtualAllocEx := kernel32.NewProc("VirtualAllocEx")

	stringPtr, _, err := VirtualAllocEx.Call(uintptr(process), uintptr(0), uintptr(len(stringBytes)+1), uintptr(windows.MEM_COMMIT), uintptr(windows.PAGE_READWRITE))

	if err != nil && err.Error() != "The operation completed successfully." {
		panic(err)
	}

	fmt.Printf("StringPtr: %x\n", stringPtr)

	// Write string to target process
	WriteProcessMemory := kernel32.NewProc("WriteProcessMemory")
	_, _, err = WriteProcessMemory.Call(uintptr(process), stringPtr, uintptr(unsafe.Pointer(&stringBytes[0])), uintptr(len(stringBytes)+1), uintptr(0))

	if err != nil && err.Error() != "The operation completed successfully." {
		panic(err)
	}

	return stringPtr
}

func inject(pid uint32, dll string) {
	// open process
	PROCESS_ALL_ACCESS := windows.PROCESS_CREATE_THREAD | windows.PROCESS_VM_OPERATION | windows.PROCESS_VM_WRITE | windows.PROCESS_VM_READ | windows.PROCESS_QUERY_INFORMATION
	process, err := windows.OpenProcess(uint32(PROCESS_ALL_ACCESS), false, pid)

	if err != nil {
		panic(err)
	}

	defer windows.CloseHandle(process)

	kernel32 := windows.NewLazySystemDLL("kernel32.dll")
	//VirtualAllocEx := kernel32.NewProc("VirtualAllocEx")
	//WriteProcessMemory := kernel32.NewProc("WriteProcessMemory")

	stringPtr := allocRemoteString(process, dll)

	is32bit := false
	windows.IsWow64Process(process, &is32bit)

	loadLibAddr := uintptr(0)
	CreateRemoteThread := kernel32.NewProc("CreateRemoteThread")

	if !is32bit {
		loadLibAddr = kernel32.NewProc("LoadLibraryA").Addr()

		// Call LoadLibraryA in target process
		_, _, err = CreateRemoteThread.Call(uintptr(process), uintptr(0), uintptr(0), loadLibAddr, stringPtr, uintptr(0), uintptr(0))

		if err != nil && err.Error() != "The operation completed successfully." {
			panic(err)
		}
	} else {
		fmt.Printf("Process is 32bit.\n")

		kernel32Wow64 := findRemoteModule(pid, "kernel32.dll", true)
		pe, err := peparser.New(windows.UTF16ToString(kernel32Wow64.ExePath[:]), &peparser.Options{})

		if err != nil {
			panic(err)
		}

		err = pe.Parse()

		if err != nil {
			panic(err)
		}

		for _, f := range pe.Export.Functions {
			fmt.Printf("%s\n", f.Name)

			if f.Name == "LoadLibraryA" {
				loadLibAddr = kernel32Wow64.ModBaseAddr + uintptr(f.FunctionRVA)
				break
			}
		}

		fmt.Printf("LoadLibAddr: %x\n", loadLibAddr)

		// Load shellcode into target process
		/*shellcode := []uint8{
			0x68, 0x00, 0x00, 0x00, 0x00, // push dll name
			0xB8, 0x00, 0x00, 0x00, 0x00, // mov eax, LoadLibraryA
			0xFF, 0xD0, // call eax
			0xC3, // ret
		}

		shellPtr, _, err := VirtualAllocEx.Call(uintptr(process), uintptr(0), uintptr(len(shellcode)), uintptr(windows.MEM_COMMIT), uintptr(windows.PAGE_EXECUTE_READWRITE))

		if err != nil && err.Error() != "The operation completed successfully." {
			panic(err)
		}

		fmt.Printf("ShellPtr: %x\n", shellPtr)

		// Write shellcode skeleton
		_, _, _ = WriteProcessMemory.Call(uintptr(process), shellPtr, uintptr(unsafe.Pointer(&shellcode[0])), uintptr(len(shellcode)), uintptr(0))
		// Write module name to push
		_, _, _ = WriteProcessMemory.Call(uintptr(process), shellPtr+1, uintptr(unsafe.Pointer(&stringPtr)), uintptr(4), uintptr(0))
		// Write LoadLibraryA to mov eax
		_, _, _ = WriteProcessMemory.Call(uintptr(process), shellPtr+6, uintptr(unsafe.Pointer(&loadLibAddr)), uintptr(4), uintptr(0))

		fmt.Printf("Calling shellcode at %x\n", shellPtr)*/

		thr, _, err := CreateRemoteThread.Call(uintptr(process), uintptr(0), uintptr(0), loadLibAddr, stringPtr, uintptr(0), uintptr(0))

		if err != nil && err.Error() != "The operation completed successfully." {
			panic(err)
		}

		fmt.Printf("Thread: %x\n", thr)
	}
}

func main() {
	procname := flag.String("procname", "", "process name to inject")
	dll := flag.String("dll", "", "dll to inject")
	flag.Parse()

	if *procname == "" {
		panic("no process name specified")
	}

	if *dll == "" {
		panic("no dll specified")
	}

	// get process id
	pid := pidFromName(*procname)
	fmt.Printf("PID: %d\n", pid)

	if pid == 0 {
		panic("could not find process")
	}

	// Get full path to DLL
	dllPath, err := filepath.Abs(*dll)

	if err != nil {
		panic(err)
	}

	fmt.Printf("DLL: %s\n", dllPath)

	// inject dll
	inject(pid, dllPath)
	fmt.Printf("Injected.")
}

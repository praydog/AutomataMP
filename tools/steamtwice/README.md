# steamtwice

This project is generally for developers. It forces Steam to launch NieR: Automata twice, which is useful for local multiplayer development.

## Building
Same as any other CMake project, except this MUST be built in 32-bit/x86 mode. Steam is 32-bit, so this must be too.

## Injecting
The folder next to this one, `injector` contains an optional DLL injector.

### Build it with:
`go build -o injector.exe main.go`

### Then run it with (in the same folder as steamtwice.dll):
`./injector.exe --procname=Steam.exe --dll=steamtwice.dll`
# AutomataMP
NieR: Automata Multiplayer Mod

## Features
### Standalone server
* Server coded in Go that can run on either Linux or Windows
* MasterClient system to allow a "host" that controls the game world (enemies and other world state)

### Player Sync
* Position
* Rotation
* Animations
* Pod firing
* Pod programs
* Pod flashlight
* Weapon/loadout selection (but not the actual weapon types themselves)

### Enemy Sync
* Replicates enemy spawns as the MasterClient spawns them
* Deletes enemies on normal client's ends that are not supposed to be networked
* Position
* Rotation
* Health/alive state
* Some animations

## Planned/Wanted Features
* Model changer
* PvP
* Room/lobby system
* Ghost system when not connected to a room/lobby

## History
Mod that was developed back in 2017 and had a very short development before I moved on to other things. This was not open sourced and released sooner as the libraries behind the original version were not open source friendly.

Because this is fairly old code originally not meant to be released publicly, the coding standards are not up to my current set. Please forgive this. It may be fixed over time.

### Late August 2017
* Development began
* Lots of reverse engineering
  * Scripting system (mruby?)
  * Entity System
  * Buddy System
  * Animations, buttons
* Added enet & enetpp, connected these systems to multiplayer
  * Server was hosted directly inside the mod itself, not a standalone program
  * Client/server communicated with raw structure data defined in C++
* System used VEH hooks for hooking the middle of functions and retrieving the CPU context
  
### Early September 2017
  * Project was abandoned, a few short videos of it in action were recorded
 
[2017-09-01_06-35-26.webm](https://user-images.githubusercontent.com/2909949/183229209-b18739bd-3893-42e4-ba8c-d83b15c643da.webm)
  
### Late July 2022-Present
* Project was revived 
  * A combination of NieR making headlines, modding tools being released, and me getting contacted boosted motivation here
* Ripped out any code that was not open source friendly and substituted everything with the open source equivalents
* Server code was placed into a standalone Go project that can run on either Linux or Windows
* Client/server code refactored to use [flatbuffers](https://github.com/google/flatbuffers) for packet structure
* New system uses MidHook from [safetyhook](https://github.com/cursey/safetyhook) to perform mid function hooks and retrieve the context
  * This is much faster and doesn't have the drawbacks of not being thread safe
* Client code refactor started to hold up to current standards and better reflect the actual game structure

# WinHideEx (Windows Hider Extended)
_"Hide all of those pesky unwanted files and folders using DLL injection!"_

## What is this?
WinHideEx is a piece of software designed to hide pesky, cluttery, or otherwise unwanted files and directories automatically by hooking NtQueryDirectoryFile(Ex)
system calls. It does this through remote DLL injection (managed by WinHideEx GUI) and API hooking using [Microsoft Detours](https://github.com/microsoft/Detours).

With WinHideEx you can hide whatever files and folders you wish, so long as the target application has been injected at some point by the provided injector service.
Simply load the management GUI from the toolbar, add the programs you wish to hide files/directories from (for example, I have added Windows Explorer and Command
Prompt), create a "hider" regex to match those unsightly inodes (example: "\..*" will hide all files and folders beginning with a dot), and finally click save! The
service will continuously monitor the process list for any new matching program instances and magically patch them as well. It's simple!

## Why?
Because I hated having tons of gross dotfiles visible all over my home directory. Windows explorer doesn't have an option to hide them, so I took matters into my own
hands. Yes, I'm _aware_ that you can manually mark all of them with the "hidden" attribute but it's like playing NTFS whack-a-mole with a filesystem watcher script.
This way feels much more elegant, extendable, and granular. Plus, it was a lot of fun to write over the weekend and put my malware analysis knowledge to good use!
Who knows, maybe you'll learn a thing or two about thunking the Windows native API after browsing through this mess of a codebase.

## Wait? I have to inject into *EVERY* process? Why not just write a VFS filter driver to hide files?
Ok, admittedly the requirement of force-injecting strange WinHideEx$ARCH.dll files into system processes sounds a bit asinine. Honestly, that's because it is.
I put a lot of care into making my API hooking logic as safe and bug-free as 2 days worth of half-assed effort allowed. So yes, I admit it, after basic brainstorming
I too came to the easy conclusion that writing a filter driver is, in fact, the "correct" way of doing things. But distributing a filter driver isn't easy. In fact,
Microsoft makes it downright hard to release unsigned drivers to the masses. Even userspace drivers. Driver signing requires both time and lots of money. And,
honestly, I have neither. So if you want to take on this task and pay an arm and a leg for a code signing certificate I say best of luck to you bold adventurer!

## Installation
I'm a nice fellow, so I put compiled builds and a fancy installer over at [the releases page](https://github.com/joshumax/WinHideEx/releases/). Feel free to try it
out on your machine and report any bugs. The GUI is admittedly basic, but it should be generally self-explainatory. Please note that if something explodes and
kills your cat, I am in no way liable for the destruction it causes. This is basically one giant hack; you have been warned.

## Contributing
If you feel like contributing to this chaos, feel free to send a pull request or open up an issue. I don't bite. Perhaps you might want to start by expanding this
README?

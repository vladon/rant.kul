kul
===

C++ Kommon usage library

Convenience library providing cross platform threading/process management/io ops/logging

How to use:
view tst/kul.test.hpp

License

LGPL
http://www.gnu.org/licenses/lgpl-2.1


Switches

Key 			__KUL_THREAD_SPAWN_WAIT__
Type 			number
Default
	nix 		5000000
	bsd 		5000000
	win 		0
OS 				nix/bsd/win
Description
Used in thread pools. The amount of time in nanoseconds waited after launching a thread.
Threads launched too quickly can cause issues on nix platforms, bsd assumed similar.

Key 			__KUL_PROC_BLOCK_ERR__ 
Type 			flag
Default 		disabled
OS 				nix/bsd
Description
Turns on error checking for creating new processes when running - fcntl(fd, F_SETFL, O_NONBLOCK);
Can be an issue being on when running many processes rapidly.


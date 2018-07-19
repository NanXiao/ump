# ump
A universal thread-safe memory pool. This simple memory pool can be used if following conditions are satisfied:  
(1) The memory sizes are some fixed numbers. E.g, `32`, `64`, `128`, etc, not random;  
(2) The allocating memory operation is not so cheap, e.g., CUDA.  

Example:  

	#include <cstdlib>
	#include <iostream>
	#include "ump.h"
	
	int main()
	{
	    UMP ump{malloc, free};
	    auto p1 = ump.allocate_memory(100);
	    std::cout << p1 << std::endl;
	    auto p2 = ump.allocate_memory(100);
	    std::cout << p2 << std::endl;
	    ump.free_memory(p1);
	    auto p3 = ump.allocate_memory(100);
	    std::cout << p3 << std::endl;
	    ump.free_memory(p2);
	    ump.free_memory(p3);
	    std::cout << ump.get_used_memory() << std::endl;
	
	    return 0;
	}

Build and run it:  

	$ c++ -g -O2 -Wall main.cpp
	$ ./a.out
	0x55db2b807160
	0x55db2b807620
	0x55db2b807160
	200



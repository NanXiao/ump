# ump
## Introduction
A universal thread-safe memory pool. This simple memory pool can be used in some platforms which allocating memory operation is not so cheap (e.g., `CUDA`), and it can work as a cache to save time.  

You can modify the code implement other "pool" easily (e.g., connection pool).

## Design
The cores of this memory pool are `2` hash tables:  
  
a) The first hash table's key is memory block size, and value is a **queue** of  allocated memory addresses;  
b) The second hash table's key is memory address, and value is size of this memory block.  

When allocating memory, the memory pool queries the first hash table to check whether there is an unoccupied memory for the specified size: if have, pop the memory address from the queue, otherwise, trigger the genuine allocating memory operation, and add the memory address and size into the second hash table.  

When freeing memory, the memory pool queries the second hash table to get the size of current memory address, then access the first hash table to push the memory address back to queue.  

## Heads-up 
The number of keys of hash tables will affect their look-up performance, so if hash tables have too many keys, the performance may be downgraded.  

## Example 

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



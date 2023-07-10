#pragma once

#include "Extra.hpp"

class Indetour
{
private:

	void* function_address = 0;								
	void* hook_address = 0;									
	bool is_hooked = false;										
	byte* original_shellcode = 0;								
	size_t original_size = 0;									

public:

	Indetour();												
	bool hook(void* function_address, void* hook_address);		
	bool unhook();												
};
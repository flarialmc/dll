#pragma once

#include <Windows.h>
#include <iostream>

namespace Extra
{
	void* get_module(const char* module_name);										
	void* get_module_export(const char* module_name, const char* function_name);	
	bool wpm_safe(void* address, void* buffer, size_t size);						
	bool memset_safe(void* address, int buffer, size_t size);						
}
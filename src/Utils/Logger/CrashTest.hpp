#pragma once

namespace glaiel::crashlogs {
    // Test crash scenarios for validating crash log output
    // WARNING: These functions will intentionally crash the application!

    namespace test {
        // Trigger a null pointer dereference (EXCEPTION_ACCESS_VIOLATION)
        inline void test_null_pointer_crash() {
            int* null_ptr = nullptr;
            *null_ptr = 42; // This will crash
        }

        // Trigger a stack overflow (EXCEPTION_STACK_OVERFLOW)
        inline void test_stack_overflow_crash() {
            volatile char buffer[1024 * 1024]; // 1MB on stack
            buffer[0] = 1;
            test_stack_overflow_crash(); // Infinite recursion
        }

        // Trigger a divide by zero (EXCEPTION_INT_DIVIDE_BY_ZERO)
        inline void test_divide_by_zero_crash() {
            volatile int zero = 0;
            volatile int result = 42 / zero; // This will crash
            (void)result;
        }

        // Trigger an illegal instruction (EXCEPTION_ILLEGAL_INSTRUCTION)
        inline void test_illegal_instruction_crash() {
            void (*bad_func)() = (void(*)())0xDEADBEEF;
            bad_func(); // This will crash
        }

        // Trigger an access violation by writing to read-only memory
        inline void test_write_readonly_crash() {
            const char* readonly_string = "This is read-only";
            char* mutable_ptr = const_cast<char*>(readonly_string);
            mutable_ptr[0] = 'X'; // This will crash
        }

        // Trigger a pure virtual function call
        class BaseClass {
        public:
            virtual void pure_virtual() = 0;
            virtual ~BaseClass() = default;
        };

        inline void test_pure_virtual_crash() {
            BaseClass* obj = (BaseClass*)malloc(sizeof(void*));
            // Set up a fake vtable pointer
            void** vtable = (void**)malloc(sizeof(void*) * 2);
            vtable[0] = nullptr;
            *(void***)obj = vtable;
            obj->pure_virtual(); // This will crash
        }

        // Menu function to select which crash to test
        inline void trigger_test_crash(int crash_type = 1) {
            switch (crash_type) {
                case 1:
                    test_null_pointer_crash();
                    break;
                case 2:
                    test_stack_overflow_crash();
                    break;
                case 3:
                    test_divide_by_zero_crash();
                    break;
                case 4:
                    test_illegal_instruction_crash();
                    break;
                case 5:
                    test_write_readonly_crash();
                    break;
                case 6:
                    test_pure_virtual_crash();
                    break;
                default:
                    test_null_pointer_crash();
                    break;
            }
        }
    }
}

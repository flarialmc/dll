echo off

cmake .
cmake --build . --config Debug
cmake --build . --config Release
@echo off
echo Building fmt with vcpkg for faster linking...

cd vcpkg

echo Installing fmt...
.\vcpkg install fmt:x64-windows

echo fmt installed successfully!
cd ..
@echo off
echo Building ImGui with MultiThreadedDLL using vcpkg...

cd vcpkg

echo Installing ImGui with dynamic runtime...
.\vcpkg install imgui:x64-windows

echo Copying ImGui library to lib/ImGui/...
copy "installed\x64-windows\lib\imgui.lib" "..\lib\ImGui\imgui.lib"

echo ImGui built and copied successfully!
cd ..
@echo off
echo Building ImGui with static runtime using vcpkg...

cd vcpkg

echo Installing ImGui with static runtime...
.\vcpkg install imgui:x64-windows-static

echo Copying ImGui library to lib/ImGui/...
copy "installed\x64-windows-static\lib\imgui.lib" "..\lib\ImGui\imgui.lib"

echo ImGui built and copied successfully!
cd ..
@echo off

taskkill /IM Minecraft.Windows.exe /F /FI "STATUS eq RUNNING"
Minecraft.Loader.exe %1

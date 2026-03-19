@echo off

taskkill /IM Minecraft.Windows.exe /F /FI "STATUS eq RUNNING"
taskkill /IM Flarial.Launcher.exe /F /FI "STATUS eq RUNNING"

echo "Killed Minecraft and Flarial Launcher"

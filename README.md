# Sponsored by [Sear](https://sear.host)
![sear](https://cdn.sear.host/synthetixlogov2-removebg-preview.png)
# Flarial Client
Welcome to **Flarial Client**, the ultimate modded client designed specifically for Minecraft Bedrock Edition (MCBE). Flarial Client enhances your gameplay with a suite of tools and features that optimize performance, improve visual quality, and provide a smoother gaming experience.
## Support us on Ko-Fi! https://ko-fi.com/flarialmc

## Installation Options

Flarial Client is available in two forms:

### Flarial Launcher (Recommended)
- A user-friendly launcher that manages Flarial and other MCBE clients.
- Includes Discord integration for community and support.
- Offers additional Minecraft management features.

### Standalone DLL
- A minimal, lightweight version for advanced users.
- Requires an external injector to set up and run.

## Supported Platforms

- **Operating System:** Windows 10/11 (64-bit processors only)
- **Minecraft Version:** Compatible with MCBE versions 1.20 and above.

## Troubleshooting

If you encounter issues, check out our [FAQ](https://discord.gg/flarial-community-1049946152092586054) or join our community on [Discord](https://discord.gg/flarial-community-1049946152092586054). We’re here to help!

---
## Build Guide

### Prerequisites:
1. **CMake**: Ensure CMake is installed on your system. You can download it from [cmake.org](https://cmake.org/download/) and follow the installation instructions for your operating system.

2. **Git**: Install Git if you haven't already. You can download Git from [git-scm.com](https://git-scm.com/downloads) and follow the installation instructions.

### Steps to Clone and Build the Project:

1. **Clone the Repository**:
   Open a terminal or command prompt and clone the repository using Git:
   ```bash
   git clone https://github.com/flarialmc/dll.git
   ```

2. **Navigate to the Project Directory**:
   Change into the directory of the cloned repository:
   ```bash
   cd dll
   ```

3. **Create a Build Directory**:
   It's a good practice to create a separate directory for out-of-source builds. This keeps your source directory clean and allows for easier management of builds:
   ```bash
   mkdir build
   cd build
   ```

4. **Configure with CMake**:
   Run CMake to configure the build environment. Specify the path to the CMakeLists.txt file in the root of the cloned repository:
   ```bash
   cmake ..
   ```
   This command generates the necessary build files based on the CMake configuration.

5. **Build the Project**:
   Once CMake has configured the build files successfully, you can build the project using a suitable build tool (like `make` on Unix-like systems or Visual Studio on Windows):
   ```bash
   cmake --build .
   ```
   This command compiles the project according to the generated build files.

By following these steps, you should be able to clone and build the project using CMake successfully.

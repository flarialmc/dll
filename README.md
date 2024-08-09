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

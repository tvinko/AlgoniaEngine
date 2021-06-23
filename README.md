# Algonia

Seamlessly combine different programming languages
 
# Table of contents

- [Intruduction](#introduction)
  - [What is Algonia](what-is-algonia)
  - [HelloWorld](helloworld)
- [Build Instructions](#build-instructions)
  - [Windows](#windows)
  - [Linux](#linux)
  - [MacOS](#macos)

# Introduction
## What is Algonia
Algonia is Visual Microservices platform to simplify programming languages interoperability
## Hello World
You can see it in action [here](https://dev.to/tvinko/languages-interoperability-406f).


# Build Instructions
## Windows
Prerequisites:
* git
* Visual Studio Code
* .NET Core SDK
* OpenSSL
* Build Tools For Visual Studio

1. Install [git](https://git-scm.com/downloads)
2. Install [Visual Studio Code](https://code.visualstudio.com/)
After installation, Visual Studio Code will automatically detect git path. In case that path is not detected and git is installed, set path VS Code manually. Install following extensions
   - [C/C++ for Visual Studio Code](vscode:extension/ms-vscode.cpptools)
   - [C# for Visual Studio Code](vscode:extension/ms-vscode.csharp)



3. If you don’t have Visual Studio, install [Build Tools for Visual Studio 2019](https://www.visualstudio.com/downloads/)
   - After running installer, select “Visual C++ build tools”
   - Be sure to select “VC++ 2015.3 v14.00 toolset for desktop” (actual version can vary, at the time of writting instructions, VS 2017 was used)

4. Find “vcvarsall.bat” file which is needed to build C/C++ projects from VSCode. Add directory path to PATH environment variable.<br/>
Typical locations to search are “C:\Program Files (x86)\Microsoft Visual Studio\Preview\Community\VC\Auxiliary\Build\”, “C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\”....

5. Find “cl.exe” file and add directory path to PATH variable. “cl.exe” is tool that controls Microsoft C and C++ compilers and linker. <br/>
Typical locations to search are “C:\Program Files (x86)\Microsoft Visual Studio\Preview\Community\VC\Tools\MSVC\14.15.26720\bin\Hostx64\x64\”....

6. Install [OpenSSL](http://slproweb.com/products/Win32OpenSSL.html).  “Win64 OpenSSL v1.0.2o Light” should suffice

7. Install [.NET Core SDK](https://www.microsoft.com/net/download/windows)

8. Create new folder for project. Make sure that path does not contain white spaces (eg C:\Algonia)

9. Go into newly created folder and clone [AlgoniaCsNodes](https://github.com/tvinko/AlgoniaCsNodes) repo. This repo contains .NET Core Elements.<br/>
git clone  https://github.com/tvinko/AlgoniaCsNodes.git

10. Clone [AlgoniaEngine](https://github.com/tvinko/AlgoniaEngine.git) repo. This repo contains native Computing Engine and Elements<br/>
git clone  https://github.com/tvinko/AlgoniaEngine.git
 
11. Create two environment variables:
    - ZENO_ROOT pointing to unmanaged AlgoniaEngine root folder (eg C:\Algonia\AlgoniaEngine)
    - ZENO_PROJ pointing to project folder (eg C:\Algonia\ZenUnmanaged\ZenEngine\project\9024bb0d-137e-4a99-2167-7b5875404b01)<br/><br/>

13. Build projects.<br/>Open “ZenComplete.code-workspace” in VS Code and press Ctrl+Shift+B to open Build task. Build projects in following order:
      - Native - ZenCommon
      - Native Lib - ZenCoreCLR
      - Native Element - ZenCounter
      - Native Element - ZenSleep
      - Native Element - ZenStart
      - Native Element -ZenDebugWrapper
      - Native - ZenEngine
## Linux
Comming soon...
## MacOS
Comming soon...
# Oracle Java Time Machine

Oracle Java Time Machine is a Windows desktop application for downloading selected historical Java Development Kit (JDK) and Java Runtime Environment (JRE) installers through a simple graphical interface.

It is designed to make older Java versions easier to obtain without requiring an Oracle account sign-in. Download availability depends on the configured file sources and the selected Java version.

## Features

- Browse supported Java versions from Java 7 through Java 26.
- Select a JDK or, when available, a JRE package.
- Choose a destination folder for downloaded installers.
- Display download progress and status information.
- Automatically detect Python on Windows.
- Offer to install or upgrade the `gdown` Python package when it is missing.
- Use a Qt-based graphical interface with bundled styling and an application icon.

## Requirements

- Windows
- [Qt 6](https://www.qt.io/development/qt-framework) with the Core, Gui, and Widgets modules
- CMake 3.21 or newer
- A C++17-compatible compiler
- Python 3
- The Python package `gdown`
- An internet connection

The application can offer to install `gdown` automatically by running:

```bash
python -m pip install --upgrade gdown
```

If the `python` command is not available, the application also checks `python3` and the Windows `py -3` launcher.

## Build from source

### 1. Install dependencies

Install Qt 6, CMake, and a C++17-compatible compiler. The CMake configuration currently defaults to the following Qt installation path on Windows when `CMAKE_PREFIX_PATH` is not provided:

```text
C:/Qt/6.11.0/msvc2022_64
```

If Qt is installed elsewhere, pass its installation directory explicitly.

### 2. Configure the project

From the repository root, run:

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH="C:/path/to/Qt/6.x.x/msvc2022_64"
```

### 3. Build

```bash
cmake --build build --config Release
```

The executable is named `OracleJavaTimeMachine`.

On Windows, if `windeployqt` is available in the Qt installation, the build configuration attempts to deploy the required Qt DLLs after building.

## Usage

1. Launch `OracleJavaTimeMachine`.
2. Select a download destination.
3. Choose a Java version.
4. Choose `JDK` or `JRE` when that package is available.
5. Click **Start Download**.
6. Monitor the progress bar and download log.

Downloaded files are saved using names such as:

```text
Java 8_JDK.exe
Java 8_JRE.exe
```

The default destination is a `Downloads` directory next to the application executable.

## Troubleshooting

### Python is not detected

Install Python 3 from the official Python website and ensure that the Python launcher or command is available to the application. Restart the application after installation.

### `gdown` installation fails

Check your internet connection and install the package manually:

```bash
python -m pip install --upgrade gdown
```

You may also need to use `python3` or `py -3` instead of `python`.

### A download fails

Confirm that the selected source file is still available, check your network connection, verify that the destination folder is writable, and review the download log for details.

## Project structure

```text
.
├── CMakeLists.txt          # CMake build configuration
├── resources/              # Application icon, Qt resources, and stylesheet
└── src/
    ├── main.cpp            # Application entry point
    ├── mainwindow.cpp      # Main window and download workflow
    ├── mainwindow.h
    ├── environmentchecker.cpp  # Python and gdown detection/installation
    └── environmentchecker.h
```

## Legal notice

Oracle, Java, JDK, and JRE are trademarks or registered trademarks of Oracle Corporation and/or its affiliates. This project is independent and is not affiliated with, sponsored by, or endorsed by Oracle.

Only download and use software for which you have the necessary rights and permissions. Review the applicable Java license terms and the laws or policies that apply to your use of historical Java installers.

## License

No license file is currently included in this repository. Unless a license is added, the repository contents should be treated as **all rights reserved**. Please contact the repository owner before redistributing or modifying the project.

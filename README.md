# Multithreaded File Search Tool

This program performs a multithreaded search for a file across all accessible root directories of the system. It supports both Windows and Unix-like systems.

## Features
- Multithreaded recursive search across all directories.
- Windows support (via `GetLogicalDrives`) and Linux (`/etc/mtab`).
- Skips directories with permission issues.
- Thread-safe console output of found files.

## Build Instructions
On Linux/macOS:
```
g++ -std=c++17 -pthread main.cpp -o FileSearcher
```
On Windows (MSVC):
```
cl /EHsc /std:c++17 main.cpp
```

## Usage
1. Run the program.
2. Enter the full or partial filename you want to search for.
3. The program will print all matching files with their full paths.

## Example Output
```
Enter the filename to search for: test
Found test.txt at: C:\Users\User\Documents\test.txt
Search finished.
```

## Notes
- The program spawns a new thread for each subdirectory, so be cautious on file systems with very deep hierarchies.

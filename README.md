# LumithyCLI

LumithyCLI is a lightweight Command-Line Interface (CLI) utility written in C++17, designed to centralize and automate your workflow by mapping custom aliases to directory paths and executable files.

## Features
- Quick Open (-o): Instantly open your favorite folders in Windows Explorer.
- Smart Launch (-l): Start applications (Minecraft, IDEs, tools) using simple keywords.
- Dynamic Configuration: Managed via a standard config.ini file.
- System Logging: Activity history tracked in history.log.
- ANSI Support: Integrated support for colored terminal output.

## Installation and Setup

### 1. File Placement
1. Create a folder at C:\LumithyCLI.
2. Place the lumithy.exe binary and your config.ini file inside this folder.

### 2. Add to System PATH
To call lumithy from any terminal window:
1. Search for "Edit the system environment variables" in Windows or tips `Win + R` and put *`sysdm.cpl`*.
2. Click Environment Variables.
3. Under System variables, select Path and click Edit.
4. Click New and add C:\LumithyCLI.
5. Restart your terminal (CMD or PowerShell).

### 3. config.ini Structure
The file must be saved in ANSI encoding and structured as follows:
```ini
[open]
root=C:\
windows=C:\Windows
system32=C:\Windows\System32
programmes=C:\Program Files
temp=C:\Windows\Temp
[launch]
notepad=C:\Windows\System32\notepad.exe
cmd=C:\Windows\System32\cmd.exe
write=C:\Windows\System32\write.exe
calc=C:\Windows\System32\calc.exe
```

## Usage Guide

| Command | Description | Example |
| :--- | :--- | :--- |
| lumithy -h | Displays the help menu and command list | lumithy -h |
| lumithy -o <alias> | Opens the associated directory in Explorer | lumithy -o ng |
| lumithy -l <alias> | Launches the configured application | lumithy -l mc |
| lumithy -c | Opens the config file directly in Notepad | lumithy -c |
| lumithy -r | Reloads the configuration file | lumithy -r |

## Technical Specifications
- Language: C++17 / Win32 API.
- Platform: Windows 10/11.
- Architecture: Uses GetModuleFileNameA for absolute path resolution, ensuring stability even when running as Administrator.

## License and Credits
- Developer: Luka Vanibels
- License: MIT
- Status: v1.0 (Stable)

## Troubleshooting
If you encounter a Read Error (Code 2), ensure your config.ini is encoded in ANSI rather than UTF-8. Legacy Windows API functions used for INI parsing may fail if a Byte Order Mark (BOM) is detected.
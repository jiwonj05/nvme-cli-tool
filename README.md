# NVMe CLI Tool

A lightweight C++ utility to retrieve NVMe drive identification (Model and Serial Number) on Windows using the Storage Query Property API.

### 🚀 Features
* **Direct Hardware Access:** Reads NVMe Identify Controller data via Windows IOCTL.
* **High Compatibility:** Uses `IOCTL_STORAGE_QUERY_PROPERTY` to avoid common driver errors (like Error 317) associated with raw pass-through commands.
* **Zero Dependencies:** Native C++ implementation using standard Windows APIs.

### 📋 Prerequisites
* Windows 10/11
* **Administrator Privileges** (Required for `PhysicalDrive` access)
* An NVMe drive installed at `\\.\PhysicalDrive0`

### 🛠️ Build & Usage

**Build (Visual Studio)**
1. Create a C++ Console App.
2. Add source code and build for **x64**.

**Run**
You **must** run the executable as Administrator.
```cmd
:: Open CMD as Administrator
nvme-cli-tool.exe

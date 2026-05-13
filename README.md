# Online Shopping System

Our project is an online shopping system which utilizes the abilities of C++ language while abiding by the rules and regulations of
OOP. The project is a console application which primarily consists of two pieces of code:

1. The networking layer.
2. The management layer.
 
## Dependencies

The project has used the windows api of threading and socket programming - `Winsock2` library. Therefore, the project can only be compiled to windows machines that support this library. Also, make sure to link the object file with winsock modules.

The networking layer also uses the local host `127.0.0.1` ip as default. You can change it in the [Server.h](./Server/Headers/Server.h)

## Build, run, tests, and coverage (Windows)

This repository includes a CMake + GoogleTest setup that can:

- Build the project libraries used by tests
- Run all unit tests
- Generate code coverage reports using `gcovr` (GCC/MinGW)

### Prerequisites

Install:

- **Git**
- **MSYS2 MinGW-w64 (recommended)** with:
  - `g++` / `gcc`
  - `cmake`
  - `ninja`
  - `gcov` (comes with GCC)
- **Python 3** (for `gcovr`)

If you use MSYS2, you can install the toolchain from an **MSYS2 MinGW64** shell:

```bash
pacman -S --needed mingw-w64-x86_64-toolchain mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja
```

Install `gcovr`:

```powershell
python -m pip install --user --upgrade gcovr
```

### Configure and build (Ninja)

From the repo root:

```powershell
cmake -S . -B build -G Ninja
cmake --build build -j
```

### Run unit tests

```powershell
ctest --test-dir build --output-on-failure
```

### Build and run with coverage + generate reports

Coverage build:

```powershell
cmake -S . -B build-coverage -G Ninja -DENABLE_COVERAGE=ON
cmake --build build-coverage -j
ctest --test-dir build-coverage --output-on-failure
```

Generate reports (important: use forward slashes for `gcov.exe` on Windows):

```powershell
$common = @(
  '-r','.',
  '--gcov-executable','C:/msys64/mingw64/bin/gcov.exe',
  '--object-directory','build-coverage',
  '--filter','Server/Headers',
  '--exclude','tests'
)

python -m gcovr @common --print-summary --txt build-coverage\coverage.txt
python -m gcovr @common --xml-pretty -o build-coverage\coverage.xml
python -m gcovr @common --html-details build-coverage\coverage.html
```

Outputs:

- `build-coverage/coverage.txt`
- `build-coverage/coverage.xml`
- `build-coverage/coverage.html`

### Running the actual client/server program (legacy build)

The original project includes `Server/serverProgram.cpp` and `Client/clientProgram.cpp`.
These use legacy `myHeader.h` compilation style (including `*.cpp` files).

If you want, you can compile them with MinGW directly (no CMake):

```powershell
g++ -std=c++17 -O2 -I .\Server .\Server\serverProgram.cpp -lws2_32 -o server.exe
g++ -std=c++17 -O2 -I .\Client .\Client\clientProgram.cpp -lws2_32 -o client.exe
```

Then run `server.exe` in one terminal and `client.exe` in another.

## Networking layer

The networking layer consists of those classes and functions which
are responsible for establishing a connection between a client and
the remote sever.

### Technology

The networking is carried out with the help of Winsock (2.2) programming - windows APIs of socket programming.

### Classes

The `Server` - which uses the `Thread` class - and `Client` class.

![networking - classes](./images/network_classes.jpg)

### Usage

The networking layer is independent of the management layer; it can used in any C++ program. You can download the following files and use them in your own project.

- Server: [Server.h](./Server/Headers/Server.h) - [Server.cpp](./Server/Headers/Server.cpp) and [Thread.h](./Server/Headers/Thread.h) - [Thread.cpp](./Server/Headers/Thread.cpp)
- Client: [Client.h](./Client/Headers/Client.h) - [Client.cpp](./Client/Headers/Client.cpp)

You may utilize the networking functions like:

#### Server:

```cpp

#include "Server.h"

void worker(Thread& server)
{
    // code to deal with client
    server.Rec(buffer);     // receive a msg from client
    server.Send(msg);       // send a msg to client
    server.endServer();     // close the server
}

int main()
{
    Server server(worker);
    if (server.start())
        server.run();

    return 0;
}
```


#### Client:

```cpp

#include "Client.h"

int main()
{
    Client client;
    if (client.start())
    {
        // code to deal with server
        client.Rec(buffer);     // receive a msg from server
        client.Send(msg);       // send a msg to server
    }
        
    return 0;
}
```

### Working

![networking - working](./images/network_working.jpg)

## Management layer

This layer uses the networking layer to implement a networked shopping app.

### Features

- Customers can sign-up or login.
- Customers can buy items.
- Before confirming, the bill is displayed. Then, they have the option to confirm, cancel, add or remove anything.
- Customers can write complaints.
- Any employee (or admin) can answer the complain.
- Employees and admin will avail 20% discount on purchases.
- Employee accounts are made by the admin.
- Customers and employees alike can view their profile.
- Automatic reorder is placed if goods fall below a certain level.
- Admin can view record of customers & employees.
- Admin can fire any employees and ban any customers.
- Admin can view the inventory level and manually order goods.
- Admin can also manually update balance.
- Admin can search for customers and employees.
- Admin also sets the reorder level, amount & unit price.

### Classes

![management - classes](./images/manage_classes.jpg)

## Documentation

For a detailed explanation of everything, refer to this [report.](./REPORT.pdf)



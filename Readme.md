# Boost Intra-process communication

This repo contains an example to use intraprocess communication using Boost.
Tested on Linux systems.

## Requirements

- OpenCV library

- Boost library:
```bash
sudo apt install libboost-dev libboost-system-dev libboost-filesystem-dev
```
## Usage

Build using cmake:
```bash
mkdir build && cd build && cmake .. && make
```

You will have two executables: *main_1* and *main_2*.
- **main_1** will open a camera, convert the stream in JPG and copy it to a shared memory object.
- **main_2** will open the shared memory object, retrieve the image and display it.

Open two terminals, run and fun.
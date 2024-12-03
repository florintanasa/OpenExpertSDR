# OpenExpertSDR

OpenExpertSDR is a software-defined radio (SDR) project Based on [https://github.com/Tsukihime/OpenExpertSDR](https://github.com/Tsukihime/OpenExpertSDR) that integrates with various SDR hardware, including Genesis G59/G11 and virtual serial port communication via `tty0tty`.

## Prerequisites

Before compiling, ensure you have the following installed:

- Qt 5 or Qt 4
- MinGW (for Windows) - [Download MinGW](http://www.mingw.org/)
- CMake (for Qt5 builds)

## Clone the Repository

To get started, clone the repository:

```bash
git clone https://github.com/florintanasa/OpenExpertSDR
```
```

## Compilation

### Option 1: Using QtCreator

1. Open `OpenExpertSDR` in QtCreator.
2. Configure the project with Qt5 or Qt4.
3. Set up MinGW (for Windows).

### Option 2: Using the Command Line

After cloning the repository, navigate to the project directory and use `qmake` or `cmake` to build the project.

#### Using `qmake` (for Qt4 or Qt5)

```bash
cd OpenExpertSDR
qmake *.pro
make
```

#### Using `cmake` (for Qt5)

```bash
cd OpenExpertSDR
mkdir build
cd build
cmake ..
make
```

## External Dependencies

### libDttSP

For the `libDttSP.so` library, you can clone it from the following repository:

```bash
git clone https://github.com/wd8rde/libDttSP
```

### libExtio_genesis

If you're using SDR hardware from Genesis (G59 or G11), you will also need `libExtio_genesis`:

1. Clone the repository:

```bash
git clone https://github.com/wd8rde/libExtio_genesis
```

2. In the `OpenExpertSDR/build` directory, create a `device` directory and create a symbolic link to the `libextio_genesis.so` library:

```bash
cd OpenExpertSDR/build
mkdir device
cd device
ln -s /usr/local/lib/libextio_genesis.so libextio_genesis.so
```

## Virtual Serial Port Support (CAT)

The project includes support for virtual serial ports using the `qextserial` library (`tnt*`), which is required for CAT (Computer Aided Transceiver) functionality.

### Installation

To set up `tty0tty` for virtual serial ports:

1. Clone the `tty0tty` repository:

```bash
git clone https://github.com/freemed/tty0tty
```

2. Navigate to the `module` directory and compile the kernel module:

```bash
cd tty0tty/module
make
```

3. Install the kernel module:

```bash
sudo cp tty0tty.ko /lib/modules/your_kernel/kernel/drivers/tty/serial/
sudo depmod -a
sudo modprobe tty0tty
```

4. Create a udev rule to manage the virtual serial ports:

```bash
sudo cp /lib/udev/rules.d/98-udev-tnt.rules /lib/udev/rules.d/
```

This rule ensures the proper group assignment for the virtual serial port (`tnt*`).

5. Add your username to the `dialout` group:

```bash
sudo gpasswd -a user_name dialout
```

If the `dialout` group does not exist, create it with:

```bash
sudo groupadd dialout
```

6. To automatically load the `tty0tty` module at startup, edit the `/etc/modules` file and add:

```
tty0tty
```

## Testing

For testing virtual serial port communication, you can use the [CoolTerm](http://freeware.the-meiers.org/) terminal emulator.

---

This guide should help you get OpenExpertSDR running with support for Genesis SDR devices and virtual serial port communication.

```

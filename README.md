# OpenExpertSDR

git clone https://github.com/florintanasa/OpenExpertSDR

To compile use:

 QtCreator with qt5 or qt4 and for windows use mingw from http://www.mingw.org/ not from qt.

Or from console after clone

```
cd OpenExpertSDR
qmake *.pro
make
# or use cmake only for qt5
cd OpenExpertSDR
mkdir build
cd build
cmake ..
make
```

For libDttSP.so use this link https://github.com/wd8rde/libDttSP

For who have SDR from Genesis G59 or G11 https://github.com/wd8rde/libExtio_genesis, then in OpenExpertSDR/build directory:
```
cd OpenExpertSDR/build
mkdir device
cd device
ln -s /usr/local/lib/libextio_genesis.so libextio_genesis.so
```

I added posibility to load at list virtual serial port at qextserial tnt* to be used with CAT

Module was take it from here https://github.com/freemed/tty0tty

To install:

git clone https://github.com/freemed/tty0tty

cd tty0tty/module

make

sudo cp tty0tty.ko /lib/modules/your kernel/kernel/drivers/tty/serial/

sudo depmod -a

sudo modprobe tty0tty

I create this file /lib/udev/rules.d/98-udev-tnt.rules with this rule

KERNEL=="tnt[0-9]*", GROUP="dialout"

Add your user name at dialout group

sudo gpasswd -a user_name dialout

if group dialout not exist create with

sudo groupadd dialout

Edit file /etc/modules and at the end add this line 

tty0tty

Now modules is taken at every statup.

For test I used this CoolTerm http://freeware.the-meiers.org/

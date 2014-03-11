Package Generation
==================

This directory creates a debian package of the stm32f1 or f4 CMSIS and StdPeriph libraries
and headers. This is useful for a few reasons:

 - You can use `sudo dpkg -i` to install a deb and avoid manually compiling
   and installing the library.
 - You can place the deb on an apt repository, and make it trivially easy to
   set up a development environment on any Ubuntu or Debian machine---this is
   great for getting a whole team up and running quickly, or for throwaway
   environments such as CI builders and virtual machines.

Toolchain
---------

You need the right GCC compiler, which is `gcc-arm-none-eabi`. Fortunately, if you're on
Ubuntu, this is really easy to get from Launchpad, thanks to Terry Guo's PPA:

~~~
sudo add-apt-repository ppa:terry.guo/gcc-arm-embedded
sudo apt-get update
sudo apt-get install gcc-arm-none-eabi
~~~

Building the deb
----------------

You'll need a [fpm](https://github.com/jordansissel/fpm), which is what actually produces
the deb package from a folder of files. This comes from rubygems:

~~~
sudo apt-get install rubygems
gem install fpm
~~~

Apart from that, it's a regular build:

~~~
mkdir packages/build
cd packages/build
cmake .. -DSTM32_FAMILY=F4
make
~~~

Enjoy your deb!

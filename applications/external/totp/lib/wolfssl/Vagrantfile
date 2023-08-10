# -*- mode: ruby -*-
# vi: set ft=ruby :

$setup = <<SCRIPT


apt-get update
apt-get install -y git autoconf libtool make valgrind libpq-dev


URL=https://github.com/libevent/libevent/releases/download/release
LIB=libevent
VER=2.0.22-stable

wget -q $URL-$VER/$LIB-$VER.tar.gz && tar -zxf $LIB-$VER.tar.gz
cd $LIB-$VER/ && ./autogen.sh && ./configure -q && make -s
sudo make install && cd .. && rm -rf $LIB-$VER*


DST=wolfssl

cp -rp /vagrant/ $DST/
chown -hR vagrant:vagrant $DST/

echo "cd $DST"                                         >> .bashrc
echo "read -p 'Sync $DST? (y/n) ' -n 1 -r"             >> .bashrc
echo "echo # new line"                                 >> .bashrc
echo 'if [[ \$REPLY =~ ^[Yy]$ ]]; then'                >> .bashrc
echo "    echo -e '\e[0;32mRunning $DST sync\e[0m'"    >> .bashrc
echo "    ./pull_to_vagrant.sh"                        >> .bashrc
echo "fi"                                              >> .bashrc


SCRIPT

VAGRANTFILE_API_VERSION = "2"

Vagrant.configure(VAGRANTFILE_API_VERSION) do |config|
  config.vm.box = "ubuntu/trusty64"
  config.vm.provision "shell", inline: $setup
  config.vm.network "forwarded_port", guest: 11111, host: 33333

end

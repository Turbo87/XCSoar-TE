# -*- mode: ruby -*-
# vi: set ft=ruby :

$script = <<SCRIPT

cat >> ~/.profile << EOF
export LANG=C
export LC_CTYPE=C
EOF

sudo bash -c "cat > /etc/apt/sources.list" << EOF
deb mirror://mirrors.ubuntu.com/mirrors.txt precise main restricted universe multiverse
deb mirror://mirrors.ubuntu.com/mirrors.txt precise-updates main restricted universe multiverse
deb mirror://mirrors.ubuntu.com/mirrors.txt precise-backports main restricted universe multiverse
deb mirror://mirrors.ubuntu.com/mirrors.txt precise-security main restricted universe multiverse
EOF

# set ANDROID variables
export ANDROID_API_LEVEL=17
export ANDROID_SDK_VERSION=24.0.2
export ANDROID_NDK_VERSION=9
export ANDROID_BUILD_TOOLS_VERSION=20.0.0

# update Apt repository
sudo apt-get update

# install base dependencies
sudo apt-get install -y make g++ libsdl-image1.2-dev libboost-dev zlib1g-dev \
    libsdl1.2-dev libfreetype6-dev libpng-dev libjpeg-dev \
    libcurl4-openssl-dev libxml-parser-perl librsvg2-bin xsltproc \
    imagemagick gettext ttf-dejavu ccache zip git

mkdir -p ~/opt

# install PC/WIN64 dependencies
sudo apt-get install -y binutils-mingw-w64-i686 gcc-mingw-w64-i686 g++-mingw-w64-i686
sudo apt-get install -y binutils-mingw-w64-x86-64 gcc-mingw-w64-x86-64 g++-mingw-w64-x86-64

# install ANDROID dependencies (incl. 64bit support)
sudo apt-get install -y openjdk-7-jre-headless openjdk-7-jdk ant vorbis-tools
sudo apt-get install -y libncurses5:i386 libstdc++6:i386 zlib1g:i386

wget -N -nv http://dl.google.com/android/android-sdk_r$ANDROID_SDK_VERSION-linux.tgz -P /vagrant/tmp
wget -N -nv http://dl.google.com/android/ndk/android-ndk-r$ANDROID_NDK_VERSION-linux-x86_64.tar.bz2 -P /vagrant/tmp

tar -xf /vagrant/tmp/android-sdk_r$ANDROID_SDK_VERSION-linux.tgz -C ~/opt
tar -xf /vagrant/tmp/android-ndk-r$ANDROID_NDK_VERSION-linux-x86_64.tar.bz2 -C ~/opt

echo 'y' | ~/opt/android-sdk-linux/tools/android update sdk --no-ui --all --filter platform-tools
echo 'y' | ~/opt/android-sdk-linux/tools/android update sdk --no-ui --all --filter build-tools-$ANDROID_BUILD_TOOLS_VERSION,android-$ANDROID_API_LEVEL

# install Windows Mobile dependencies
wget -N -nv http://spekje.snt.utwente.nl/~roel/cegcc/mingw32ce-mk-2013-04-03-amd64.tar.xz -P /vagrant/tmp
tar -xf /vagrant/tmp/mingw32ce-mk-2013-04-03-amd64.tar.xz -C ~/opt
echo "PATH=\"\$HOME/opt/mingw32ce-mk-2013-04-03-amd64/bin:\$PATH\"" >> ~/.profile

# install Manual dependencies
sudo apt-get install --no-install-recommends -y \
    texlive texlive-latex-extra latex-xcolor pgf liblocale-po-perl \
    texlive-lang-ukenglish texlive-lang-german texlive-lang-french

SCRIPT

Vagrant.configure("2") do |config|
    config.vm.box = "ubuntu-12.04.2-x86_64"
    config.vm.box_url = "http://puppet-vagrant-boxes.puppetlabs.com/ubuntu-server-12042-x64-vbox4210.box"

    config.vm.network "private_network", type: "dhcp"
    config.vm.synced_folder ".", "/vagrant", type: "nfs"

    config.vm.provider "virtualbox" do |v|
        host = RbConfig::CONFIG['host_os']

        # Give VM 1/4 system memory & access to all cpu cores on the host
        if host =~ /darwin/
            cpus = `sysctl -n hw.ncpu`.to_i
            # sysctl returns Bytes and we need to convert to MB
            mem = `sysctl -n hw.memsize`.to_i / 1024 / 1024 / 4

        elsif host =~ /linux/
            cpus = `nproc`.to_i
            # meminfo shows KB and we need to convert to MB
            mem = `grep 'MemTotal' /proc/meminfo | sed -e 's/MemTotal://' -e 's/ kB//'`.to_i / 1024 / 4

        else
            cpus = 2
            mem = 1024
        end

        v.customize ["modifyvm", :id, "--memory", mem]
        v.customize ["modifyvm", :id, "--cpus", cpus]
    end

    config.vm.provision "shell", inline: $script, privileged: false
end

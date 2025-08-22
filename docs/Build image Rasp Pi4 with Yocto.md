# **BUILD IMAGE FOR RASPBERRY PI 4**

## Prerequisistes 
- OS: Ubuntu 18.04 or later.
- Device: Raspberry Pi 4
- Flash card: MicroSD 16GB
- Packages :

```bash
  sudo apt update
  sudo apt install gawk wget git diffstat unzip texinfo gcc build-essential chrpath socat \
  cpio python3 python3-pip python3-pexpect xz-utils debianutils iputils-ping \
  python3-git python3-jinja2 python3-subunit zstd liblz4-tool file locales libacl1
```
## 1. Set up environment Yocto & clone Meta-layer
Download layer for own Raspberry Pi

```bash
git clone git://git.yoctoproject.org/meta-raspberrypi -b dunfell
```

Download layer **meta-openembedded**

```bash
git clone git://git.openembedded.org/meta-openembedded -b dunfell
```

Download layer **meta-qt5** for QT Creator

```bash
git clone https://github.com/meta-qt5/meta-qt5.git -b dunfell
```

## 2. Source Environment and Create Build Directory

```bash
source oe-init-build-env
```

After that, a folder `build/` will create
## 3. Config Build
### 3.1. Modify `conf/local.conf/`

Open file `conf/local.conf/` and modify MACHINE suit with your device

```bash
# MACHINE ??= "qemux86-64"
MACHINE = "raspberrypi4"
```

And you can add some necessary packages while create image

```bash
BB_NUMBER_THREADS = "2"
PARALLEL_MAKE = "-j2"

IMAGE_INSTALL_append = " cluster qtbase qtdeclarative qtquickcontrols2 qtquickcontrols2-qmlplugins qtdeclarative-qmlplugins qtgraphicaleffects qtquickcontrols"

IMAGE_INSTALL_append = " can-utils apt apt-utils gnupg dpkg iproute2"

```

### 3.2. Add meta-layer to `bblayers.conf`

Open file `conf/bblayers.conf/` and update  `BBLAYERS` to add layer was cloned

```bash
BLAYERS ?= " \
  /home/bocdo/yocto/poky/meta \
  /home/bocdo/yocto/poky/meta-poky \
  /home/bocdo/yocto/poky/meta-yocto-bsp \
  /home/bocdo/yocto/poky/meta-openembedded/meta-oe \
  /home/bocdo/yocto/poky/meta-qt5 \
  /home/bocdo/yocto/poky/meta-raspberrypi \
  /home/bocdo/yocto/poky/build/workspace \
  "
```

## 4. Build image 
Build image in Yocto you can use `bitbake` such as: 

```bash
bitbake core-image-sato 
```
Or:

```bash
bitbake core-image-minimal
```

*This process may take up to 30 minutes depending on your computer configuration.*

While Image creating, in your screen will had response :
```
bitbake core-image-sato
Loading cache: 100% |###################################################################################################################################################| Time: 0:00:02
Loaded 2440 entries from dependency cache.
Parsing recipes: 100% |#################################################################################################################################################| Time: 0:00:01
Parsing of 1644 .bb files complete (1643 cached, 1 parsed). 2441 targets, 119 skipped, 0 masked, 0 errors.
NOTE: Resolving any missing task queue dependencies

Build Configuration:
BB_VERSION           = "1.46.0"
BUILD_SYS            = "x86_64-linux"
NATIVELSBSTRING      = "universal"
TARGET_SYS           = "arm-poky-linux-gnueabi"
MACHINE              = "raspberrypi4"
DISTRO               = "poky"
DISTRO_VERSION       = "3.1.33"
TUNE_FEATURES        = "arm vfp cortexa7 neon vfpv4 thumb callconvention-hard"
TARGET_FPU           = "hard"
meta                 
meta-poky            
meta-yocto-bsp       = "dunfell:63d05fc061006bf1a88630d6d91cdc76ea33fbf2"
meta-oe              = "dunfell:01358b6d705071cc0ac5aefa7670ab235709729a"
meta-qt5             = "dunfell:5ef3a0ffd3324937252790266e2b2e64d33ef34f"
meta-raspberrypi     = "dunfell:2081e1bb9a44025db7297bfd5d024977d42191ed"
workspace            = "dunfell:63d05fc061006bf1a88630d6d91cdc76ea33fbf2"

WARNING: /home/bocdo/yocto/poky/meta/recipes-devtools/binutils/binutils_2.34.bb:do_compile is tainted from a forced run                                                 | ETA:  0:00:05
Initialising tasks: 100% |##############################################################################################################################################| Time: 0:00:15
Sstate summary: Wanted 10 Found 9 Missed 1 Current 2931 (90% match, 99% complete)
NOTE: Executing Tasks
NOTE: Tasks Summary: Attempted 7273 tasks of which 7273 didn't need to be rerun and all succeeded.

Summary: There was 1 WARNING message shown.

```

## 5. Flash image
When the process Build your image completed. Total files will save in: `/yocto/poky/build/tmp/deploy/images/raspberrypi4$ ` and you can `ls` in this posision for preview total files.

when you use: `ls` you will some file like:

```
bcm2711-rpi-4-b.dtb
bcm2711-rpi-4-b-raspberrypi4.dtb
bootfiles
core-image-sato.env
core-image-sato-raspberrypi4-20250807144818.rootfs.ext3
core-image-sato-raspberrypi4-20250807144818.rootfs.manifest
core-image-sato-raspberrypi4-20250807144818.rootfs.tar.bz2
core-image-sato-raspberrypi4-20250807144818.rootfs.wic.bmap
core-image-sato-raspberrypi4-20250807144818.rootfs.wic.bz2
core-image-sato-raspberrypi4-20250807144818.testdata.json

```

*This is all necessary files you can check the process of build image success or not*

Identify memory card device

```bash
lsblk
```

Flash image to your device

```bash
sudo bzcat core-image-sato-raspberrypi4-20250807144818.rootfs.wic.bz2 | sudo dd of=/dev/sdb bs=4M status=progress conv=fsync
```

**Notes:** Make sure /dev/sdb is your correct memory card device!

## 6 Boot SDcard and start your device

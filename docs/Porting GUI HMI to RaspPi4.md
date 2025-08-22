# **PORTING GUI HMI TO RAPSBERRY PI 4**

## Prerequisistes 
- OS: Ubuntu 18.04 or later:
- Had a source code qt/qml on GitHub
- Using recipe in yocto to porting

## 1. Clone source code from github.

```bash
cd yocto/poky
source oe-init-build-env
devtool add cluster-display https://github.com/BocD-210/Porting-.git --srcbranch main
```

After running, a new recipe will be created in the `poky/build/workspace/recipes/....` folder. The source code content will be saved in the `workspace/sources` folder:
```bash
~/yocto/poky/build/workspace$ tree -L 1 workspace/
workspace/
├── appends     # Save changes when modifying or upgrading
├── conf        # Config file
├── README
├── recipes     # New recipe
└── sources     # Source code

```

## 2. Modify file.bb

```bash
cd ~/yocto/poky/build/workspace/recipes/cluster-display$ ls
```
You will see file `cluster-display_git.bb` -> open this file and modify suit with your project.

```bash
# Recipe created by recipetool
# This is the basis of a recipe and may need further editing in order to be fully functional.
# (Feel free to remove these comments when editing.)

# Unable to find any files that looked like license statements. Check the accompanying
# documentation and source headers and set LICENSE and LIC_FILES_CHKSUM accordingly.
#
# NOTE: LICENSE is being set to "CLOSED" to allow you to at least start building - if
# this is not accurate with respect to the licensing of the software being built (it
# will not be in most cases) you must specify the correct value before using this
# recipe for anything other than initial testing/development!
LICENSE = "CLOSED"
LIC_FILES_CHKSUM = ""

SRC_URI = "git://github.com/BocD-210/Porting-.git;protocol=https;branch=main"

# Modify these as desired
PV = "1.0+git${SRCPV}"
SRCREV = "b4165dec39412c0f366ad5a2af7b3cd04422356e"

S = "${WORKDIR}/git"

RDEPENDS:${PN} += "qtdeclarative-qmlplugins qtquickcontrols2-qmlplugins qtgraphicaleffects-qmlplugins"
DEPENDS = "qtbase qtdeclarative qtquickcontrols2 qtgraphicaleffects qtquickcontrols"

RDEPENDS:${PN} += "qtsvg"
DEPENDS += "qtsvg"


inherit qmake5

FILES:${PN} += " /opt/Cluster/bin/Cluster "


do_install() {
    install -d ${D}${bindir}
    install -m 0755 Cluster ${D}${bindir}
    
    # Install QML files
    install -d ${D}${datadir}/${BPN}
    install -m 0644 ${S}/*.qml ${D}${datadir}/${BPN}/
    
    # Install assetss
    install -d ${D}${datadir}/${BPN}/assets
    install -m 0644 ${S}/assets/*.svg ${D}${datadir}/${BPN}/assets/
	install -m 0644 ${S}/assets/*.png ${D}${datadir}/${BPN}/assets/
    
    # Install resource file
    install -m 0644 ${S}/*.qrc ${D}${datadir}/${BPN}/
}

FILES:${PN} += " \
    ${datadir}/${BPN} \
    ${datadir}/${BPN}/* \
"
```
After that, you need rebuild your image to install new recipe.
## 3. Rebuild the image

```bash
cd yocto/poky
source oe-init-build-env
bitbake core-image-sato
```
when it completed, you can flash this image to your SD Card and start your device to check GUI QT/QML
**Notes:** Make sure /dev/sdb is your correct memory card device!

# Maintainer: Jeff Baitis <jeff@baitis.net>
pkgname=openexpertsdr
pkgver=3479960
pkgrel=1
pkgdesc="Software defined radio GUI supporting the SunSDR, Genesis, and other IQ direct sampling hardware"
arch=('i686' 'x86_64')
url="http://eesdr.com"
license=('GPL-2')
groups=('')
depends=(
	'alsa-lib'
	'bzip2'
	'db'
	'expat'
	'freetype2'
	'gcc-libs'
	'glib2'
	'glibc'
	'glu'
	'graphite'
	'harfbuzz'
	'icu'
	'jack'
	'libcap'
	'libdrm'
	'libffi'
	'libgcrypt'
	'libgpg-error'
	'libpng'
	'libsystemd'
	'libx11'
	'libxau'
	'libxcb'
	'libxdamage'
	'libxdmcp'
	'libxext'
	'libxfixes'
	'libxshmfence'
	'libxxf86vm'
	'lz4'
	'mesa'
	'pcre'
	'portaudio'
	'qt5-base'
	'xz'
	'zlib'
)
makedepends=(
	'git' 
	'cmake'
	'libDttSP-git'
) # 'bzr', 'git', 'mercurial' or 'subversion'
provides=("${pkgname}-git")
conflicts=("${pkgname}-git")
source=("${pkgname}::git://github.com/florintanasa/OpenExpertSDR")
md5sums=('SKIP')

# Please refer to the 'USING VCS SOURCES' section of the PKGBUILD man page for
# a description of each element in the source array.

pkgver() {
	pkgver_git
}

pkgver_git() {
    cd "${srcdir}/${pkgname}"
    local ver="$(git show | grep commit | awk '{print $2}' )"
    echo ${ver:0:7}
}

build() {
	cd "$srcdir/${pkgname}/"
	mkdir build
	cd build
	cmake ..
	make
}

package() {
	cd "$srcdir/${pkgname}/build/"
	make DESTDIR="$pkgdir/" install
}

# Maintainer: Kristupas Pranckietis <kristupaspranc@gmail.com>

pkgname='vocabulary'
pkgver=0.1.0
pkgrel=1
pkgdesc="notebook to write down new words and practice using them"
arch=('x86_64')
url='https://github.com/kristupaspranc/vocabulary'
license=('MIT')
depends=('sqlite' 'ncurses')
makedepends=('cmake' 'gcc')
checkdepends=() # See if check function is needed
source=("$pkgname-$pkgver.tar.gz")
sha256sums=('SKIP')
# validpgpkeys=()

#build() {
	#cd "$pkgname-$pkgver"
	#./configure --prefix=/usr
	#make
#}

#check() {
	#cd "$pkgname-$pkgver"
	#make -k check
#}

#package() {
	#cd "$pkgname-$pkgver"
	#make DESTDIR="$pkgdir/" install
#}

build() {
  cd "$srcdir/$pkgname-$pkgver"
  cmake -S . -B build -DCMAKE_INSTALL_PREFIX=/usr
  cmake --build build
}

package() {
  cd "$srcdir/$pkgname-$pkgver"
  cmake --install build --prefix=/usr --strip
}

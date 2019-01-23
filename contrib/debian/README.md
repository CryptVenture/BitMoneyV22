
Debian
====================
This directory contains files used to package BitMoneyd/BitMoney-qt
for Debian-based Linux systems. If you compile BitMoneyd/BitMoney-qt yourself, there are some useful files here.

## BitMoney: URI support ##


BitMoney-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install BitMoney-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your BitMoneyqt binary to `/usr/bin`
and the `../../share/pixmaps/BitMoney128.png` to `/usr/share/pixmaps`

BitMoney-qt.protocol (KDE)


Nano-BMP: A minimal BMP library
=
Written because I got curious about loading and working with a basic file format and wanted to go through
the process of creating and cross-platform distributable library. The last bit isn't quite done, I still need
to make a find_package script for cmake. It is possible to get it to work without a find_package by copying/linking
the library from your `/usr/local/lib` to somewhere in your path, or adding that to directory your path. On Windows
and Mac it seems to work alright.

What's it do? Read/Write/Create/Convert 24 or 32bpp BI_RGB(X) bitmaps with the BITMAPINFOHEADER (no color palette support).

What doesn't it do? Everything else.


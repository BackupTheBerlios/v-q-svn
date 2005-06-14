#!/bin/sh

(cd ../../../ && doxygen doxygen.cfg)
(cd ../../../contrib/vqwww && doxygen doxygen.cfg)
(cd ../../../contrib/freemail && doxygen doxygen.cfg)
(cd ../../../contrib/hosting && doxygen doxygen.cfg)

find -type f -exec chmod 644 {} \; -o -type d -exec chmod 755 {} \;

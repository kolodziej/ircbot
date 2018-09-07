#!/bin/sh
set -x
set -e

if [ "${IRCBOT_VERSION}x" = "x" ]; then
    echo "IRCBOT_VERSION env must be set!"
    exit 1
fi

ARCHIVE_NAME="ircbot_${IRCBOT_VERSION}.orig.tar.gz"

tar czf ${ARCHIVE_NAME} cmake CMakeLists.txt config gtest include packaging \
        plugins program *.md source tests docs protobuf pybind11 pyircbot

cp ${ARCHIVE_NAME} packaging/deb

cd packaging/deb
mkdir ircbot-${IRCBOT_VERSION}
cd ircbot-${IRCBOT_VERSION}

tar xf ../${ARCHIVE_NAME}
cp -r ../debian .

export DEB_BUILD_OPTIONS="parallel=$(nproc)"
debuild -uc -us
cd ../../..

mkdir -p packages/debian
cp packaging/deb/*.deb packages/debian

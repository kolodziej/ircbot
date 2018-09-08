#!/bin/sh
set -x
set -e

TARGET_OS=$1

if [ "${IRCBOT_VERSION}x" = "x" ]; then
    echo "IRCBOT_VERSION env must be set!"
    exit 1
fi

if [ "${TARGET_OS}x" = "x" ]; then
    echo "TARGET_OS (first arg) must be set!"
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
debuild -uc -us --set-envvar GIT_COMMIT=${GIT_COMMIT} --set-envvar GIT_REF=${GIT_REF}
cd ../../..

mkdir -p packages/${TARGET_OS}
cp packaging/deb/*.deb packages/${TARGET_OS}

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

mkdir -p rpmbuild/BUILD
cp -r cmake CMakeLists.txt config gtest include packaging \
      plugins program *.md source tests docs protobuf pybind11 pyircbot \
      rpmbuild/BUILD/

rpmbuild --define "_topdir $(realpath ./rpmbuild)" \
    --define "GIT_COMMIT ${GIT_COMMIT}" \
    --define "GIT_REF ${GIT_REF}" \
    -ba packaging/rpm/ircbot.spec

mkdir -p packages/${TARGET_OS}
cp ./rpmbuild/SRPMS/*.rpm packages/${TARGET_OS}
cp ./rpmbuild/RPMS/**/*.rpm packages/${TARGET_OS}

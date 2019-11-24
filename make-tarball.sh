#!/bin/bash
#
# make-tarball.sh
#

VERSION=$(grep Version docsnap.spec | rev | cut -f1 -d' ' | rev | head -n1)

git archive --prefix=docsnap-${VERSION}/ HEAD | gzip -9 > ~/rpmbuild/SOURCES/docsnap-${VERSION}.tar.gz


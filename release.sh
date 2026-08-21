#!/usr/bin/env bash

set -euo pipefail

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SPEC="$PROJECT_DIR/packaging/dustbunny.spec"

RPMBUILD="$HOME/rpmbuild"
SPEC_DEST="$RPMBUILD/SPECS/dustbunny.spec"
RPM_DIR="$RPMBUILD/RPMS/x86_64"

REPO_DIR="$HOME/Projects/dot-underscore1703.github.io"
REPO_ARCH="$REPO_DIR/x86_64"

PACKAGE="dustbunny"

# ------------------------------------------------------------
# Read version from spec
# ------------------------------------------------------------

VERSION="$(awk '$1 == "Version:" {print $2; exit}' "$SPEC")"
RELEASE="$(awk '$1 == "Release:" {print $2; exit}' "$SPEC" | sed 's/%{?dist}//')"

if [[ -z "$VERSION" ]]; then
    echo "ERROR: Could not determine package version."
    exit 1
fi

echo "==> Releasing $PACKAGE $VERSION-$RELEASE"

# ------------------------------------------------------------
# Make sure we're using the current spec
# ------------------------------------------------------------

echo "==> Installing spec into rpmbuild tree"

cp "$SPEC" "$SPEC_DEST"

# ------------------------------------------------------------
# Clean old build directories
# ------------------------------------------------------------

echo "==> Cleaning old build artifacts"

rm -rf "$RPMBUILD/BUILD/${PACKAGE}-"* \
       "$RPMBUILD/BUILDROOT/${PACKAGE}-"* \
       "$RPMBUILD/RPMS/x86_64/${PACKAGE}-"* \
       "$RPMBUILD/SRPMS/${PACKAGE}-"* 

# ------------------------------------------------------------
# Build RPM
# ------------------------------------------------------------

echo "==> Building RPM"

rpmbuild -ba "$SPEC_DEST"

RPM="$RPM_DIR/${PACKAGE}-${VERSION}-${RELEASE}.fc44.x86_64.rpm"

if [[ ! -f "$RPM" ]]; then
    echo "ERROR: Expected RPM was not produced:"
    echo "       $RPM"
    exit 1
fi

# ------------------------------------------------------------
# Verify RPM metadata
# ------------------------------------------------------------

echo "==> Verifying RPM"

RPM_VERSION="$(rpm -qp --qf '%{VERSION}-%{RELEASE}.%{ARCH}\n' "$RPM")"

echo "    $RPM_VERSION"

if [[ "$RPM_VERSION" != "$VERSION-$RELEASE.fc44.x86_64" ]]; then
    echo "ERROR: RPM version does not match spec."
    exit 1
fi

# ------------------------------------------------------------
# Verify binary inside RPM
# ------------------------------------------------------------

echo "==> Verifying binary"

TMP_DIR="$(mktemp -d)"
trap 'rm -rf "$TMP_DIR"' EXIT

cd "$TMP_DIR"

rpm2cpio "$RPM" | cpio -idm >/dev/null 2>&1

BINARY="./usr/bin/dustbunny"

if [[ ! -x "$BINARY" ]]; then
    echo "ERROR: RPM does not contain /usr/bin/dustbunny"
    exit 1
fi

BINARY_VERSION="$("$BINARY" <<<'help' | head -n 1)"

echo "    $BINARY_VERSION"

EXPECTED="DUSTBUNNY $VERSION"

if [[ "$BINARY_VERSION" != "$EXPECTED"* ]]; then
    echo "ERROR: Binary reports:"
    echo "       $BINARY_VERSION"
    echo "but expected:"
    echo "       $EXPECTED"
    exit 1
fi

# ------------------------------------------------------------
# Copy packages to repository
# ------------------------------------------------------------

echo "==> Publishing RPMs"

cp "$RPM" "$REPO_ARCH/"

for rpm in \
    "$RPM_DIR/${PACKAGE}-debuginfo-${VERSION}-${RELEASE}.fc44.x86_64.rpm" \
    "$RPM_DIR/${PACKAGE}-debugsource-${VERSION}-${RELEASE}.fc44.x86_64.rpm"
do
    if [[ -f "$rpm" ]]; then
        cp "$rpm" "$REPO_ARCH/"
    fi
done

# ------------------------------------------------------------
# Regenerate repository metadata
# ------------------------------------------------------------

echo "==> Regenerating repository metadata"

cd "$REPO_ARCH"

createrepo_c --update .

# ------------------------------------------------------------
# Commit and push
# ------------------------------------------------------------

cd "$REPO_DIR"

echo "==> Git status"
git status --short

echo
read -rp "Commit and push release $VERSION-$RELEASE? [y/N] " answer

if [[ "$answer" != "y" && "$answer" != "Y" ]]; then
    echo "Release files are ready, but were NOT committed/pushed."
    exit 0
fi

git add x86_64/

git commit -m "Release $PACKAGE $VERSION"

git push

echo
echo "=========================================="
echo " Successfully released $PACKAGE $VERSION"
echo "=========================================="

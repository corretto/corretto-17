#!/bin/bash

set -x

VERSION_BRANCH=$1

git config user.email "no-reply@amazon.com"
git config user.name "corretto-github-robot"

git checkout ${VERSION_BRANCH}

# Load the current OpenJDK version
source make/conf/version-numbers.conf

# Load the current Corretto version
CURRENT_VERSION=$(cat version.txt)

if [[ ${CURRENT_VERSION} == ${DEFAULT_VERSION_FEATURE}.${DEFAULT_VERSION_INTERIM}.${DEFAULT_VERSION_UPDATE}.* ]]; then
    echo "Corretto version is current."
else
    echo "Updating Corretto version"
    echo "${DEFAULT_VERSION_FEATURE}.${DEFAULT_VERSION_INTERIM}.${DEFAULT_VERSION_UPDATE}.0.0" > version.txt
    git commit -m "Update Corretto version to match upstream: ${DEFAULT_VERSION_FEATURE}.${DEFAULT_VERSION_INTERIM}.${DEFAULT_VERSION_UPDATE}" version.txt
    git push origin ${VERSION_BRANCH}
fi

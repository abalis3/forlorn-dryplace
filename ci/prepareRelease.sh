#!/bin/bash

# Pull dev build for this CI job and prepare docker tags for publishing
# Publish latest, full patch version, latest minor version release, and latest major version release
PATCH_TAG=$1
MINOR_TAG=${PATCH_TAG%.*}
MAJOR_TAG=${MINOR_TAG%.*}
docker tag abalis3/forbidden-desert-server:local abalis3/forbidden-desert-server:latest
docker tag abalis3/forbidden-desert-server:local abalis3/forbidden-desert-server:$PATCH_TAG
docker tag abalis3/forbidden-desert-server:local abalis3/forbidden-desert-server:$MINOR_TAG
docker tag abalis3/forbidden-desert-server:local abalis3/forbidden-desert-server:$MAJOR_TAG
docker rmi abalis3/forbidden-desert-server:local

# Run client release preparation
./client/ci/prepareRelease.sh $1
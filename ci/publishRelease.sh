#!/bin/bash

# Push built server container tags to docker hub
echo $DOCKERHUB_TOKEN | docker login -u abalis3 --password-stdin
docker push abalis3/forbidden-desert-server
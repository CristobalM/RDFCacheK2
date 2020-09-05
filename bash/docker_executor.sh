#!/bin/bash
CURRENT_FOLDER=$(eval pwd)
echo "CURRENT_FOLDER: ${CURRENT_FOLDER}"
time docker run --rm \
    --mount "type=bind,src=${CURRENT_FOLDER},destination=/mnt-outside" \
    --name docker_executor \
    --entrypoint "/RDFCacheK2/runner.sh" \
    rdf-cachke-k2v0-1 $@

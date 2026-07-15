#!/bin/bash
docker run -it --rm \
    --network host \
    -e XDG_RUNTIME_DIR=/tmp/runtime-root \
    -e "CYCLONEDDS_URI=<CycloneDDS><Domain><General><Interfaces><NetworkInterface name=\"lo\"/></Interfaces></General></Domain></CycloneDDS>" \
    -e ROS_DOMAIN_ID=1 \
    -v $(pwd):/workspace:z \
    acre_ctrl
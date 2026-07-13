FROM osrf/ros:humble-desktop

# Install development essentials
RUN apt-get update && apt-get install -y \
    git \
    wget \
    curl \
    gpg \
    python3-dev \
    python3-colcon-common-extensions \
    cmake \
    build-essential \
    libeigen3-dev \
    ros-humble-rmw-cyclonedds-cpp \
    ros-humble-rosidl-generator-dds-idl \
    libyaml-cpp-dev \
    nlohmann-json3-dev \
    && rm -rf /var/lib/apt/lists/*

# Clone and build Unitree CycloneDDS packages
RUN git clone https://github.com/unitreerobotics/unitree_ros2.git /opt/unitree_ros2 && \
    cd /opt/unitree_ros2/cyclonedds_ws && \
    . /opt/ros/humble/setup.sh && \
    CC=gcc CXX=g++ colcon build --symlink-install

# Install PCL and ANYbotics Gridmap
RUN apt-get update && apt-get install -y \
    ros-humble-pcl-conversions \
    ros-humble-pcl-ros \
    libpcl-dev \
    && rm -rf /var/lib/apt/lists/*

RUN mkdir -p /opt/anybotics_ws/src && \
    cd /opt/anybotics_ws/src && \
    git clone https://github.com/anybotics/grid_map.git --branch humble && \
    apt-get update && \
    rosdep update && \
    cd /opt/anybotics_ws && \
    . /opt/ros/humble/setup.sh && \
    rosdep install -y --ignore-src --from-paths src && \
    colcon build --symlink-install && \
    rm -rf /var/lib/apt/lists/*

# Install the Gridmap Python Bindings
RUN cd /opt/anybotics_ws/src && \
    git clone https://github.com/ACRE-Lab-NCSU/grid_map_python.git && \
    cd /opt/anybotics_ws && \
    . /opt/ros/humble/setup.sh && \
    . install/setup.sh && \
    colcon build --symlink-install --packages-select grid_map_python

# Entrypoint
RUN printf '#!/bin/bash\n\
source /opt/ros/humble/setup.sh\n\
source /opt/unitree_ros2/cyclonedds_ws/install/setup.bash\n\
source /opt/anybotics_ws/install/setup.bash\n\
export RMW_IMPLEMENTATION=rmw_cyclonedds_cpp\n\
exec "$@"\n' > /entrypoint.sh && chmod +x /entrypoint.sh

COPY . /workspace
WORKDIR /workspace

ENTRYPOINT ["/entrypoint.sh"]
CMD ["bash"]
# /ros_ws/src
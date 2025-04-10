FROM ubuntu:24.04
ARG DEBIAN_FRONTEND=noninteractive

RUN <<EOF
    echo "Set up the repository"

    set -x
    set -e
    set -u

    apt-get update
    apt-get install -y --no-install-recommends wget gpg sudo ca-certificates

# See https://www.intel.com/content/www/us/en/docs/oneapi/installation-guide-linux/2025-0/apt-005.html
#
# download the key to system keyring
    wget -O- https://apt.repos.intel.com/intel-gpg-keys/GPG-PUB-KEY-INTEL-SW-PRODUCTS.PUB \
    | gpg --dearmor | sudo tee /usr/share/keyrings/oneapi-archive-keyring.gpg > /dev/null

# add signed entry to apt sources and configure the APT client to use Intel repository:
    echo "deb [signed-by=/usr/share/keyrings/oneapi-archive-keyring.gpg] https://apt.repos.intel.com/oneapi all main" | sudo tee /etc/apt/sources.list.d/oneAPI.list

    apt-get update
    apt-get upgrade -y

    rm -rf /var/lib/apt/lists/*
EOF

RUN <<EOF
    echo "Install required oneAPI packages (development)"

    apt-get update

    apt-get install -y --no-install-recommends \
    intel-oneapi-compiler-dpcpp-cpp-2025.0 \
    intel-oneapi-compiler-fortran-2025.0 \
    intel-oneapi-mpi-devel-2021.14 \
    ""

    rm -rf /var/lib/apt/lists/*
EOF

RUN <<EOF
    echo "Install PISM's prerequisite tools and libraries"
    apt-get update

    apt-get install -y --no-install-recommends \
    autoconf \
    automake \
    cdo \
    cmake \
    coreutils \
    cython3 \
    git \
    libfftw3-dev \
    libfyaml-dev \
    libgsl-dev \
    libproj-dev \
    libtool \
    libudunits2-dev \
    m4 \
    make \
    nco \
    pkg-config \
    python3 \
    python3-dev \
    python3-netcdf4 \
    python3-nose \
    python3-numpy \
    python3-scipy \
    python3-setuptools \
    ssh \
    swig \
    ""

    rm -rf /var/lib/apt/lists/*
EOF

# See
# https://www.intel.com/content/www/us/en/docs/oneapi/programming-guide/2024-1/use-the-setvars-and-oneapi-vars-scripts-with-linux.html
#
# for the right way to initialize Intel oneAPI.

COPY hdf5.sh /tmp/
RUN <<EOF
    echo "Install HDF5"
    . /opt/intel/oneapi/setvars.sh

    set -eux
    export CC=mpiicx
    export CXX=mpiicpx
    export build_dir=/var/tmp/build/hdf5
    export prefix=/opt/hdf5

    /tmp/hdf5.sh
    rm -rf ${build_dir}
    rm -rf /opt/hdf5/share/hdf5_examples
EOF

COPY pnetcdf.sh /tmp/
RUN <<EOF
    echo "Install PnetCDF"
    . /opt/intel/oneapi/setvars.sh

    set -eux
    export CC=mpiicx
    export build_dir=/var/tmp/build/pnetcdf
    export prefix=/opt/pnetcdf

    /tmp/pnetcdf.sh
    rm -rf ${build_dir}
EOF

COPY netcdf.sh /tmp/
RUN <<EOF
    echo "Install NetCDF"
    . /opt/intel/oneapi/setvars.sh

    set -eux
    export CC=mpiicx
    export build_dir=/var/tmp/build/netcdf
    export lib_dir=/opt
    export prefix=/opt/netcdf

    /tmp/netcdf.sh
    rm -rf ${build_dir}
EOF

COPY yac.sh /tmp/
RUN <<EOF
    echo "Install YAC"
    . /opt/intel/oneapi/setvars.sh

    set -eux
    export CC=mpiicx
    export FC=mpiifx
    export build_dir=/var/tmp/build/netcdf
    export prefix=/opt/yac

    /tmp/yac.sh
    rm -rf ${build_dir}
EOF

COPY petsc.sh /tmp/
RUN <<EOF
    echo "Install PETSc"
    . /opt/intel/oneapi/setvars.sh

    set -eux
    export CC=mpiicx
    export CXX=mpiicpx
    export build_dir=/var/tmp/build/petsc
    export prefix=/opt/petsc
    export COPTFLAGS="-O3 -fp-model=precise -march=native -mtune=native"
    export version=3.22.2

    /tmp/petsc.sh

    rm -rf ${build_dir}
    rm -rf /opt/petsc/share/petsc
EOF

ENV PETSC_DIR="/opt/petsc" \
    PYTHONPATH="/opt/petsc/lib"

ENV PATH="/opt/hdf5/bin:/opt/netcdf/bin:/opt/petsc/bin:$PATH"

RUN ln -s /opt/intel/oneapi/setvars.sh /etc/profile.d/02-intel-oneapi-setvars.sh

# Add a non-privileged user
RUN useradd --create-home --system --shell=/bin/false worker && usermod --lock worker
USER worker

# Important parts of ". /opt/intel/oneapi/setvars.sh"
ENV ONEAPI_ROOT=/opt/intel/oneapi
ENV LD_LIBRARY_PATH=${ONEAPI_ROOT}/tbb/2021.14/env/../lib/intel64/gcc4.8:${ONEAPI_ROOT}/mpi/2021.14/opt/mpi/libfabric/lib:${ONEAPI_ROOT}/mpi/2021.14/lib:${ONEAPI_ROOT}/mkl/2025.0/lib:${ONEAPI_ROOT}/compiler/2025.0/opt/compiler/lib:${ONEAPI_ROOT}/compiler/2025.0/lib:${ONEAPI_ROOT}/mpi/2021.14/opt/mpi/libfabric/lib:${ONEAPI_ROOT}/mpi/2021.14/lib:${ONEAPI_ROOT}/compiler/2025.0/opt/compiler/lib:${ONEAPI_ROOT}/compiler/2025.0/lib
ENV I_MPI_ROOT=${ONEAPI_ROOT}/mpi/2021.14
ENV FI_PROVIDER_PATH=${ONEAPI_ROOT}/mpi/2021.14/opt/mpi/libfabric/lib/prov:/usr/lib/x86_64-linux-gnu/libfabric
ENV PATH=${ONEAPI_ROOT}/mpi/2021.14/bin:${ONEAPI_ROOT}/mkl/2025.0/bin/:${ONEAPI_ROOT}/compiler/2025.0/bin:${PATH}

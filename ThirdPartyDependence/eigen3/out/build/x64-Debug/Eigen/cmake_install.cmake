# Install script for directory: C:/PHD/Code/CASE2021_code/ThirdPartyDependence/eigen3/Eigen

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/PHD/Code/CASE2021_code/ThirdPartyDependence/eigen3/out/install/x64-Debug")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDevelx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eigen3/Eigen" TYPE FILE FILES
    "C:/PHD/Code/CASE2021_code/ThirdPartyDependence/eigen3/Eigen/Cholesky"
    "C:/PHD/Code/CASE2021_code/ThirdPartyDependence/eigen3/Eigen/CholmodSupport"
    "C:/PHD/Code/CASE2021_code/ThirdPartyDependence/eigen3/Eigen/Core"
    "C:/PHD/Code/CASE2021_code/ThirdPartyDependence/eigen3/Eigen/Dense"
    "C:/PHD/Code/CASE2021_code/ThirdPartyDependence/eigen3/Eigen/Eigen"
    "C:/PHD/Code/CASE2021_code/ThirdPartyDependence/eigen3/Eigen/Eigenvalues"
    "C:/PHD/Code/CASE2021_code/ThirdPartyDependence/eigen3/Eigen/Geometry"
    "C:/PHD/Code/CASE2021_code/ThirdPartyDependence/eigen3/Eigen/Householder"
    "C:/PHD/Code/CASE2021_code/ThirdPartyDependence/eigen3/Eigen/IterativeLinearSolvers"
    "C:/PHD/Code/CASE2021_code/ThirdPartyDependence/eigen3/Eigen/Jacobi"
    "C:/PHD/Code/CASE2021_code/ThirdPartyDependence/eigen3/Eigen/KLUSupport"
    "C:/PHD/Code/CASE2021_code/ThirdPartyDependence/eigen3/Eigen/LU"
    "C:/PHD/Code/CASE2021_code/ThirdPartyDependence/eigen3/Eigen/MetisSupport"
    "C:/PHD/Code/CASE2021_code/ThirdPartyDependence/eigen3/Eigen/OrderingMethods"
    "C:/PHD/Code/CASE2021_code/ThirdPartyDependence/eigen3/Eigen/PaStiXSupport"
    "C:/PHD/Code/CASE2021_code/ThirdPartyDependence/eigen3/Eigen/PardisoSupport"
    "C:/PHD/Code/CASE2021_code/ThirdPartyDependence/eigen3/Eigen/QR"
    "C:/PHD/Code/CASE2021_code/ThirdPartyDependence/eigen3/Eigen/QtAlignedMalloc"
    "C:/PHD/Code/CASE2021_code/ThirdPartyDependence/eigen3/Eigen/SPQRSupport"
    "C:/PHD/Code/CASE2021_code/ThirdPartyDependence/eigen3/Eigen/SVD"
    "C:/PHD/Code/CASE2021_code/ThirdPartyDependence/eigen3/Eigen/Sparse"
    "C:/PHD/Code/CASE2021_code/ThirdPartyDependence/eigen3/Eigen/SparseCholesky"
    "C:/PHD/Code/CASE2021_code/ThirdPartyDependence/eigen3/Eigen/SparseCore"
    "C:/PHD/Code/CASE2021_code/ThirdPartyDependence/eigen3/Eigen/SparseLU"
    "C:/PHD/Code/CASE2021_code/ThirdPartyDependence/eigen3/Eigen/SparseQR"
    "C:/PHD/Code/CASE2021_code/ThirdPartyDependence/eigen3/Eigen/StdDeque"
    "C:/PHD/Code/CASE2021_code/ThirdPartyDependence/eigen3/Eigen/StdList"
    "C:/PHD/Code/CASE2021_code/ThirdPartyDependence/eigen3/Eigen/StdVector"
    "C:/PHD/Code/CASE2021_code/ThirdPartyDependence/eigen3/Eigen/SuperLUSupport"
    "C:/PHD/Code/CASE2021_code/ThirdPartyDependence/eigen3/Eigen/UmfPackSupport"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDevelx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eigen3/Eigen" TYPE DIRECTORY FILES "C:/PHD/Code/CASE2021_code/ThirdPartyDependence/eigen3/Eigen/src" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()


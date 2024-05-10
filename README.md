# vsgPhysX

Integration between the VulkanSceneGraph and PhysX.

## Licence

SPDX: MIT

The first few commits had the LGPL licence as I'd not yet checked whether everything was sufficiently distinct from osgPhysX to not count as derivative, or in places where it was, whether they were too trivial to copyright or copying was permissible on fair use grounds.
Now I *have*, I can grant availability of all parent commits of the one that changed the licence under the same MIT licence, too, and am writing this sentence to do so without having to rewrite the Git history.

## Dependencies

Right now, this project only depends on VSG and PhysX.

### VulkanSceneGraph

Just build/download and install it normally, and ensure the install path is available via `CMAKE_PREFIX_PATH` or is the same as `CMAKE_INSTALL_PREFIX` when configuring this project so `find_package(vsg REQUIRED)` works.

### PhysX

PhysX can be more of a pain, so there are two recommended approaches:

1. * Build https://github.com/NVIDIA-Omniverse/PhysX according to the instructions for your platform, with any extra steps necessary to make this actually work.
     If you're targeting Windows, it might be a good idea to alter the provided preset to dynamically link the C runtime, as this is the default for most projects and will prevent linker errors later.
   * Install it according to the instructions for your platform.
   * When configuring this project with CMake:
     * ensure the install path is available via `CMAKE_PREFIX_PATH` or is the same as `CMAKE_INSTALL_PREFIX`
     * set `PHYSX_PLATFORM_DIRECTORY_NAME` to the one PhysX has chosen, e.g. `win.x86_64.vc143.md` or `linux.clang`
     * If you're not happy with the defaults (`debug` for `Debug` and `checked` for everything else), set the `PHYSX_MAP_IMPORTED_CONFIG_<confi>}` variables, or set `PHYSX_BUILD_TYPE` to do it overall.
2. * Get the unofficial vcpkg PhysX port.
   * You can either use vcpkg's toolchain file as usual, or include the vcpkg install directory in `CMAKE_PREFIX_PATH` as it exports proper CMake config files rather than relying on vcpkg magic.

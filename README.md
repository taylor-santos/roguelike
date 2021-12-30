[![codecov](https://codecov.io/gh/taylor-santos/roguelike/branch/trunk/graph/badge.svg?token=EQ4YRC3D76)](https://codecov.io/gh/taylor-santos/roguelike)

<h3 align="center">Unnamed Roguelike</h3>

<details open="open">
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#acknowledgements">Acknowledgements</a></li>
  </ol>
</details>

<!-- GETTING STARTED -->

## Getting Started

### Prerequisites

* CMake >= 3.16
* A C++20 supporting compiler
    * g++ >= 8
    * clang++ >= 9
    * MSVC >= 19.29
* Linux dependencies:
    * GLFW dependencies: [See here](https://www.glfw.org/docs/latest/compile.html#compile_deps) for dependencies based
      on your distro and display server.
        * For example, on Debian with X11 you only need the `xorg-dev` package.
        * On Debian with Wayland, you need the following packages:
            * `libwayland-dev`
            * `libxkbcommon-dev`
            * `wayland-protocols`
            * `extra-cmake-modules`
    * An OpenGL implementation:
        * Mesa can be installed on Debian with the `libgl1-mesa-dev` package.
* The following dependencies are fetched and built automatically by CMake:
    * glad >= 0.1.34
    * glfw3 >= 3.3.6
    * glm >= 0.9.9.8
    * ImGui (docking branch) >= 1.85
    * doctest >= 2.4.6

### Installation

1. Clone the repo and create a build directory:

    ```sh
    git clone https://github.com/taylor-santos/roguelike
    cd roguelike
    mkdir build
    cd build
    ```

2. Run CMake, depending on your display server:
    * X11 / Windows / MacOS:
      ```sh
       cmake -D CMAKE_BUILD_TYPE=Release ..
      ```
    * Wayland:
      ```
      cmake -D CMAKE_BUILD_TYPE=Release -D GLFW_USE_WAYLAND=ON ..
      ```
    * OSMesa (headless):
      ```
      cmake -D CMAKE_BUILD_TYPE=Release -D GLFW_USE_OSMESA=ON ..
      ```

3. Build
    ```sh
    cmake --build .
    ```
4. Run Application
    ```sh
    
    ```
5. Run Tests
    ```sh
    cd test
    ctest -C Release --rerun-failed --output-on-failure
    ```

<!-- CONTRIBUTING -->

## Contributing

1. Fork the Project
1. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
1. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
1. Push to the Branch (`git push origin feature/AmazingFeature`)
1. Open a Pull Request

<!-- LICENSE -->

## License

Distributed under the MIT License. See `LICENSE` for more information.

<!-- CONTACT -->

## Contact

Taylor Santos - taylor.p.santos@gmail.com

Project Link: [https://github.com/taylor-santos/roguelike](https://github.com/taylor-santos/roguelike)

<!-- ACKNOWLEDGEMENTS -->

## Acknowledgements

* [Dear ImGui](https://github.com/ocornut/imgui)

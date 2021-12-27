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
* glad >= 0.1.34 (included in `external/` directory)
* glfw3 >= 3.3.6 (retrieved by CMake FetchContent)
* Linux dependencies:
    * Depending on your display server: ([more info](https://www.glfw.org/docs/latest/compile.html#compile_deps_x11))
        * X11: `xorg-dev`
        * Wayland: `libwayland-dev`
        * OSMesa: `libosmesa6-dev`
    * `libgl1-mesa-dev`
    * `libXrandr-dev`
    * `libXinerama-dev`
    * `libXcursor-dev`
    * `libXi-dev`
* glm >= 0.9.9.8 (retrieved by CMake FetchContent)
* ImGui (docking branch) >= 1.85 (retrieved by CMake FetchContent)
* doctest >= 2.4.6 (retrieved by CMake FetchContent)

### Installation

1. Clone the repo

    ```sh
    git clone https://github.com/taylor-santos/roguelike
    ```

1. Run CMake
    ```sh
    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    ```

1. Build
    ```sh
    cmake --build .
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

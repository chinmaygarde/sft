# A Software Renderer

[![Build](https://github.com/chinmaygarde/sft/actions/workflows/build.yml/badge.svg)](https://github.com/chinmaygarde/sft/actions/workflows/build.yml)

A software renderer modelled after graphics APIs like Metal and Vulkan. A programmable shader based graphics pipeline, depth & stencil buffers, multi-sampling, tile-based-deferred-rendering, multi-core operation, blending, custom ImGUI integration, etc..

| Depth Buffers            |  Stencil Buffers           |
:-------------------------:|:---------------------------:
![](fixtures/demo/demo1.png) | ![](fixtures/demo/demo5.png) |
| ImGUI Integration        |  Instrumentation           |
![](fixtures/demo/demo2.png) | ![](fixtures/demo/demo4.png) |
| Texture Sampling         |  Blending                  |
![](fixtures/demo/demo3.png) | ![](fixtures/demo/demo6.png) |

## Prerequisites

* CMake (3.22 or above).
* Git.
* Ninja.
* [Just](https://just.systems/), a task runner.
* A C11 and C++20 compiler.
* [vcpkg](https://vcpkg.io/en/index.html) for package management.
  * Ensure that the `VCPKG_ROOT` environment variable is present and valid.

## Build & Run

* `just sync` ensures that the right sub-module dependencies are pulled in.
* `just run` runs the demo application.

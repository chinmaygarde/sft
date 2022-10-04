# A Software Renderer

A software renderer modelled after graphics APIs like Metal and Vulkan. A programmable shader based graphics pipeline, depth & stencil buffers, blending, custom ImGUI integration, etc..

| Depth Buffers            |  Stencil Buffers           |
:-------------------------:|:---------------------------:
![](assets/demo/demo1.png) | ![](assets/demo/demo5.png) |
| ImGUI Integration        |  Instrumentation           |
![](assets/demo/demo2.png) | ![](assets/demo/demo4.png) |
| Texture Sampling         |  Blending                  |
![](assets/demo/demo3.png) | ![](assets/demo/demo6.png) |

## Dependencies

This is a very simple CMake project. The Makefile at the project root has tasks to make development easier. The Makefile does assume you have `cmake` and `ninja` installed on your host.

## Build & Run

* `make sync` ensures that the right sub-module dependencies are pulled in.
* `make run` runs the demo application.

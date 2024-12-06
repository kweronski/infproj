# Building

To build the project you can install the deps on deb-based distros like so:

```console
sudo apt install libsfml-dev libpugixml-dev
```

And then invoke cmake:

```console
cmake -B build
make -C build
```

# Where to start?

Take a look at the src/demo directory :)
All the deps along with the binaries can be found in ./build/infproj

If you do not want the demo code to be built add -DDEMO=OFF to cmake.

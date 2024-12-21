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

All the deps along with the binaries can be found in ./build/infproj

To integrate your own mini-game, simply add your custom initialization
function to the main initialization function located in src/cfusion.cpp

It is recommended to use the create\_scene\_skeleton function
to initialize your scene's pointers.

A good demonstration of creating the basics for a mini-game
can be found in src/rps.cpp in the initialize\_rps function.

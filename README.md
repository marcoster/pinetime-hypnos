# PineTime Hypnos

This is a WIP [Zephyr RTOS](https://www.zephyrproject.org/) firmware for the
[PineTime](https://www.pine64.org/pinetime/) smartwatch.

It started as a fork of Jan Jansen's [firmware toolkit](https://github.com/najnesnaj/pinetime-zephyr).

## Getting started

Follow Zephyr's [Getting Started Guide](https://docs.zephyrproject.org/latest/getting_started/index.html)
up to step 3) "Get the source code". Here you should run the commands below
instead of the ones in the guide:

```
$ mkdir ~/pinetime-hypnos
$ cd ~/pinetime-hypnos
$ west init -m https://github.com/endian-albin/pinetime-hypnos
$ west update
```

Then complete the remaining steps.

To build and install the application, run

```
$ west build -p -b pinetime hypnos
$ west flash
```

## License

All code and documentation is licensed under the Apache 2.0 (see LICENSE),
unless stated otherwise.

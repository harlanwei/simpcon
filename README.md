# Simpcon: A simple container for lightweight applications

## Dependencies

- `cmake`, version >= 3.16
- `make`
- `gcc`, verified under 9.3.0

## Build

To build Simpcon, run the following commands under project root:

1. `cmake .`
2. `make simpcon`

## Usage

Simpcon is a single executable program. To use Simpcon, run it as root, provide the path to root image with the environment variable `simpcon_fkroot` and provide the command you wish to execute. For example:

```sh
sudo simpcon_fkroot="/home/user/fkroot" ./simpcon /bin/sh
```

We recommend using [Alpine Linux mini root filesystem](https://www.alpinelinux.org/downloads/) as your root image. 

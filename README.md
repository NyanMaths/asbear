# ASBEAR (Another Simple But Efficient Audio Recorder)

ASBEAR is a simple audio recorder made with Qt and SFML.
Intended to work on Linux, I can not promise anything for Winedow$ or Apple things.
I can't test for xorg as I am on wayland but it should be fiiiiine...

## Showcase

[!image](.captures/recorder.webp)
[!image](.captures/other.webp)

## Installation

As I do not support anything else than Linux for now, Windows users are on their own.
On Linux, you have to build it yourself. You will need Qt 6 and SFML 2 to do so.
Clone the repo, cd inside and `cmake . && make build`.
To launch, `cd build/release && bin/asbear`.

## Roadmap

* Using an acutaul configuration format like YAML
* Automating installation
* Supporting more codecs


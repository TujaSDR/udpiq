# udpiq

This is a small program for streaming raw IQ samples over UDP. It's part of a larger project to build a
Raspberry Pi based SDR transceiver.

This technique works great over LAN but probably not so great over the internet because of packet loss.

Usable with GNU Radio UDP source (**port 7373 payload size 8192**).

## ideas for improvement aka TODO

* Smaller UDP packets.
* Experiment with internet streaming.

## building and using

```
meson build
cd build
ninja
./udpiq IP
```

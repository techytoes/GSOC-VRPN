# VRPN 
This repo contains all my work that I did related to the GSOC project.

## Installing VRPN

* MacOS

```
$ brew install vrpn
```
To run the example
```
$ g++ test.cpp -lvrpn -lpthread
$ ./a.out
```
* Linux

Download VRPN source (version 7.33):
```sh
wget https://github.com/vrpn/vrpn/releases/download/v07.33/vrpn_07_33.zip
```

Extract source archive:

```sh
unzip vrpn_07_33.zip
```

We don't need the archive anymore. So let's delete it:

```sh
rm -f vrpn_07_33.zip
```

Change to source directory:

```sh
cd vrpn
```

Create a build directory and go into it:

```sh
mkdir build && cd build
```

Generate build environment using `cmake`:

```sh
cmake ..
```

Build project:

```sh
make
```

Install VRPN:

```sh
sudo make install
```

# QtCreator

![cmake](images/plugin.png)

# CMAKE-GUI Configuration

![cmake](images/cmake.png)

## How to Use ServerCommunication VRPN

For Analog -

Recieve

```
<ServerCommunicationVRPN name="vrpn1" job="receiver" address="localhost"/>
<CommunicationSubscriber name="sub1" communication="@vrpn1" subject="Mouse0" target="@light1" datas="cutoff"/>
```

Sender

```
<ServerCommunicationVRPN name="vrpn1" job="sender" address="localhost"/>
<CommunicationSubscriber name="sub1" communication="@vrpn1" subject="Mouse0" target="@light1" datas="cutoff"/>
```

For Tracker -

Receiver

```
<ServerCommunicationVRPN name="vrpn1" job="receiver" address="localhost"/>
<CommunicationSubscriber name="sub1" communication="@vrpn1" subject="Tracker0" target="@light1" datas="aNewStringValue"/>
```

Sender

```
<ServerCommunicationVRPN name="vrpn1" job="sender" address="localhost"/>
<CommunicationSubscriber name="sub1" communication="@vrpn1" subject="Tracker0" target="@light1" datas="aNewStringValue"/>
```

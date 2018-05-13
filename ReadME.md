# VRPN 
This repo contains all my work that I did related to the GSOC project.

## Installing VRPN

* MacOS

```
$ brew install vrpn
```
To run the example
```
$ g++ test.cpp -lvrpn
$ ./a.out
```

### Configuration

- Before communication we need to configure `vrpn_server` for `Mouse0`. Open `vrpn.cfg` and remove the `#` from the `vrpn_Mouse`. It will look like this after the changes.

```
vrpn_Mouse Mouse0
```

- Now start the `testServer` present in the same folder.
- Open a new terminal window and open `vrpn_print_devices` with argument as `Mouse0@localhost`. 

```
$ /Users/firedranzer/Documents/build-vrpn-Desktop_Qt_5_10_1_clang_64bit-Default/client_src/vrpn_print_devices Mouse0@localhost
```
- Now the 2 terminals can be used to communicate with each other.

Terminal Window 1
```
Please enter the message:
Rupesh Harode
VRPN Error
 (0) from Mouse0: Server connection re-established!
VRPN Error
 (0) from Mouse0: Server connection re-established!
VRPN Error
 (0) from Mouse0: Server connection re-established!
VRPN Error
 (0) from Mouse0: Server connection re-established!
VRPN Error
 (0) from Mouse0: Server connection re-established!
Please enter the message:
Please enter the message:
Is this connected?
```
Terminal Window 2
```
Mouse0@localhost: Text message: Rupesh
Mouse0@localhost: Text message: Harode
Mouse0@localhost: Text message: Is
Mouse0@localhost: Text message: this
Mouse0@localhost: Text message: connected?
```
# Server-Client

A Simple Server Client Communication via Analog Message.

## Running the example

* Make Sure that VRPN library is compiled correctly on your Machine.

### Starting the Server

```
$ g++ testServer.cpp -lvrpn
$ ./a.out
```

![cmake](/images/server.png)

### Starting the Client

```
$ g++ testClient.cpp -lvrpn
$ ./a.out
```

![cmake](/images/client.png)

* Note : If you face errors while compiling you can use flags like `-lvrpn` and `-lpthread`.

Assignment on RFModule and Yarp thrift IDL
==========================================

In this assignment (assignment_Codec) you are asked to add a service interface (using Yarp thrift IDL) to the previously developed module during the [tutorial_RFModule](https://github.com/vvv-school/tutorial_RFModule).


# Assignment
During the [`tutorial_RFModule`](https://github.com/vvv-school/tutorial_RFModule) we have developed a module to encode and decode string messages. The module could work as `coder` or `decoder` depending on its parameters :
```
$ tutorial_RFModule --mode coder
$ tutorial_RFModule --mode decoder
```
It had an input port to receive text messages, an output port to steam out the encoded/decoded messages. Now you need to add a RPC port (lets call it `"/Codec/rpc"`) for the remote and runtime configuration of the module:

![module](/misc/tutotial_RFModule.png)


Via this RPC port we should be able to call the following methods:
1. `set_mode()` which accept a **string** (i.e. `coder`, `decoder`) and  switch the module behavior between coding and decoding mode.
2. `get_mode()` returns an **string** showing the module current mode (i.e. `coder`, `decoder`).
3. `set_codec()` accepts an **integer** (i.e. the asscci code of a character) and uses it to encode/decode the string.


# What do you need to do?
Not so much! ;)
- Modify the file `src/idl/assignment_Codec.thrift` to add the above methods in thrift IDL format.
- Modify the file `src/Codec.h` to add the missing interfaces of `Codec_IDL` class
- Modify the file `src/Codec.cpp` to implement the missing interface of `Codec_IDL` class


# Build and Install the code
Follow these steps to build and properly install your module:
```
$ cd assignment_Codec
$ mkdir build; cd build
$ cmake ../
$ make
$ make install
```
Running `cmake` and `make` will automatically call `yarpidl_thrift` on your `src/idl/assignment_Codec.thrift` and generate the corresponding code into `build` directory:
- `build/include/src/idl/Codec_IDL.h`
- `build/src/Codec_IDL.cpp`  

**Notice**: you do not need to modify these files. So every time you update the `assignment_Codec.thrift` file, you just need to rerun the `make` and it will update the corresponding (generated) files :)

The `make install` will also install your module (binary, xml files, etc) in the icub contrib folder which is already setup on your machine.

# Testing your code
- open a terminal and run `assignment_Codec`
```
$ assignment_Codec
[INFO]Configuring the codedc module...
yarp: Port /Codec/rpc active at tcp://127.0.0.1:10002/
yarp: Port /Codec/in active at tcp://127.0.0.1:10003/
yarp: Port /Codec/out active at tcp://127.0.0.1:10004/
```

- In another terminal run `yarp read`
```
$ yarp read ... /Codec/out
```
- In another terminal run `yarp write` and type something
```
$ yarp write ... /Codec/in
>> hello
```
You should be able to see the codec message (i.e. `(%,,/`)in the `yarp read` terminal.

- In a terminal run `yarp rpc` and check your interfaces  
```
$ yarp rpc /Codec/rpc
>> get_mode
Response: coder
>> set_mode decoder
Response: [ok]
```
Now if you write a `(%,,/` in the `yarp write` terminal you should be able to see the encoded message (i.e. `hello`) in the `yarp read` terminal.  


# Run the smoke test
Close the `assignment_Codec` module (e.g. press `CTRL+C`) and other terminals. switch to the `smoke-test` folder and run the test:
```
$ cd assignment_Codec/smoke-test
$ ./test.sh
```

# [How to complete the assignment](https://github.com/vvv-school/vvv-school.github.io/blob/master/instructions/how-to-complete-assignments.md)

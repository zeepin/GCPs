# Usage instruction of ZeepinChain

### 1. Download zeepin executable file
Download the file from the link below, and save it in a local folder named "zeepin" or whatever you want.

https://github.com/zeepin/zeepinChain/releases

<p align="center">
  <img  src="https://github.com/zeepin/GCPs/blob/master/src/screenshot/01.jpg">
</p>

**The following instruction will be based on the MAC operating system.**

Rename the file from "zeepin-darwin-amd64" to "zeepin" to use it more conveniently.

<p align="center">
  <img  src="https://github.com/zeepin/GCPs/blob/master/src/screenshot/02.jpg">
</p>

Start a terminal, and use the following command to authorize the file as an executable file.

<p align="center">
  <img  src="https://github.com/zeepin/GCPs/blob/master/src/screenshot/03.jpg">
</p>

<p align="center">
  <img  src="https://github.com/zeepin/GCPs/blob/master/src/screenshot/04.jpg">
</p>

### 2. Create your first account
Use the following command to create an account.

<p align="center">
  <img  src="https://github.com/zeepin/GCPs/blob/master/src/screenshot/05.jpg">
</p>

Press enter directly to select default choices three times. Then enter and re-enter your own password. **Please remember this password.**

<p align="center">
  <img  src="https://github.com/zeepin/GCPs/blob/master/src/screenshot/06.jpg">
</p>

You have already created your first account successfully! You can see a new "wallet.dat" file is created automatically in the "zeepin" folder. This file stores your account keystore, which contains the information of your account. You don't need to know about it, and **what you have to do is to remember the password you inputed before**. It is very important for you to invoke the smart contract depolyed on the chain.

### 3. Generate a .wasm file by smart contract
We have provided a sample smart contract code for you to try to deploy and invoke. See our code from the link below.

https://github.com/zeepin/GCPs/blob/master/StorageSample.c

This code provides all APIs of our Zeepin chain for you to use them directly. Pay attention to the `invoke` function, it's like a entry of the whole program

You can see three methods:

`addStorage`: Add a key-value pair into the database.

`getStorage`: Get the value from the given key.

`deleteStorage`: Delete a key-value pair from the database.

Now we give you a brief view of our contract code. You can try to depoly and invoke it to figure out how it works. Please operate as follows:

Copy the whole code, then paste them into the link below.

https://wasdk.github.io/WasmFiddle/

Paste the code on the upper left side, then click the "Build". After building, you will see something occurs on the bottom left side, and then click "Wasm" to download the "program.wasm" file. Save this file into the "zeepin" folder.

<p align="center">
  <img  src="https://github.com/zeepin/GCPs/blob/master/src/screenshot/07.jpg">
</p>

<p align="center">
  <img  src="https://github.com/zeepin/GCPs/blob/master/src/screenshot/08.jpg">
</p>

### 4. Deploy and invoke the smart contract
Now you have three files in the "zeepin" folder.

<p align="center">
  <img  src="https://github.com/zeepin/GCPs/blob/master/src/screenshot/09.jpg">
</p>

On your current terminal, input the following command to run the Zeepin chain under "testmode". Enter your password created before.

<p align="center">
  <img  src="https://github.com/zeepin/GCPs/blob/master/src/screenshot/10.jpg">
</p>

Then open another terminal. Input the following command to deploy the smart contract on the chain.

<p align="center">
  <img  src="https://github.com/zeepin/GCPs/blob/master/src/screenshot/11.jpg">
</p>

Then you get the deployed contract address. You will use it to invoke this contract.

<p align="center">
  <img  src="https://github.com/zeepin/GCPs/blob/master/src/screenshot/12.jpg">
</p>

### invoke
##### addStorage
You can use the command below to invoke the "addStorage" method to add a key-value pair into the database. Enter your password. (Each time you invoke the contract, you need to enter the password.)

<p align="center">
  <img  src="https://github.com/zeepin/GCPs/blob/master/src/screenshot/13.jpg">
</p>

Then you will get the TxHash. You can use it to check the information.

<p align="center">
  <img  src="https://github.com/zeepin/GCPs/blob/master/src/screenshot/14.jpg">
</p>

<p align="center">
  <img  src="https://github.com/zeepin/GCPs/blob/master/src/screenshot/15.jpg">
</p>

We can see "Notify" shows that this invoke succeed.

##### getStorage
Then you can get the value of key "a" by the command below.

<p align="center">
  <img  src="https://github.com/zeepin/GCPs/blob/master/src/screenshot/16.jpg">
</p>

##### deleteStorage
Finally, you can delete a key-value pair from the database.

<p align="center">
  <img  src="https://github.com/zeepin/GCPs/blob/master/src/screenshot/17.jpg">
</p>

Now you may understand how to deploy and invoke a smart contract on the Zeepin chain. You can have a try to write your own contract. If you wish to issue tokens, either fungible tokens or non-fungible tokens, you can read our GCP-10 and GCP-30 protocols, which may be very helpful to you.

GCP-10: https://github.com/zeepin/GCPs/blob/master/English/GCP10_EN.md

GCP-30: https://github.com/zeepin/GCPs/blob/master/English/GCP30_EN.md

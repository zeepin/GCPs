# Modify WASM Memory Bound

Build your code, and copy the content in the red circle.

<p align="center">
  <img  src="https://github.com/zeepin/GCPs/blob/master/src/screenshot/18.jpg">
</p>

Open the link below.

https://webassembly.github.io/wabt/demo/wat2wasm/

Paste the content into the red circle, find the code `(memory $0 1)`, and then modify it to `(memory $0 2)`. Finally click the download.

<p align="center">
  <img  src="https://github.com/zeepin/GCPs/blob/master/src/screenshot/19.jpg">
</p>

Now you get a new .wasm file with larger memory bound. You can deploy it onto Zeepin Chain and have a try to invoke it again.

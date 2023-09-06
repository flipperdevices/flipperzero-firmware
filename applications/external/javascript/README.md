# js-f0
JavaScript... on the Flipper Zero?
## How?
This is all possible using the [microvium JavaScript engine](https://github.com/coder-mike/microvium). Check it out!
## Installation
Download the artifact from the most recent successful [action](https://github.com/zap8600/js-f0/actions) and put it in `/ext/apps` on your Flipper Zero.
## Usage
1. Install microvium on your computer with `npm install -g microvium`.
2. Create a new script with the following contents:
```js
// script.mvm.js
console.log = vmImport(7);
function main() {
  console.log('Hello, World!');
}
vmExport(1, main);
```
3. Compile it with `microvium script.mvm.js`. This will make a file in the same directory called `script.mvm-bc`.
4. Copy `script.mvm-bc` to `/ext/apps_data/js` on your Flipper Zero.
5. Run `JavaScript` on your Flipper Zero.
6. Press the center button to open the console and find "Hello, world!" in the log! 

# Flipper Zero JavaScript SDK
This package contains official tooling and typings for developing Flipper Zero
applications in JavaScript.

## Getting started
Create your application using the interactive wizard:
```shell
npx @flipperdevices/create-fz-app@latest
```

Then, enter the directory with your application and launch it:
```shell
cd my-flip-app
npm start
```

You are free to use `pnpm` or `yarn` instead of `npm`.

## Versioning
For each version of this package, the major and minor components match those of
the Flipper Zero JS SDK version that that package version targets. This version
follows semver. For example, apps compiled with SDK version `0.1.0` will be
compatible with SDK versions `0.1`...`1.0` (not including `1.0`).

Every API has a version history reflected in its JSDoc comment. It is heavily
recommended to check SDK compatibility using a combination of
`sdkCompatibilityStatus`, `isSdkCompatible`, `assertSdkCompatibility` depending
on your use case.

## Documentation
Check out the [JavaScript section in the Developer Documentation](https://developer.flipper.net/flipperzero/doxygen/js.html)

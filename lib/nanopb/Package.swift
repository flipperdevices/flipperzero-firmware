// swift-tools-version:5.0
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let package = Package(
  name: "nanopb",
  products: [
    .library(
      name: "nanopb",
      targets: ["nanopb"]
    )
  ],

  targets: [
    .target(
      name: "nanopb",
      path: ".",
      sources: [
        "pb.h",
        "pb_common.h",
        "pb_common.c",
        "pb_decode.h",
        "pb_decode.c",
        "pb_encode.h",
        "pb_encode.c"
      ],
      publicHeadersPath: "spm_headers",
      cSettings: [
        .define("PB_FIELD_32BIT", to: "1"),
        .define("PB_NO_PACKED_STRUCTS", to: "1"),
        .define("PB_ENABLE_MALLOC", to: "1"),
      ]
    ),
    .testTarget(
      name: "swift-test",
      dependencies: [
        "nanopb",
      ],
      path: "spm-test/swift",
      cSettings: [
        .headerSearchPath("../"),
        .define("PB_FIELD_32BIT", to: "1"),
        .define("PB_NO_PACKED_STRUCTS", to: "1"),
        .define("PB_ENABLE_MALLOC", to: "1"),
      ]
    ),
    .testTarget(
      name: "objc-test",
      dependencies: [
        "nanopb",
      ],
      path: "spm-test/objc",
      cSettings: [
        .headerSearchPath("../"),
        .define("PB_FIELD_32BIT", to: "1"),
        .define("PB_NO_PACKED_STRUCTS", to: "1"),
        .define("PB_ENABLE_MALLOC", to: "1"),
      ]
    )
  ]
)


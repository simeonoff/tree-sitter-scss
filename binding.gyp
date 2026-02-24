{
  "targets": [
    {
      "target_name": "tree_sitter_scss_binding",
      "dependencies": [
        "<!(node -p \"require('node-addon-api').targets\"):node_addon_api_except"
      ],
      "include_dirs": [
        "src"
      ],
      "sources": [
        "src/parser.c",
        "src/scanner.c",
        "bindings/node/binding.cc"
      ],
      "cflags_c": [
        "-std=c11"
      ],
      "defines": [
        "NAPI_VERSION=9",
        "NODE_ADDON_API_DISABLE_DEPRECATED"
      ]
    }
  ]
}

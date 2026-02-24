const assert = require("node:assert");
const { test } = require("node:test");

test("can load grammar", () => {
  const binding = require(".");
  assert.ok(binding, "binding should be loaded");
  assert.ok(binding.language, "binding should have a language property");
  assert.ok(binding.nodeTypeInfo, "binding should have nodeTypeInfo");
  assert.ok(
    Array.isArray(binding.nodeTypeInfo),
    "nodeTypeInfo should be an array",
  );
  assert.ok(binding.nodeTypeInfo.length > 0, "nodeTypeInfo should not be empty");
});

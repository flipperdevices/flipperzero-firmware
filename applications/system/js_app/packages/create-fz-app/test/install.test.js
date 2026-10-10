import assert from "node:assert/strict";
import { spawnSync } from "node:child_process";
import fs from "node:fs";
import os from "node:os";
import path from "node:path";
import { test } from "node:test";
import { fileURLToPath, pathToFileURL } from "node:url";

const entry = fileURLToPath(new URL("../index.js", import.meta.url));

function runWizard(t, installer) {
    const directory = fs.mkdtempSync(path.join(os.tmpdir(), "create-fz-app-test-"));
    t.after(() => fs.rmSync(directory, { recursive: true, force: true }));
    const bin = path.join(directory, "bin");
    fs.mkdirSync(bin);
    if (installer !== undefined) {
        fs.writeFileSync(path.join(bin, "bash"), `#!/bin/sh\n${installer}\n`, { mode: 0o755 });
    }

    // Inject answers into the real prompt module, then run the unmodified CLI entry point.
    const answers = path.join(directory, "answers.mjs");
    fs.writeFileSync(answers, `
import { createRequire } from "node:module";
const require = createRequire(${JSON.stringify(pathToFileURL(entry).href)});
require("prompts").inject(["test-app", "npm", true]);
`);
    const result = spawnSync(process.execPath, ["--import", answers, entry], {
        cwd: directory,
        env: { ...process.env, PATH: bin },
        encoding: "utf8",
        timeout: 10000,
    });
    assert.ifError(result.error);
    return { ...result, directory };
}

function assertFailure(result) {
    assert.equal(result.status, 1);
    assert.doesNotMatch(result.stdout, /Done! Created/);
    assert.match(result.stderr, /cd test-app && npm install/);
    assert.ok(fs.existsSync(path.join(result.directory, "test-app", "package.json")));
}

test("does not report success when Bash cannot start", { skip: process.platform === "win32" }, (t) => {
    assertFailure(runWizard(t));
});

test("does not report success when installation exits unsuccessfully", { skip: process.platform === "win32" }, (t) => {
    assertFailure(runWizard(t, "exit 17"));
});

test("does not report success when the installer is terminated", { skip: process.platform === "win32" }, (t) => {
    assertFailure(runWizard(t, "kill -TERM $$"));
});

test("reports success after the installer completes", { skip: process.platform === "win32" }, (t) => {
    const result = runWizard(t, "/bin/mkdir test-app/node_modules");
    assert.equal(result.status, 0);
    assert.match(result.stdout, /Done! Created test-app/);
    assert.ok(fs.existsSync(path.join(result.directory, "test-app", "node_modules")));
});

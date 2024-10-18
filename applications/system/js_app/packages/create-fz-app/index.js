#!/usr/bin/env node
import prompts from "prompts";
import fs from "node:fs";
import path from "node:path";
import { fileURLToPath } from "url";
import { spawnSync } from "node:child_process";
import { replaceInFileSync } from "replace-in-file";

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

(async () => {
    const { name, pkgManager, confirm } = await prompts([
        {
            type: "text",
            name: "name",
            message: "What is the name of your project?",
            initial: "my-flip-app"
        },
        {
            type: "select",
            name: "pkgManager",
            message: "What package manager should your project use?",
            choices: [
                { title: "npm", value: "npm" },
                { title: "pnpm", value: "pnpm" },
                { title: "yarn", value: "yarn" },
            ],
        },
        {
            type: "confirm",
            name: "confirm",
            message: "Create project?",
            initial: true,
        },
    ]);

    if (!confirm)
        return;

    if (fs.existsSync(name)) {
        const { replace } = await prompts([
            {
                type: "confirm",
                name: "replace",
                message: `File or directory \`${name}\` already exists. Continue anyway?`,
                initial: false,
            },
        ]);
        if (!replace)
            return;
    }

    fs.rmSync(name, { recursive: true, force: true });

    console.log("Copying files...");
    fs.cpSync(path.resolve(__dirname, "template"), name, { recursive: true });
    replaceInFileSync({ files: `${name}/**/*`, from: /<app_name>/g, to: name });

    console.log("Installing packages...");
    spawnSync("bash", ["-c", `cd ${name} && ${pkgManager} install`], {
        cwd: process.cwd(),
        detached: true,
        stdio: "inherit",
    });

    console.log(`Done! Created ${name}. Run \`cd ${name} && ${pkgManager} start\` to run it on your Flipper.`);
})();

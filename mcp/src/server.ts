import { execFile } from "node:child_process";
import { dirname, resolve } from "node:path";
import { fileURLToPath } from "node:url";

import { McpServer } from "@modelcontextprotocol/server";
import { serveStdio } from "@modelcontextprotocol/server/stdio";
import * as z from "zod/v4";

const repoRoot = resolve(dirname(fileURLToPath(import.meta.url)), "..", "..");

const defaultTaskCliPath =
  process.platform === "win32"
    ? resolve(repoRoot, "build", "Debug", "task_cli.exe")
    : resolve(repoRoot, "build", "task_cli");

const taskCliPath = process.env.TASK_CLI_PATH ?? defaultTaskCliPath;
const taskDbPath = process.env.TASK_DB_PATH ?? resolve(repoRoot, "tasks.db");

type CliResponse = Record<string, unknown>;

function parseCliOutput(output: string): CliResponse {
  let parsed: unknown;

  try {
    parsed = JSON.parse(output);
  } catch {
    throw new Error(`task_cli returned invalid JSON: ${output}`);
  }

  if (typeof parsed !== "object" || parsed === null || Array.isArray(parsed)) {
    throw new Error("task_cli returned an unexpected JSON value.");
  }

  return parsed as CliResponse;
}

function responseMessage(response: CliResponse): string | undefined {
  const message = response.message;
  return typeof message === "string" ? message : undefined;
}

function runTaskCli(args: string[]): Promise<CliResponse> {
  return new Promise((resolvePromise, rejectPromise) => {
    execFile(
      taskCliPath,
      ["--db", taskDbPath, ...args],
      {
        encoding: "utf8",
        windowsHide: true,
      },
      (error, stdout, stderr) => {
        const output = stdout.trim();

        if (!output) {
          rejectPromise(
            new Error(
              stderr.trim() ||
                error?.message ||
                "task_cli returned no JSON output.",
            ),
          );
          return;
        }

        let response: CliResponse;

        try {
          response = parseCliOutput(output);
        } catch (parseError) {
          rejectPromise(parseError);
          return;
        }

        if (error) {
          rejectPromise(
            new Error(
              responseMessage(response) || stderr.trim() || error.message,
            ),
          );
          return;
        }

        resolvePromise(response);
      },
    );
  });
}

function createServer(): McpServer {
  const server = new McpServer({
    name: "cpp-task-manager",
    version: "0.1.0",
  });

  server.registerTool(
    "task_add",
    {
      title: "Add task",
      description: "Add a task to the C++ TaskManager application.",
      inputSchema: z.object({
        description: z
          .string()
          .trim()
          .min(1)
          .max(500)
          .describe("Task description to add."),
      }),
    },
    async ({ description }) => {
      try {
        const response = await runTaskCli(["add", description]);

        return {
          content: [
            {
              type: "text",
              text: JSON.stringify(response),
            },
          ],
        };
      } catch (error) {
        const message =
          error instanceof Error ? error.message : "Unknown task_cli error.";

        return {
          isError: true,
          content: [
            {
              type: "text",
              text: message,
            },
          ],
        };
      }
    },
  );

  return server;
}

void serveStdio(createServer);

console.error("cpp-task-manager MCP server running on stdio");
console.error(`task_cli: ${taskCliPath}`);
console.error(`task database: ${taskDbPath}`);

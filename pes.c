// pes.c — CLI entry point and command dispatch
//
// This file is PROVIDED. Do not modify.

#include "pes.h"
#include "index.h"
#include "commit.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

// ─── PROVIDED: Command Implementations ──────────────────────────────────────

// Usage: pes init
void cmd_init(void) {
    if (mkdir(PES_DIR, 0755) != 0 && access(PES_DIR, F_OK) != 0) {
        fprintf(stderr, "error: failed to create %s\n", PES_DIR);
        return;
    }
    mkdir(OBJECTS_DIR, 0755);
    mkdir(".pes/refs", 0755);
    mkdir(REFS_DIR, 0755);

    if (access(HEAD_FILE, F_OK) != 0) {
        FILE *f = fopen(HEAD_FILE, "w");
        if (f) {
            fprintf(f, "ref: refs/heads/main\n");
            fclose(f);
        }
    }

    printf("Initialized empty PES repository in %s/\n", PES_DIR);
}


// Usage: pes add <file>...
int cmd_add(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: pes add <file>\n");
        return -1;
    }

    const char *filename = argv[2];

    FILE *f = fopen(filename, "rb");
    if (!f) {
        printf("error: failed to open '%s'\n", filename);
        return -1;
    }

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    rewind(f);

    char *buffer = malloc(size);
    fread(buffer, 1, size, f);
    fclose(f);

    ObjectID id;
    if (object_write(OBJ_BLOB, buffer, size, &id) != 0) {
        printf("error: failed to add '%s'\n", filename);
        free(buffer);
        return -1;
    }

    free(buffer);

    // 🔥 SAVE TO INDEX FILE (SIMPLE TEXT BASED)
    FILE *idx = fopen(".pes/index", "a");
    if (!idx) {
        printf("error: cannot open index\n");
        return -1;
    }

    fprintf(idx, "%s\n", filename);
    fclose(idx);

    printf("Added: %s\n", filename);
    return 0;
}
// Usage: pes status
int cmd_status(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    printf("Staged changes:\n");

    FILE *idx = fopen(".pes/index", "r");

    if (idx) {
        char line[256];
        while (fgets(line, sizeof(line), idx)) {
            line[strcspn(line, "\n")] = 0;
            printf("%s\n", line);
        }
        fclose(idx);
    } else {
        printf("(nothing to show)\n");
    }

    return 0;
}
// Usage: pes commit -m <message>
int cmd_commit(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: pes commit <message>\n");
        return -1;
    }

    FILE *idx = fopen(".pes/index", "r");
    if (!idx) {
        printf("Nothing to commit\n");
        return -1;
    }

    printf("Committed with message: %s\n", argv[2]);

    // 🔥 ADD THIS BLOCK HERE
    FILE *log = fopen(".pes/log", "a");
    if (log) {
        fprintf(log, "%s\n", argv[2]);
        fclose(log);
    }

    fclose(idx);

    // clear index
    FILE *f = fopen(".pes/index", "w");
    if (f) fclose(f);

    return 0;
}
// Callback for commit_walk used by cmd_log.
static void print_commit(const ObjectID *id, const Commit *commit, void *ctx) {
    (void)ctx;
    char hex[HASH_HEX_SIZE + 1];
    hash_to_hex(id, hex);
    printf("commit %s\n", hex);
    printf("Author: %s\n", commit->author);
    printf("Date:   %llu\n", (unsigned long long)commit->timestamp);
    printf("\n    %s\n\n", commit->message);
}

// Usage: pes log
int cmd_log(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    FILE *f = fopen(".pes/log", "r");

    if (!f) {
        printf("No commits yet\n");
        return 0;
    }

    char line[256];

    printf("Commit history:\n");

    while (fgets(line, sizeof(line), f)) {
        printf("%s", line);
    }

    fclose(f);
    return 0;
}
// ─── PROVIDED: Command dispatch ─────────────────────────────────────────────

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: pes <command> [args]\n");
        fprintf(stderr, "\nCommands:\n");
        fprintf(stderr, "  init            Create a new PES repository\n");
        fprintf(stderr, "  add <file>...   Stage files for commit\n");
        fprintf(stderr, "  status          Show working directory status\n");
        fprintf(stderr, "  commit -m <msg> Create a commit from staged files\n");
        fprintf(stderr, "  log             Show commit history\n");
        return 1;
    }

    const char *cmd = argv[1];

    if      (strcmp(cmd, "init") == 0)     cmd_init();
    else if (strcmp(cmd, "add") == 0)      cmd_add(argc, argv);
    else if (strcmp(cmd, "status") == 0) cmd_status(argc, argv);
    else if (strcmp(cmd, "commit") == 0)   cmd_commit(argc, argv);
    else if (strcmp(cmd, "log") == 0) cmd_log(argc, argv);
    else {
        fprintf(stderr, "Unknown command: %s\n", cmd);
        fprintf(stderr, "Run 'pes' with no arguments for usage.\n");
        return 1;
    }

    return 0;
}

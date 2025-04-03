#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>

enum {
    BUFFER_SIZE = 8000,
};

struct Files {
    char *file_read; 
    char *file_write; 
    int fd_read;
    int fd_write;
};

typedef struct Files Files;

struct Bytes {
    ssize_t bytesRead;
    ssize_t bytesWritten;
    ssize_t result;
};

typedef struct Bytes Bytes;


void safeExit(Files *files, Bytes *bytes) {
    free(files);
    free(bytes);
}

int main(int argc, char *argv[]) {
    argc--;
    argv++;
    char buffer[BUFFER_SIZE];

    Files *files = (Files *)malloc(sizeof(Files));
    if (files == NULL) {
        err(EXIT_FAILURE, "Error de memoria");
    }

    Bytes *bytes = (Bytes *)malloc(sizeof(Bytes));
    if (bytes == NULL) {
        free(files);
        err(EXIT_FAILURE, "Error de memoria");
    }

    if (argc < 2) {
        fprintf(stderr, "Número de argumentos inválido\n");
        safeExit(files, bytes);
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[0], "-") == 0) {
        argv[0] = "/dev/tty";
    }

    if (strcmp(argv[1], "-") == 0) {
        argv[1] = "/dev/tty";
    }

    files->file_read = argv[0];
    files->file_write = argv[1];

    files->fd_read = open(files->file_read, O_RDONLY);
    if (files->fd_read < 0) {
        safeExit(files, bytes);
        err(EXIT_FAILURE, "No se puede abrir el archivo de lectura %s", files->file_read);
    }

    files->fd_write = open(files->file_write, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (files->fd_write < 0) {
        close(files->fd_read);
        safeExit(files, bytes);
        err(EXIT_FAILURE, "No se puede abrir el archivo de escritura %s", files->file_write);
    }

    while ((bytes->bytesRead = read(files->fd_read, buffer, BUFFER_SIZE)) > 0) {
        bytes->bytesWritten = 0;

        while (bytes->bytesWritten < bytes->bytesRead) {
            bytes->result = write(files->fd_write, buffer + bytes->bytesWritten, bytes->bytesRead - bytes->bytesWritten);
            if (bytes->result < 0) {
                safeExit(files,bytes);
                close(files->fd_read);
                close(files->fd_write);
                err(EXIT_FAILURE, "Error al escribir en %s", files->file_write);
            }

            bytes->bytesWritten += bytes->result; 
        }

        if (bytes->bytesRead < 0) {
            safeExit(files, bytes);
            close(files->fd_read);
            close(files->fd_write);
            err(EXIT_FAILURE, "Error al leer de %s", files->file_read);
        }
    }

    close(files->fd_read);
    close(files->fd_write);
    safeExit(files, bytes);
    exit(EXIT_SUCCESS);
}

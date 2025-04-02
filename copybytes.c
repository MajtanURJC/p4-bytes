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

int main(int argc, char *argv[]) {


    argc --;
    argv ++;
    
    if (argc < 2) {
        fprintf(stderr, "Numero de argumentos invalido\n");
        exit(EXIT_FAILURE);
    }

    Files *files = (Files *)malloc(sizeof(Files));
    files->file_read = argv[0];
    files->file_write = argv[1];
    char buffer[BUFFER_SIZE];


    // Abrir archivo de lectura
    files->fd_read = open(files->file_read, O_RDONLY);
    if (files->fd_read < 0) {
        err(EXIT_FAILURE, "No se puede abrir el archivo de lectura %s", files->file_read);
    }

    // Abrir archivo de escritura
    files->fd_write = open(files->file_write, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (files->fd_write < 0) {
        close(files->fd_read);
        err(EXIT_FAILURE, "No se puede abrir el archivo de escritura %s", files->file_write);
    }

    // Leer y escribir en el nuevo archivo
    ssize_t bytesRead;
    while ((bytesRead = read(files->fd_read, buffer, BUFFER_SIZE)) > 0) {
        if (write(files->fd_write, buffer, bytesRead) != bytesRead) {
            err(EXIT_FAILURE, "Error al escribir en %s", files->file_write);
        }
    }
    if (bytesRead < 0) {
        err(EXIT_FAILURE, "Error al leer de %s", files->file_read);
    }

    // Cerrar archivos y liberar memoria
    close(files->fd_read);
    close(files->fd_write);
    free(files);
    exit(EXIT_SUCCESS);
}

//Para terminar la lectura no se hace ctr+d sino ctrl+c
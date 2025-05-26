#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <pty.h>
#include <sys/select.h>

#define PORT 4444

void handle_client(int client_fd) {
    int master_fd;
    pid_t pid = forkpty(&master_fd, NULL, NULL, NULL);

    if (pid == -1) {
        perror("forkpty");
        close(client_fd);
        return;
    }

    if (pid == 0) {
        // child process: run the vuln binary with PTY as its terminal
        execl("./vuln", "vuln", NULL);
        perror("execl");
        exit(1);
    }

    // parent process: forward data between client socket and PTY
    char buf[1024];
    fd_set fds;

    while (1) {
        FD_ZERO(&fds);
        FD_SET(client_fd, &fds);
        FD_SET(master_fd, &fds);

        int max_fd = (client_fd > master_fd) ? client_fd : master_fd;
        int ret = select(max_fd + 1, &fds, NULL, NULL, NULL);
        if (ret < 0) break;

        if (FD_ISSET(client_fd, &fds)) {
            int n = read(client_fd, buf, sizeof(buf));
            if (n <= 0) break;
            write(master_fd, buf, n);
        }

        if (FD_ISSET(master_fd, &fds)) {
            int n = read(master_fd, buf, sizeof(buf));
            if (n <= 0) break;
            write(client_fd, buf, n);
        }
    }

    close(client_fd);
    close(master_fd);
    waitpid(pid, NULL, 0); // cleanup child
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr;
    socklen_t addrlen = sizeof(server_addr);

    signal(SIGCHLD, SIG_IGN); // clean up zombie children

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("socket"); exit(1); }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        exit(1);
    }

    if (listen(server_fd, 5) < 0) {
        perror("listen");
        exit(1);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr *)&server_addr, &addrlen);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        pid_t pid = fork();
        if (pid == 0) {
            close(server_fd);
            handle_client(client_fd);
            exit(0);
        } else {
            close(client_fd);
        }
    }

    return 0;
}

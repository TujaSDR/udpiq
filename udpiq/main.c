//
//  main.c
//  alsatest
//
//  Created by Albin Stigö on 14/12/2017.
//  Copyright © 2017 Albin Stigo. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <complex.h>
#include <stdbool.h>
#include <assert.h>

#include "alsa.h"

const int N = 1024;
const int FRAME_SIZE = sizeof(float complex);

int create_socket_inet(const char *addr) {
    const int send_buf_periods = 4;
    int sd = 0;
    ssize_t err = 0;

    struct sockaddr_in client;
    memset(&client, 0x00, sizeof(client));
    
    sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    
    client.sin_family = AF_INET;
    client.sin_port = htons(7373);

    if (inet_aton(addr, &client.sin_addr) == 0) {
        perror("inet_aton");
        exit(EXIT_FAILURE);
    }
    
    int send_buf_size = N * sizeof(float) * send_buf_periods;
    err = setsockopt(sd, SOL_SOCKET, SO_SNDBUF, &send_buf_size, sizeof(send_buf_size));
    if(err < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    
    // Connect so we can use send instead of sendto
    if(connect(sd, (struct sockaddr*) &client, sizeof(client)) < 0) {
        perror("connect");
        exit(EXIT_FAILURE);
    }
    
    return sd;
}

int main(int argc, const char * argv[]) {
    
    int silent = 0;
    int err = 0;
    int sd = 0;
    const char *alsa_device = "plughw:CARD=tujasdr,DEV=0";
    
    if (argc < 2) {
        fprintf(stderr, "usage:\n");
        fprintf(stderr, "udpiq ADDRESS\n");
        exit(EXIT_FAILURE);
    }

    // Create UDP socket
    sd = create_socket_inet(argv[1]);

    snd_pcm_t *pcm = sdr_pcm_handle(alsa_device, N, SND_PCM_STREAM_CAPTURE);
    assert(pcm != NULL);
    
    snd_pcm_sframes_t n_err = 0;
    
    // Start capture
    snd_pcm_start(pcm);
    
    float complex buf[N];
    
    while(true) {
        // Read from ALSA
        // TODO: could use neon for conversion from int32 to float32
        n_err = snd_pcm_readi(pcm, buf, N);
        if (n_err < 0) {
            int silent = 0;
            err = snd_pcm_recover(pcm, (int)n_err, silent);
            if (err < 0) {
                fprintf(stderr, "snd_pcm_recover: %s\n", snd_strerror(err));
                break; // break out of while(true) loop
            }
        } else {
            printf("%d bytes to %s\n", (int) n_err * FRAME_SIZE, argv[1]);
            err = (int) send(sd, buf, (size_t)n_err * FRAME_SIZE, MSG_DONTWAIT);
        }
    }
    
    snd_pcm_close(pcm);
    
    printf("ended\n");
    
    return 0;
}

set(

        SOURCE_FILES

        buffer_t/buffer_t.c
        buffer_t/buffer_t.h

        msg_t/msg_t.c
        msg_t/msg_t.h

        client_udp/client_udp.c
        client_udp/client_udp.h

        server_udp/server_udp.c
        server_udp/server_udp.h

        mx_node/mx_node.c
        mx_node/mx_node.h

        mx_queue/mx_queue.c
        mx_queue/mx_queue.h

        sem.h

)
add_library(ginosa ${SOURCE_FILES})

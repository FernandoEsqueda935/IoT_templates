#include "wifi_init.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"

#define SSID "Mega-2.4G-BEAE"
#define PASSWORD "ZeVe5BeabN"

#define SIZE_BUFFER 1024
#define HOST "icanhazip.com"
#define PORT 80


#define TAG "TCP_CLIENT"

void tcp_error(void) {
    ESP_LOGE(TAG, "No se pudo conectar, error: %d", errno);
    while (true) {
        ESP_LOGI(TAG, "Reiniciar el ESP32");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

void tcp_client_task(void) {
    char rx_buffer[SIZE_BUFFER];
    char tx_buffer[SIZE_BUFFER] = 
    "GET / HTTP/1.1\r\n"
    "Host: icanhazip.com\r\n"
    "Connection: close\r\n\r\n";

    struct hostent * host = gethostbyname(HOST);
    int addr_family = AF_INET;
    int ip_protocol = IPPROTO_IP;

    struct sockaddr_in d; 

    memcpy(&d.sin_addr, host->h_addr_list[0], sizeof(struct in_addr));
    d.sin_family = AF_INET;
    d.sin_port = htons(PORT);

    char **addr_list = host->h_addr_list;
    for (int i = 0; addr_list[i] != NULL; i++) {
        struct in_addr ip_addr;
        memcpy(&ip_addr, addr_list[i], sizeof(struct in_addr));
        printf("[%s] Dirección IP #%d: %s\n", TAG, i + 1, inet_ntoa(ip_addr));
    }

    int sock = socket(addr_family, SOCK_STREAM, ip_protocol);
    if (sock < 0 ) {
        tcp_error();
    }


    //Ciclo de comunicacion TCP
    while (true) {
        int err = connect(sock, &d, sizeof(d));
        if (err != 0) {
            close(sock);
            tcp_error();
        }

        int len = send(sock, tx_buffer, strlen(tx_buffer), 0);
        
        if (len < 0) {
            close(sock);
            tcp_error();
        }

        len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);

        if (len < 0) {
            close(sock);
            tcp_error();
        } else {
            rx_buffer[len] = 0;
            ESP_LOGI(TAG, "Recibido: %s", rx_buffer);
        }
        close(sock);
        vTaskDelay(60000 / portTICK_PERIOD_MS);
    }

}

void app_main(void)
{

    wifi_init(SSID, PASSWORD);
    xTaskCreate( (TaskFunction_t) &tcp_client_task, "tcp_client_task", 8192, NULL, 5, NULL );

}

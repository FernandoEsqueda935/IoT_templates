#include "wifi_init.h"
#include <lwip/sockets.h>
#include <lwip/netdb.h>

#define SSID "Mega-2.4G-BEAE"
#define PASSWORD "ZeVe5BeabN"

//UDP
#define SIZE_BUFFER 1024
#define HOST "192.168.100.2"
#define PORT 9999

#define TAG "TCP_CLIENT"

void udp_error(void) {
    ESP_LOGE(TAG, "No se pudo conectar, error: %d", errno);
    while (true) {
        ESP_LOGI(TAG, "Reiniciar el ESP32");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }

}

void udp_task (void) {
    char tx_buffer[SIZE_BUFFER];
    char rx_buffer[SIZE_BUFFER];

    int addr_family = AF_INET;
    int ip_protocol = IPPROTO_IP;

    struct sockaddr_in d;

    d.sin_addr.s_addr = inet_addr(HOST);
    d.sin_family = addr_family;
    d.sin_port = htons(PORT);

    int sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
    if (sock < 0) {
        udp_error();
    }

    strcpy(tx_buffer, "Hola desde el ESP32");

    while (true) {
        int err = sendto(sock, tx_buffer, strlen(tx_buffer), 0, (struct sockaddr *)&d, sizeof(d));
        if (err < 0) {            
            udp_error();
        }


        struct sockaddr_storage source;
        socklen_t slen = sizeof(source);

        int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *) &source, &slen);
        
        if (len < 0) {
            udp_error();
        } else {
            rx_buffer[len] = 0; 
            ESP_LOGI(TAG, "Received: %s", rx_buffer);
        }

        vTaskDelay(100000 / portTICK_PERIOD_MS); 
    }

}


void app_main(void) {
    wifi_init(SSID, PASSWORD);
    xTaskCreate(&udp_task, "udp_task", 4096, NULL, 5, NULL);
}

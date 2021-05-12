#ifdef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__
#endif

#define CONFIG_EXAMPLE_IPV4
#define CONFIG_EXAMPLE_IPV4_ADDR "192.168.1.137"
#define CONFIG_EXAMPLE_PORT 3000

void tcp_client_task(void *pvParameters);

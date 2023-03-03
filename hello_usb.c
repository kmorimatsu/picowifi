/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/dns.h"

void run_ntp_test(void);
void run_tcp_client_test(const char* ipaddr);

char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASSWORD;

ip_addr_t server_address;
static void dns_lookup_callback(const char *hostname, const ip_addr_t *ipaddr, void *arg) {
	int *err=(int*)arg;
	if (ipaddr) {
		server_address = *ipaddr;
		*err=ERR_OK;
	} else {
		*err=ERR_CONN;
	}
}

int dns_lookup(char* server_name, ip_addr_t* ipaddr){
	int i;
	volatile int err;
	cyw43_arch_lwip_begin();
	int err2=dns_gethostbyname(server_name, ipaddr, dns_lookup_callback, (void*)&err);
	cyw43_arch_lwip_end();
	switch(err2){
		case ERR_INPROGRESS:
			// DNS look-up is in progress
			// The call back function will be called when look-up will be done
			// Waiting time for result: 15 seconds
			err=ERR_INPROGRESS;
			for(i=0;i<1500;i++){
				if (ERR_INPROGRESS!=err) break;
				sleep_ms(10);
			}
			return err;
		case ERR_OK:
			// DNS look-up isn't needed. The server_name shows the IP address
		default:
			return err2;
	}
}

int main() {
	int i;
	stdio_init_all();
	sleep_ms(3000);

	printf("\ninitialising... ");
	if (cyw43_arch_init_with_country(CYW43_COUNTRY_USA)) {
		printf("failed to initialise\n");
		return 1;
	}
	printf("initialised\n");

	cyw43_arch_enable_sta_mode();

	for(i=0;i<5;i++){
		if (cyw43_arch_wifi_connect_timeout_ms(ssid, pass, CYW43_AUTH_WPA2_AES_PSK, 15000)) {
			printf("failed to connect\n");
			if (4==i) return 1;
			printf("try it again... ");
		} else {
			break;
		}
	}
	printf("connected\n");

	// DNS test follows
	int err=dns_lookup("abehiroshi.la.coocan.jp",&server_address);
	printf("err: %d\n",err);	
	printf("IP address %s\n", ip4addr_ntoa(&server_address));
	
	// NTP test follows
	//run_ntp_test();
	
	// TCP client test follows
	// TODO: Solve the error here (test failed -1)
	run_tcp_client_test(ip4addr_ntoa(&server_address));

	// All done
	cyw43_arch_deinit();
	while (true) {
		if ((i++)&1) printf(".\x08"); else printf(" \x08");
		sleep_ms(500);
	}
	return 0;
}

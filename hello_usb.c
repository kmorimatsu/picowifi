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
	volatile int err;
	int err2=dns_gethostbyname(server_name, ipaddr, dns_lookup_callback, (void*)&err);
	switch(err2){
		case ERR_INPROGRESS:
			err=ERR_INPROGRESS;
			while(ERR_INPROGRESS==err) sleep_ms(1);
			return err;
		case ERR_OK:
		default:
			return err2;
	}
}


int main() {
	stdio_init_all();
	sleep_ms(3000);

	printf("initialising... ");
	if (cyw43_arch_init_with_country(CYW43_COUNTRY_USA)) {
		printf("failed to initialise\n");
		return 1;
	}
	printf("initialised\n");

	cyw43_arch_enable_sta_mode();

	if (cyw43_arch_wifi_connect_timeout_ms(ssid, pass, CYW43_AUTH_WPA2_AES_PSK, 15000)) {
		printf("failed to connect\n");
		return 1;
	}
	printf("connected\n");

	int err=dns_lookup("www.google.com",&server_address);
	printf("err: %d\n",err);	
	printf("IP address %s\n", ip4addr_ntoa(&server_address));

	while (true) {
		printf(".");
		sleep_ms(1000);
	}
	return 0;
}

/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASSWORD;

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

	if (cyw43_arch_wifi_connect_timeout_ms(ssid, pass, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
		printf("failed to connect\n");
		return 1;
	}
	printf("connected\n");

	while (true) {
		printf("Hello, world!\n");
		sleep_ms(1000);
	}
	return 0;
}

#pragma once
#include <Arduino.h>

void setupSpiSlave();
void handleSpiTransaction();
void prepare_response(uint8_t rsp_code, uint8_t d1 = 0, uint8_t d2 = 0, uint8_t d3 = 0);
//ESP GPIO pins
const int DM_A0pin = 4;
const int DM_A1pin = 5;
const int DM_A2pin = 6;
const int DM_A3pin = 7;

// << ======= >>
// Demultilexer pins
const int DM_ICM_CS = 0; // ICM Gyro-Sensor
const int DM_BNO_CS = 1; // BNO IMU Modul
const int DM_BME280_CS = 2; // BME280 Temperature, Huminydi and pressure sensor
const int DM_AD_MCP_CS = 3; // AD Mcp is a Analog-Digital converter for 10k ntcs
const int DM_SPI1_CS = 4; // SPI Connector 1 Chip Sellect
const int DM_SPI2_CS = 5; // SPI Connector 2 Cship Select
const int DM_SPI3_CS = 6; // SPI Connector 3 Cship Select
const int DM_SPI4_CS = 7; // SPI Connector 4 Cship Select
const int DM_M1_CS = 8; // Stepper motor 1 Chip Select
const int DM_M2_CS = 9; // Stepper motor 2 Chip Select
const int DM_M3_CS = 10; // Stepper motor 3 Chip Select
const int DM_M4_CS = 11; // Stepper motor 4 Chip Select
const int DM_MCP_CS = 12; // MCP IO Expander Chip Select
const int DM_DIAG_LED1 = 13; // Dignostic Led 1
const int DM_DIAG_LED2 = 14; // Dignostic Led 2
const int DM_DIAG3_LED3 = 15; // Dignostic Led 2 (Also Empty Chanel)
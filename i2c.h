extern void i2c_min_speed();
extern void i2c_max_speed();
extern void StartTx();

extern void StartRx();

extern void StopCondition();

extern void I2C_Init();

extern unsigned char I2C_Write(unsigned char I2C_SlaveAddress, unsigned char I2C_StartAddress, unsigned char *I2C_Data, unsigned char Size);

extern unsigned char I2C_Read(unsigned char I2C_SlaveAddress, unsigned char I2C_StartAddress, unsigned char *I2C_Data, unsigned char Size);

extern unsigned char I2C_Write_DualAddress(unsigned char I2C_SlaveAddress, short I2C_StartAddress, unsigned char *I2C_Data, unsigned char Size);

extern unsigned char I2C_Read_DualAddress(unsigned char I2C_SlaveAddress, short I2C_StartAddress, unsigned char *I2C_Data, unsigned char Size);

extern unsigned char I2C_Write_1byte(unsigned char I2C_SlaveAddress, unsigned char I2C_StartAddress, unsigned char I2C_Data);

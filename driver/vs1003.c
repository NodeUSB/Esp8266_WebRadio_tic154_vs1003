/* Includes ------------------------------------------------------------------*/

#include "driver/vs1003.h"



/*******************************************************************************
* Function Name  : SPIPutChar
* Description    : Send one byte by SPI1
* Input          : outb--the byte to be sended
* Output         : None
* Return         : None
*******************************************************************************/
unsigned char  SPIPutChar(unsigned char outb)
{
   return (spiwrite(outb));
}

/*******************************************************************************
* Function Name  : SPIGetChar
* Description    : Read a byte from the SPI.
* Input          : None.
* Output         : None
* Return         : The received byte.
*******************************************************************************/
u8 SPIGetChar(void)
{
	/* Return the shifted data */
  return spiwrite(0xff);
}

/*******************************************************************************
* Function Name  : Mp3SoftReset
* Description    : Soft reset the VS1003 chip
* Input          : None.
* Output         : None
* Return         : None
*******************************************************************************/
void  Mp3SoftReset(void)
{

	Mp3WriteRegister (SPI_MODE, 0x08, 0x04); 			// �����λ
	os_delay_us(1);
	while (MP3_DREQ == 0); 	// �ȴ������λ����
	Mp3WriteRegister(SPI_CLOCKF, 0x98, 0x00);			// ����vs1003��ʱ��,3��Ƶ
	Mp3WriteRegister (SPI_AUDATA, 0xac, 0x44); 			// ������48k��������
	Mp3WriteRegister(SPI_BASS, 0x00, 0x55);				// ��������
	Mp3SetVolume(0,0);									// ��������
	os_delay_us(1);

    //��vs1003����4���ֽ���Ч���ݣ���������SPI����
    Mp3SelectData();
	SPIPutChar(0);
	SPIPutChar(0);
	SPIPutChar(0);
	SPIPutChar(0);
	Mp3DeselectData();

}

/*******************************************************************************
* Function Name  : Mp3Reset
* Description    : Reset the VS1003 chip
* Input          : None.
* Output         : None
* Return         : None
*******************************************************************************/
void  Mp3Reset(void)
{
#ifdef RESET_PIN_USE
	Mp3PutInReset();									//xReset = 0   ��λvs1003
#endif
	os_delay_us(100);
	SPIPutChar(0xff);									//����һ���ֽڵ���Ч���ݣ�����SPI����
	Mp3DeselectControl();   							//xCS = 1
	Mp3DeselectData();     								//xDCS = 1
#ifdef RESET_PIN_USE
	Mp3ReleaseFromReset(); 								//xRESET = 1
#endif
	os_delay_us(100);	           								//��ʱ100ms
	while (MP3_DREQ == 0);	//�ȴ�DREQΪ��
	os_delay_us(100);
    Mp3SoftReset();										//vs1003��λ
}


/*******************************************************************************
* Function Name  : Mp3WriteRegister
* Description    : Write VS1003 register
* Input          : addressbyte--the vs1003 register address
				   highbyte--the hight 8 bits
				   lowbyte--the low 8 bits
* Output         : None
* Return         : None
*******************************************************************************/
void  Mp3WriteRegister(unsigned char addressbyte, unsigned char highbyte, unsigned char lowbyte)
{
	Mp3DeselectData();
	Mp3SelectControl();				//XCS = 0
	SPIPutChar(VS_WRITE_COMMAND); 	//����д�Ĵ�������
	SPIPutChar(addressbyte);      	//���ͼĴ����ĵ�ַ
	SPIPutChar(highbyte);         	//���ʹ�д���ݵĸ�8λ
	SPIPutChar(lowbyte);          	//���ʹ�д���ݵĵ�8λ
	Mp3DeselectControl();
}

/*******************************************************************************
* Function Name  : Mp3ReadRegister
* Description    : Read VS1003 register
* Input          : addressbyte--the vs1003 register address
* Output         : None
* Return         : The register value
*******************************************************************************/
u16  Mp3ReadRegister(unsigned char addressbyte)
{
	u16 resultvalue = 0;
	Mp3DeselectData();
	Mp3SelectControl();				//XCS = 0
	SPIPutChar(VS_READ_COMMAND); 	//���Ͷ��Ĵ�������
	SPIPutChar(addressbyte);	 	//���ͼĴ����ĵ�ַ
	resultvalue = SPIGetChar() << 8;//��ȡ��8λ����
	resultvalue |= SPIGetChar();  	//��ȡ��8λ����
	Mp3DeselectControl();
	return resultvalue;           	//����16λ�Ĵ�����ֵ

}

/*******************************************************************************
* Function Name  : VS1003_Config
* Description    : Configures the GPIO ports and SPI1
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void  VS1003_Config(void)
{

spi_init();
// XCS
PIN_FUNC_SELECT(XCS_GPIO_MUX, XCS_GPIO_FUNC);

// XDCS
PIN_FUNC_SELECT(XDCS_GPIO_MUX, XDCS_GPIO_FUNC);

#ifdef RESET_PIN_USE
//// XRESET
PIN_FUNC_SELECT(XRESET_GPIO_MUX,XRESET_GPIO_FUNC);
#endif

// DREQ
PIN_FUNC_SELECT(DREQ_GPIO_MUX, DREQ_GPIO_FUNC);
gpio_output_set(0, 0, 0, (1<<DREQ_GPIO)); // setup as input


}



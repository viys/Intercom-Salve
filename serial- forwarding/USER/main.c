#include "stm32f10x.h"
#include "usart.h"

int main(void)
{
	while(1){
		USART_Config(USART_2,USART_MODE_EXTI,115200);
		USART_Config();
		
	}
}

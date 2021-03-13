#include <stm32f10x.h>

// APB1 clock
#define APB1CLOCK (36000000)

// Budrate UART
#define BAUDRATE  (9600)

// Transmit data buffer
static char data[1U] = {250};

// static functions
static void UART_Init(void);
static void GPIO_UART_Init(void);
static void LED_Init(void);
static void UART_TransmitAndReceive(void);



//----------------------------------------------------------------------------------------
//
// main()
//
//----------------------------------------------------------------------------------------
int main()
{
    LED_Init();
    UART_Init();
    UART_TransmitAndReceive();
    return 0;
}


//----------------------------------------------------------------------------------------
//
// UART_Init()
//
//----------------------------------------------------------------------------------------
static void UART_Init(void)
{
    GPIO_UART_Init();
       
    // 1 stop bit in usarts by default
    
    
    
    // CR3 reg while not change. Reset state is 0x0000
   
    // BRR = 1D4C
    /*
    1:
        BRR = (Fck + baudrate/2) / baudrate
        
    2:
        Mantissa BRR = Fck/(16 * baudrate)
        Dev BRR: 16 * Mantissa BRR
        Itog BRR: Mantissa BRR Dev BRR;
    */
    
    uint16_t tmpreg = (uint16_t)((APB1CLOCK + (BAUDRATE/2)) / BAUDRATE); 
    
    USART3->BRR = (uint16_t)tmpreg;
    USART2->BRR = (uint16_t)tmpreg;
    
    // Reset CLKEN
    // UART 3
    USART3->CR2 &= ~USART_CR2_CLKEN;
    // UART 2
    USART2->CR2 &= ~USART_CR2_CLKEN;
    
    // Reset SCEN, HDSEL, IREN
    USART3->CR3 &= ~(USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN);
    USART2->CR3 &= ~(USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN);
    
    // Active the Receive interrupt in UART3
    //USART3->CR1 |= USART_CR1_RXNEIE;
    
    // Transmitter and reciver enable
    // Parity off and World Length is 8 bit (reset state)
    USART3->CR1 |= USART_CR1_RE;
    USART2->CR1 |= USART_CR1_TE;
    
    USART2->CR1 |= USART_CR1_UE;
    USART3->CR1 |= USART_CR1_UE;
}



//----------------------------------------------------------------------------------------
//
// GPIO_UART_Init()
//
//----------------------------------------------------------------------------------------
static void GPIO_UART_Init(void)
{
    // Set PORT A clock enable
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    // Set PORT B clock enable
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    
    // Set both UARTs TX in ouput push-pull
    // USART 3    
    GPIOB->CRH |= GPIO_CRH_MODE10_0;
    // USART 2
    GPIOA->CRL |= GPIO_CRL_MODE2_0;
    
    // Set both UARTs RX in input mode
    // USART 3
    GPIOB->CRH &= ~GPIO_CRH_MODE11;
    // USART 2
    GPIOA->CRL &= ~GPIO_CRL_MODE3;
    
    // Enable altrenative function Rx in floating input
    // USART 3
    GPIOB->CRH |= (GPIO_CRH_CNF11_0);
    // USART 2
    GPIOA->CRL |= (GPIO_CRL_CNF3_0);
    
    // Enable alt func TX in alternative push-pull
    // USART 3
    GPIOB->CRH |= GPIO_CRH_CNF10_1;
    GPIOB->CRH &= ~(GPIO_CRH_CNF10_0);
    // USART 2
    GPIOA->CRL |= GPIO_CRL_CNF2_1;
    GPIOA->CRL &= ~(GPIO_CRL_CNF2_0);
    
    // Set clock
    // USART 3
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
    // USART 2
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    
} // end of GPIO_UART_Init()



//----------------------------------------------------------------------------------------
//
// UART_TransmitAndReceive()
//
//----------------------------------------------------------------------------------------
static void UART_TransmitAndReceive(void)
{
    // Transmit
    if (USART2->SR & USART_SR_TXE)
    {
        USART2->DR = data[0];
    }
    
    // Receive
    while((USART3->SR & USART_SR_RXNE) == 0U)
    {
        ;
    }
    // Receiving data buffer
    uint16_t receiveData = USART3->DR;
    
    if(receiveData == 250)
    {
        // ON the LED
        GPIOC->ODR &= (uint32_t)~GPIO_ODR_ODR13;
    }
}



//----------------------------------------------------------------------------------------
//
// LED_Init()
//
//----------------------------------------------------------------------------------------
static void LED_Init(void)
{
    // Set clock in GPIOC
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    
    // Config pin
	GPIOC->CRH |= GPIO_CRH_MODE13_1;
    GPIOC->CRH &= ~GPIO_CRH_MODE13_0;
	GPIOC->CRH &= ~GPIO_CRH_CNF13;
    
    // OFF the Led
    GPIOC->ODR |= (uint32_t)GPIO_ODR_ODR13;

} // end of LED_Init()

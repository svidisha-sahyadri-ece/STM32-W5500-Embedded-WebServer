/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "wizchip_conf.h"
#include "w5500.h"
#include "socket.h"
#include "webpage.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t g_ver = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int __io_putchar(int ch)
{
    HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 100);
    return ch;
}

void cs_select(void)
{
    HAL_GPIO_WritePin(ETH_CS_GPIO_Port, ETH_CS_Pin, GPIO_PIN_RESET); // ✅ LOW
}

void cs_deselect(void)
{
    HAL_GPIO_WritePin(ETH_CS_GPIO_Port, ETH_CS_Pin, GPIO_PIN_SET);   // ✅ HIGH
}
uint8_t spi_readbyte(void)
{
    uint8_t tx = 0xFF;
    uint8_t rx = 0;
    HAL_SPI_TransmitReceive(&hspi2, &tx, &rx, 1, 100);
    HAL_Delay(1);
    return rx;
}

void spi_writebyte(uint8_t wb)
{
    HAL_SPI_Transmit(&hspi2, &wb, 1, 100);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI2_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */

  printf("UART3 Working\r\n");

  /* Ensure CS idle HIGH */
  HAL_GPIO_WritePin(ETH_CS_GPIO_Port, ETH_CS_Pin, GPIO_PIN_SET);

  /* Register SPI callbacks */
  reg_wizchip_cs_cbfunc(cs_select, cs_deselect);
  reg_wizchip_spi_cbfunc(spi_readbyte, spi_writebyte);

  /* Reset W5500 */
  HAL_GPIO_WritePin(ETH_RST_GPIO_Port, ETH_RST_Pin, GPIO_PIN_RESET);
  HAL_Delay(100);
  HAL_GPIO_WritePin(ETH_RST_GPIO_Port, ETH_RST_Pin, GPIO_PIN_SET);
  HAL_Delay(200);

  /* Initialize W5500 internal memory */
  uint8_t txsize[8] = {2,2,2,2,2,2,2,2};
  uint8_t rxsize[8] = {2,2,2,2,2,2,2,2};
  wizchip_init(txsize, rxsize);

  /* Check chip version */
  for(int i=0;i<5;i++)
  {
      uint8_t ver = getVERSIONR();
      printf("Try %d: W5500 Version = 0x%02X\r\n", i, ver);
      HAL_Delay(100);
  }
  /* Network configuration */
  wiz_NetInfo netinfo = {
      .mac  = {0x00,0x08,0xDC,0x11,0x22,0x33},

      .ip   = {192,168,1,112},   // ✔ OK (same network)
      .sn   = {255,255,255,0},

      .gw   = {192,168,1,1},     // ✔ FIX THIS
      .dns  = {8,8,8,8},

      .dhcp = NETINFO_STATIC
  };

  /* Apply network config */
  wizchip_setnetinfo(&netinfo);
  printf("Network Configured\r\n");

  /* Read back network info */
  wiz_NetInfo checkinfo;
  wizchip_getnetinfo(&checkinfo);

  printf("IP: %d.%d.%d.%d\r\n",
         checkinfo.ip[0],
         checkinfo.ip[1],
         checkinfo.ip[2],
         checkinfo.ip[3]);

  printf("GW: %d.%d.%d.%d\r\n",
         checkinfo.gw[0],
         checkinfo.gw[1],
         checkinfo.gw[2],
         checkinfo.gw[3]);

  /* Check Ethernet cable */
  uint8_t phy;
  ctlwizchip(CW_GET_PHYLINK, (void*)&phy);

  if (phy == PHY_LINK_ON)
      printf("Ethernet Cable Connected\r\n");
  else
      printf("Ethernet Cable NOT Connected\r\n");
  uint8_t socket_num = 0;
  uint8_t buffer[512];
  socket(socket_num, Sn_MR_TCP, 80, 0);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  switch(getSn_SR(socket_num))
	      {
	          case SOCK_INIT:
	              listen(socket_num);
	              break;

	          case SOCK_LISTEN:
	              break;

	          case SOCK_ESTABLISHED:


	          if(getSn_RX_RSR(socket_num) > 0)
	          {
	              memset(buffer,0,sizeof(buffer));

	              int len = recv(socket_num, buffer, sizeof(buffer)-1);

	              if(len > 0 && len < sizeof(buffer))
	                  buffer[len] = '\0';

	              printf("%s\r\n",buffer);

	              if (strstr((char*)buffer, "GET /data"))
	              {
	                  /* === REPLACE these with your real sensor reads === */
	            	  uint32_t t = HAL_GetTick();

	            	  float    temperature = 25.0f + (float)(t % 100) * 0.1f;
	            	  float    vibration   = 0.10f + (float)(t % 50)  * 0.01f;
	            	  uint16_t rpm         = 1400  + (t % 200);
	            	  char*    status      = (t % 9000 < 3000) ? "FAULT" :
	            	                         (t % 9000 < 6000) ? "WARN"  : "NORMAL";
	                  char json[128];
	                  snprintf(json, sizeof(json),
	                      "{\"temp\":%.1f,\"vib\":%.2f,\"rpm\":%u,\"status\":\"%s\"}",
	                      temperature, vibration, rpm, status);

	                  char hdr[128];
	                  snprintf(hdr, sizeof(hdr),
	                      "HTTP/1.1 200 OK\r\n"
	                      "Content-Type: application/json\r\n"
	                      "Access-Control-Allow-Origin: *\r\n"
	                      "Connection: close\r\n"
	                      "Content-Length: %d\r\n"
	                      "\r\n",
	                      (int)strlen(json));

	                  send(socket_num, (uint8_t*)hdr,  strlen(hdr));
	                  send(socket_num, (uint8_t*)json, strlen(json));
	                  disconnect(socket_num);
	                  break;   /* ← don't fall through to HTML */
	              }
	              if(strstr((char*)buffer,"PB3ON"))
	              HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_SET);


	              if(strstr((char*)buffer,"PB3OFF"))
	              HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_RESET);


	              if(strstr((char*)buffer,"PB4ON"))
	              HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET);


	              if(strstr((char*)buffer,"PB4OFF"))
	              HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET);


	              char http_header[] =
	              "HTTP/1.1 200 OK\r\n"
	              "Content-Type: text/html\r\n"
	              "Connection: close\r\n"
	              "\r\n";

	              /* Send header */
	              send(socket_num, (uint8_t*)http_header, strlen(http_header));

	              /* Send webpage */
	              uint16_t sent = 0;

	              while(sent < index_html_len)
	              {
	                  uint16_t chunk = (index_html_len - sent > 1024) ? 1024 : (index_html_len - sent);

	                  send(socket_num, (uint8_t*)&index_html[sent], chunk);

	                  sent += chunk;
	              }
	              disconnect(socket_num);
	          }


	          	  break;

	          case SOCK_CLOSE_WAIT:
	              disconnect(socket_num);
	              break;

	          case SOCK_CLOSED:
	              socket(socket_num, Sn_MR_TCP, 80, 0);
	              break;
	      }

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 176;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

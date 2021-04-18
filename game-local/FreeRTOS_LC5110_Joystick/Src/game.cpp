/* Includes ------------------------------------------------------------------*/
#include <game.hpp>
#include <PRNG_LFSR.hpp>
#include "main.h"
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */
//#include "atraso.h"
#include "atrasoC.h"
#include "defPrincipais.h"
//#include "NOKIA5110_fb.h"
//#include "figuras.h"

#include "game_config.h"

//#include <app.hpp>
#include <nokia.hpp>
#include <assert.h> //to disable, #define NDEBUG


/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

//osThreadId defaultTaskHandle;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
uint32_t ADC_buffer[2];
uint32_t valor_ADC[2];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
//void StartDefaultTask(void const * argument);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int max (int* arr, int len){
    int maxval = -999; //TODO: proper -inf
    for (int i=0; i<len; i++){
        if (arr[i] > maxval){
            maxval = arr[i];
        }
    }
    return maxval;
}
int argmax (int* arr, int len){
    int maxidx = 0;
    int maxval = -999; //TODO: proper -inf
    for (int i=0; i<len; i++){
        if (arr[i] > maxval){
            maxidx = i;
            maxval = arr[i];
        }
    }
    return maxidx;
}


class EnvBlobObs{
    public:
    int dfx;
    int dfy;
    int dex;
    int dey;
};

class Step {
    public:
    EnvBlobObs new_obs;
    int reward;
    int done;
    int info;
};

class Blob{
    public:
    int x;
    int y;

    void set_pos(int x0, int y0){
        x=x0;
        y=y0;
    }

    void move(int dx, int dy){
        x += dx;
        y += dy;

        // Wrap border
        if (x >= GRID_SIZE)
        {
        	 x = 0;
        }
        if (x<0)
        {
            x = GRID_SIZE - 1;
        }
        if (y >= GRID_SIZE)
        {
        	y = 0;
        }
        if (y<0)
        {
            y = GRID_SIZE - 1;
        }
    }

};

class EnvBlob{
    public:
    int _step_counter;
    int _done;

    Blob _player;
    Blob _food;
    Blob _enemy;

    EnvBlob(){
        // TODO: color
        _player = Blob();
        _food = Blob();
        _enemy = Blob();
    }

    void reset(void){
        // Generate unique tuples
        int inits[6] = {0,0,0,0,0,0};
        inits[0] = prng_LFSR()%GRID_SIZE;
        inits[1] = prng_LFSR()%GRID_SIZE;
        while (1){
            inits[2] = prng_LFSR()%GRID_SIZE;
            inits[3] = prng_LFSR()%GRID_SIZE;
            if (inits[2]!=inits[0] || inits[3]!=inits[1])
                break;
	    }
        while (1){
            inits[4] = prng_LFSR()%GRID_SIZE;
            inits[5] = prng_LFSR()%GRID_SIZE;
            if ((inits[4]!=inits[0] || inits[5]!=inits[1]) && (inits[4]!=inits[2] || inits[5]!=inits[3]))
                break;
	    }

        // Reset the agents
        _player.set_pos(inits[0], inits[1]);
        _food.set_pos(inits[2], inits[3]);
        _enemy.set_pos(inits[4], inits[5]);
        _step_counter = 0;
        _done = 0;

 	    // Impossible game check
       assert(_player.x != _enemy.x or _player.y != _enemy.y);
       assert(_player.x != _food.x or _player.y != _food.y);
       assert(_enemy.x != _food.x or _enemy.y != _food.y);
    }

    EnvBlobObs observation(void){
        EnvBlobObs o =  EnvBlobObs();
        o.dfx = _preprocess_obs(_player.x - _food.x);
        o.dfy = _preprocess_obs(_player.y - _food.y);
        o.dex = _preprocess_obs(_player.x - _enemy.x);
        o.dey = _preprocess_obs(_player.y - _enemy.y);
        return o;
    }

    // @Brief: convert the env internal representation to what is actually shown to the agent
    int _preprocess_obs(int obs){
        int max_obs = OBS_SIZE -1;
        if (obs>max_obs)
            obs = max_obs;   // Saturation
        if (obs<-max_obs)
            obs = -max_obs;

        obs = obs+max_obs; // Make positive
        return obs;
    }

    void step_enemy(int action)
    {
    	 // Game mechanics
		if (action == 0)
			_enemy.move(1, 0);
		else if (action == 1)
			_enemy.move(-1, 0);
		else if (action == 2)
			_enemy.move(0, 1);
		else if (action == 3)
			_enemy.move(0, -1);
		else{}
		//throw std::logic_error("Invalid action");
}

    Step step(int action){
        Step s = Step(); //TODO: quando esse memória é desalocada?

        // Game mechanics
        if (action == 0)
            _player.move(1, 0);
        else if (action == 1)
            _player.move(-1, 0);
        else if (action == 2)
            _player.move(0, 1);
        else if (action == 3)
            _player.move(0, -1);
        else{}
            //throw std::logic_error("Invalid action");

        // Reward function
        int dfx = _player.x - _food.x;
        int dfy = _player.y - _food.y;
        int dex = _player.x - _enemy.x;
        int dey = _player.y - _enemy.y;
        if (dex==0 && dey==0) // exactly on the enemy
            s.reward = ENEMY_PENALTY;
        else if (dfx==0 && dfy==0) // exactly on the food
            s.reward = FOOD_REWARD;
        else if ((dfx*dfx + dfy*dfy)<3) // Near food //TODO: overflow?
            s.reward = 0;
        else
            s.reward = -1;

        EnvBlobObs o = EnvBlobObs(); //TODO: quando esse memória é desalocada?
        o.dfx = 0;
        o.dfy = 0;
        o.dex = 0;
        o.dey = 0;


        s.new_obs = o;
        s.info = 0;

        _step_counter += 1;
        if (_step_counter>=199 || s.reward==FOOD_REWARD  || s.reward==ENEMY_PENALTY)
            s.done=1;
        else
            s.done=0;
        return s;
    }

    void render(void){
        //std::cout << "------------\n";
        //std::cout << _player.x << ' ' << _player.y << '\n';
        //std::cout << _food.x << ' ' << _food.y << '\n';
        //std::cout << _enemy.x << ' ' << _enemy.y << '\n';

    	limpa_LCD();

    	struct pontos_t scenery;

    	// limits scenery
		scenery.x1 = 0;
		scenery.y1 = 0;
		scenery.x2 = 83;
		scenery.y2 = 47;

		desenha_retangulo(&scenery,1);

    	print_AI(_player.x*8,_player.y*8);
    	print_food(_food.x*8,_food.y*8);
    	print_monster(_enemy.x*8,_enemy.y*8);

    }
};

class IntelligentAgent{
    int _epsilon; //percentual, de 0 a 100
    int _q_table[OBS_RANGE][OBS_RANGE][OBS_RANGE][OBS_RANGE][ACTION_SPACE];
    public:
        IntelligentAgent(){
            _epsilon = INITIAL_EPSILON;

            #ifdef LOAD_Q_TABLE
                //TODO: load q_table from .h file
            #else
                for (int a=0; a<OBS_RANGE; a++){
                    for (int b=0; b<OBS_RANGE; b++){
                        for (int c=0; c<OBS_RANGE; c++){
                            for (int d=0; d<OBS_RANGE; d++){
                                for (int e=0; e<ACTION_SPACE; e++){
                                    _q_table[a][b][c][d][e] =  prng_LFSR()%6 - 5;
                                }
                            }
                        }
                    }
                }
            #endif

            //std::cout << "[AI] Init done\n";
        }

        int action(EnvBlobObs* obs){
            int action;
            if (prng_LFSR()%100 > _epsilon)
                action = argmax(&_q_table[obs->dfx][obs->dfy][obs->dex][obs->dey][0], ACTION_SPACE);
            else
                action = prng_LFSR()%ACTION_SPACE;

            return action;
        }
        void feedback(EnvBlobObs* obs, EnvBlobObs* new_obs, int action, int reward){
            float max_future_q = (float) max(&_q_table[new_obs->dfx][new_obs->dfy][new_obs->dex][new_obs->dey][0], ACTION_SPACE);
            float current_q = (float) _q_table[obs->dfx][obs->dfy][obs->dex][obs->dey][action];
            int new_q = (int) ((1 - LEARNING_RATE)*current_q + LEARNING_RATE*((float)reward+DISCOUNT*max_future_q));
            _q_table[obs->dfx][obs->dfy][obs->dex][obs->dey][action] = new_q;



            //std::cout << "[AI] Great feedback, thanks\n";
        }
        int to_disk(void){
            return 0; //TODO
        }
        int from_disk(void){
            return 0; //TODO
        }
        void episode_callback(void){
            _epsilon *= EPS_DECAY;
        }
};

void game_init();
//init_LFSR(666);
EnvBlob env = EnvBlob();
IntelligentAgent agent1 = IntelligentAgent();
int episode_rewards = 0;
int done = 0;

int app(){
      //  int episode_reward = 0;

        // Play the game within episode
        //while (!done){
        EnvBlobObs obs = env.observation();

            // Collect actions
            int action = agent1.action(&obs);
			//int action = 0;
			//action = prng_LFSR()%ACTION_SPACE;
            //action from agent2?
            //action fom player?

            // Avance the game
            Step step = env.step(action);
            agent1.feedback(&obs, &step.new_obs, action, step.reward);
            episode_rewards += step.reward;
            //done=step.done;
            if(step.done==1)
            {
            	game_init();
            }
            //if (episode%SHOW_EVERY==0 && episode!=0){
                env.render();
            //}
       // }
}

void move_enemy(int action)
{

}

void game_init()
{
	env.render();
	HAL_Delay(500);
	limpa_LCD();
	HAL_Delay(100);
	env.render();
	HAL_Delay(500);
	limpa_LCD();
	HAL_Delay(100);
	env.render();
	HAL_Delay(500);
	limpa_LCD();
	HAL_Delay(100);
	env.render();
	HAL_Delay(500);
	limpa_LCD();
	HAL_Delay(100);
	env.reset();
	agent1.episode_callback();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	if(hadc->Instance == ADC1)
	{
		valor_ADC[0]=ADC_buffer[0];
		valor_ADC[1]=ADC_buffer[1];
	}
}

//---------------------------------------------------------------------------------------------------
// Tarefa para atualizar periodicamente o LCD
void vTask_LCD_Print(void *pvParameters)
{
	while(1) imprime_LCD();
}
//---------------------------------------------------------------------------------------------------
// Tarefa para imprimir um numero aleatorio
void vTask_Nr_Print(void *pvParameters)
{
	//uint32_t rand_prng;

	while(1)
	{
		app();

		// joystick conditions
		if(valor_ADC[0] > 3000)
		{
			env.step_enemy(3);
		}
		else if(valor_ADC[0] < 1000)
		{
			env.step_enemy(2);
		}

		if(valor_ADC[1] > 3000)
		{
			env.step_enemy(0);
		}
		else if(valor_ADC[1] < 1000)
		{
			env.step_enemy(1);
		}
		escreve_Nr_Peq(65,2,episode_rewards,0);
		vTaskDelay(250);
	}
}
//---------------------------------------------------------------------------------------------------
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	//uint32_t semente_PRNG=1;
	unsigned long semente_PRNG=1;

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

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
	MX_DMA_Init();
	MX_ADC1_Init();

	/* USER CODE BEGIN 2 */

	HAL_ADC_Start_DMA(&hadc1,(uint32_t*)ADC_buffer,2);
	HAL_ADC_Start_IT(&hadc1);

	// inicializa LCD 5110
	inic_LCD();
	limpa_LCD();

	game_init();

	// --------------------------------------------------------------------------------------
	// inicializa tela

	goto_XY(0, 0);
	string_LCD("Press.  Botao");
	imprime_LCD();

	//while(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15)) // enquando nao pressionar joystick fica travado
	//{
	//	semente_PRNG++;		// semente para o gerador de n�meros pseudoaleatorios
							// pode ser empregado o ADC lendo uma entrada flutuante para gerar a semente.
	//}

	init_LFSR(666);	// inicializacao para geracao de numeros pseudoaleatorios
	//rand_prng = prng_LFSR();	// sempre q a funcao prng() for chamada um novo nr � gerado.

	limpa_LCD();
	//escreve2fb((unsigned char *)dragon);
	imprime_LCD();
	HAL_Delay(1000);
	limpa_LCD();
	imprime_LCD();

	/* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

	/* Create the thread(s) */
	/* definition and creation of defaultTask */
	// osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
	// defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

	/* USER CODE BEGIN RTOS_THREADS */
	xTaskCreate(vTask_LCD_Print, "Task 1", 100, NULL, 1,NULL);
	xTaskCreate(vTask_Nr_Print, "Task 2", 100, NULL, 1,NULL);
	/* USER CODE END RTOS_THREADS */

	/* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
	/* USER CODE END RTOS_QUEUES */


	/* Start scheduler */
	// osKernelStart();
	vTaskStartScheduler();	// apos este comando o RTOS passa a executar as tarefas



  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

  /* USER CODE END WHILE */
  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

/* ADC1 init function */
static void MX_ADC1_Init(void)
{

  ADC_ChannelConfTypeDef sConfig;

    /**Common config
    */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 2;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure Regular Channel
    */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure Regular Channel
    */
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

}

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6
                          |GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PC15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA3 PA4 PA5 PA6
                           PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6
                          |GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* StartDefaultTask function */
//void StartDefaultTask(void const * argument)
//{

  /* USER CODE BEGIN 5 */
  /* Infinite loop */
//  for(;;)
//  {
//    osDelay(1);
//  }
  /* USER CODE END 5 */
//}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM4 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM4) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

 // my functions

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

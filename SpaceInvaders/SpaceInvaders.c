// SpaceInvaders.c
// Runs on TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the EE319K Lab 10

// Last Modified: 8/24/2022 
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php

// ******* Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PD2/AIN5
// Slide pot pin 3 connected to +3.3V 
// buttons connected to PE0-PE3
// 32*R resistor DAC bit 0 on PB0 (least significant bit)
// 16*R resistor DAC bit 1 on PB1
// 8*R resistor DAC bit 2 on PB2 
// 4*R resistor DAC bit 3 on PB3
// 2*R resistor DAC bit 4 on PB4
// 1*R resistor DAC bit 5 on PB5 (most significant bit)
// LED on PD1
// LED on PD0


#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "../inc/ST7735.h"
#include "Random.h"
#include "TExaS.h"
#include "../inc/ADC.h"
#include "Images.h"
#include "Sound.h"
#include "Timer1.h"

//enemy
//x,y location 
//image 
// life 
typedef enum{dead, alive, dying} life_t;    //status_t = life_t
struct enemy{        // enemy = sprite 
	int16_t y;
	int16_t x; 
	uint32_t points;
	int32_t vx,vy;
	const unsigned short *image;
	const unsigned short *bimage;
	life_t life;
	int32_t w;
	int32_t h;
	uint32_t needDraw;
};

typedef struct enemy enemy_t; 
#define NUME 4 
enemy_t Enemies[NUME];
void Enemy_Init(void){
	Enemies[0].x =5 ;
	Enemies[0].y =10; 
	Enemies[0].points = 10;
	Enemies[0].image = SmallEnemy10pointA;
  Enemies[0].bimage = EnemyBlack;
	Enemies[0].life = alive;
	
	Enemies[1].x = 32 ;
	Enemies[1].y =10; 
	Enemies[1].points = 10;
	Enemies[1].image = SmallEnemy20pointA;
	Enemies[1].bimage = EnemyBlack;
	Enemies[1].life = alive;
	
	Enemies[2].x = 80 ;
	Enemies[2].y = 10; 
	Enemies[2].points = 30;
	Enemies[2].image = SmallEnemy30pointA;
	Enemies[2].bimage = EnemyBlack;
	Enemies[2].life = dead;
     for(int i=3; i<NUME; i++){
			 Enemies[i].life =dead;
	 }
 }	 

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Delay100ms(uint32_t count); // time delay in 0.1 seconds
void Draw(void){
	for(int i = 0; i<NUME; i++){
		if(Enemies[i].life == alive){
			Enemies[i].y++;
		}
	}
 } 

 
uint32_t Count = 0;
#define RATE 10
void Move(void){
	if((Count%RATE)==0){
	for(int i = 0; i<NUME; i++){
		if(Enemies[i].life == alive){
			Enemies[i].y++;
	  	}
   	 }		
    }
   }	

void Port_Init(void){
	SYSCTL_RCGCGPIO_R |= 0x10;
	while((SYSCTL_PRGPIO_R & 0x10) != 0x10)
	GPIO_PORTB_DIR_R |= 0x07;
	GPIO_PORTB_DEN_R |= 0x07;
	GPIO_PORTA_DIR_R &= ~0x10;
	GPIO_PORTA_DEN_R |= 0x10;
}

int* Bullets;
uint32_t LastE;
void ProcessInput(void){
	uint32_t Now;
	Now = GPIO_PORTE_DATA_R &0x08; //PE3 
	if((Now == 8) && (LastE == 0)){
		Bullets[0].life = alive;
		Bullets[0].vy = -1;
		Bullets[0].x = position;
		Bullets[0].y = 150;
 }
	LastE = Now;
}
 // 30Hz > 2*fmax 
 // low priority 

int Flag;
void Timer1A_Handler(void){ // can be used to perform tasks in background
  TIMER1_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER1A timeout
                      // execute user task
	                    // sample ADC for slide pot, to move player 
	                    // sample buttons 
	Count++;
	Move();             // move sprites 
	                    // game, collisions, start sounds, points
	                    // signal to main thread that changes 
	Flag = 1;           //signal semaphore
}





int main1(void){
  DisableInterrupts();
  TExaS_Init(NONE);       // Bus clock is 80 MHz 
  Random_Init(1);

  Output_Init();
  ST7735_FillScreen(0x0000);            // set screen to black
  
  ST7735_DrawBitmap(22, 159, PlayerShip0, 18,8); // player ship bottom
  ST7735_DrawBitmap(53, 151, Bunker0, 18,5);
  ST7735_DrawBitmap(42, 159, PlayerShip1, 18,8); // player ship bottom
  ST7735_DrawBitmap(62, 159, PlayerShip2, 18,8); // player ship bottom
  ST7735_DrawBitmap(82, 159, PlayerShip3, 18,8); // player ship bottom

  ST7735_DrawBitmap(0, 9, SmallEnemy10pointA, 16,10);
  ST7735_DrawBitmap(20,9, SmallEnemy10pointB, 16,10);
  ST7735_DrawBitmap(40, 9, SmallEnemy20pointA, 16,10);
  ST7735_DrawBitmap(60, 9, SmallEnemy20pointB, 16,10);
  ST7735_DrawBitmap(80, 9, SmallEnemy30pointA, 16,10);
  ST7735_DrawBitmap(100, 9, SmallEnemy30pointB, 16,10);

  Delay100ms(50);              // delay 5 sec at 80 MHz

  ST7735_FillScreen(0x0000);   // set screen to black
  ST7735_SetCursor(1, 1);
  ST7735_OutString("GAME OVER");
  ST7735_SetCursor(1, 2);
  ST7735_OutString("Nice try,");
  ST7735_SetCursor(1, 3);
  ST7735_OutString("Earthling!");
  ST7735_SetCursor(2, 4);
  ST7735_OutUDec(1234);
  while(1){
  }

}


// You can't use this timer, it is here for starter code only 
// you must use interrupts to perform delays
void Delay100ms(uint32_t count){uint32_t volatile time;
  while(count>0){
    time = 727240;  // 0.1sec at 80 MHz
    while(time){
      time--;
    }
    count--;
  }
}
typedef enum {English, Spanish, Portuguese, French} Language_t;
Language_t myLanguage=English;
typedef enum {HELLO, GOODBYE, LANGUAGE} phrase_t;
const char Hello_English[] ="Hello";
const char Hello_Spanish[] ="\xADHola!";
const char Hello_Portuguese[] = "Ol\xA0";
const char Hello_French[] ="All\x83";
const char Goodbye_English[]="Goodbye";
const char Goodbye_Spanish[]="Adi\xA2s";
const char Goodbye_Portuguese[] = "Tchau";
const char Goodbye_French[] = "Au revoir";
const char Language_English[]="English";
const char Language_Spanish[]="Espa\xA4ol";
const char Language_Portuguese[]="Portugu\x88s";
const char Language_French[]="Fran\x87" "ais";
const char *Phrases[3][4]={
  {Hello_English,Hello_Spanish,Hello_Portuguese,Hello_French},
  {Goodbye_English,Goodbye_Spanish,Goodbye_Portuguese,Goodbye_French},
  {Language_English,Language_Spanish,Language_Portuguese,Language_French}
};

int main(void){ char l;
  DisableInterrupts();
  TExaS_Init(NONE);       // Bus clock is 80 MHz 
  Output_Init();
  ST7735_FillScreen(0x0000);            // set screen to black
  for(phrase_t myPhrase=HELLO; myPhrase<= GOODBYE; myPhrase++){
    for(Language_t myL=English; myL<= French; myL++){
         ST7735_OutString((char *)Phrases[LANGUAGE][myL]);
      ST7735_OutChar(' ');
         ST7735_OutString((char *)Phrases[myPhrase][myL]);
      ST7735_OutChar(13);
    }
  }
  Delay100ms(30);
  ST7735_FillScreen(0x0000);       // set screen to black
	
	
	
	
	
	
	
	
	
	// Initialize everything 
 Timer1_Init(80000000/30,5);
 Flag = 0; 
 Enemy_Init();
 Draw();
 EnableInterrupts();
 while(1){
	 while(Flag == 0){};
		 // draw the screen LCD 
		 Flag = 1; 
	 }
	



	 
	
	
	
  l = 128;
  while(1){
    Delay100ms(20);
    for(int j=0; j < 3; j++){
      for(int i=0;i<16;i++){
        ST7735_SetCursor(7*j+0,i);
        ST7735_OutUDec(l);
        ST7735_OutChar(' ');
        ST7735_OutChar(' ');
        ST7735_SetCursor(7*j+4,i);
        ST7735_OutChar(l);
        l++;
      }
    }
  }  
}







// 10.8 from the book 

uint32_t TimerCount;
void Timer2_Init(uint32_t period){
  uint32_t volatile delay;
  SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate timer2
  delay = SYSCTL_RCGCTIMER_R;
  TimerCount = 0;
  TIMER2_CTL_R = 0x00000000;   // 1) disable timer2A
  TIMER2_CFG_R = 0x00000000;   // 2) 32-bit mode
  TIMER2_TAMR_R = 0x00000002;  // 3) periodic mode
  TIMER2_TAILR_R = period-1;   // 4) reload value
  TIMER2_TAPR_R = 0;           // 5) clock resolution
  TIMER2_ICR_R = 0x00000001;   // 6) clear timeout flag
  TIMER2_IMR_R = 0x00000001;   // 7) arm timeout
  NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|0x80000000;
// 8) priority 4
  NVIC_EN0_R = 1<<23;          // 9) enable IRQ 23 in
  TIMER2_CTL_R = 0x00000001;   // 10) enable timer2A
}

// trigger is Timer2A Time-Out Interrupt

// set periodically TATORIS set on rollover

void Timer2A_Handler(void){
  TIMER2_ICR_R = 0x00000001;  // acknowledge
  TimerCount++;
// run some background stuff here
}

void Timer2A_Stop(void){
  TIMER2_CTL_R &= ~0x00000001; // disable
}

void Timer2A_Start(void){
  TIMER2_CTL_R |= 0x00000001;   // enable

}









// 10.7 from the book 

volatile uint32_t FallingEdges = 0;
void EdgeCounter_Init(void){       
  SYSCTL_RCGCGPIO_R |= 0x00000020; // (a) activate clock for port F
  FallingEdges = 0;             // (b) initialize count and wait for clock
  GPIO_PORTF_DIR_R &= ~0x10;    // (c) make PF4 in (built-in button)
  GPIO_PORTF_DEN_R |= 0x10;     //     enable digital I/O on PF4
  GPIO_PORTF_PUR_R |= 0x10;     //     enable weak pull-up on PF4
  GPIO_PORTF_IS_R &= ~0x10;     // (d) PF4 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x10;    //     PF4 is not both edges
  GPIO_PORTF_IEV_R &= ~0x10;    //     PF4 falling edge event
  GPIO_PORTF_ICR_R = 0x10;      // (e) clear flag4
  GPIO_PORTF_IM_R |= 0x10;      // (f) arm interrupt on PF4
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5
  NVIC_EN0_R = 0x40000000;      // (h) enable interrupt 30 in NVIC
  EnableInterrupts();          // (i) Enable global Interrupt flag (I)
}

void GPIOPortF_Handler(void){
  GPIO_PORTF_ICR_R = 0x10;      // acknowledge flag4
  FallingEdges = FallingEdges + 1;
}
int main(void){ 
   EdgeCounter_Init(); // initialize GPIO Port F interrupt 
   while(1){ 
      WaitForInterrupt();
   }






























/*
 * main.c
 *
 * Created: 12/6/2023 2:27:19 PM
 *  Author: Leonardo LASID
 */ 
#define F_CPU 16000000UL

#include <avr/io.h>
#include <xc.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "kiss_fft130/kiss_fft.h"
#include "kiss_fft130/_kiss_fft_guts.h"
#include "AVR-SSD1306-master/Files/SSD1306.h"
#include "AVR-SSD1306-master/Files/Font5x8.h"


uint16_t cont = 0;


// Constantes Sensor
#define proximitySensorPin 0 // A0
#define proximitySensorPin1 1 // A1
#define proximitySensorPin2 2 // A2
#define proximitySensorPin3 3 // A3
#define voltageRange 5.0
#define distanceRange 8.0
double media;


// Constantes FFT
#define SAMPLES 128
#define SAMPLING_FREQUENCY 1000


float entrada[SAMPLES];

// Variáveis globais
double VelocidadeAtual;
double frequencySM;



// Inicialização da variável PID
//double Input, Output, Setpoint;

// Estrutura para dados complexos (parte real e imaginária)
typedef struct {
	float r;
	float i;
} complex_t;

// Configuração da FFT
kiss_fft_cfg cfg;


// Protótipos de Funções
void setup();
double FFT_SM(double entrada[SAMPLES]);
void loop();
void analogWrite25k(int value);

struct SensorData {
	double distance0;
	double distance1;
	double distance2;
	double distance3;
	double media;
};
struct SensorData readSensor();

void setup() {
	// Configuração dos pinos
	DDRB &= ~(1 << proximitySensorPin) & (1 << proximitySensorPin1) & (1 << proximitySensorPin2) & (1 << proximitySensorPin3); // Configura os pinos A0, A1, A2, A3 como saída para leitura dos sensores

	// Configuração Timer1 para PWM
	/*TCCR1A = (1 << WGM11) | (1 << COM1B1); // Modo de PWM fase corrigida, não invertido em OC1B
	TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS10); // Prescaler 1
	ICR1 = 320; // TOP = 320

	// Inicialização de variáveis
	//int Input = 0;
	//int Output = 0;
	//int Setpoint = 0;
	VelocidadeDesejada = 100; // Substitua por um valor padrão ou leia de alguma entrada
	currentPWM = 0;
	*/

	// Inicialização da configuração da FFT
	
	cfg = kiss_fft_alloc(SAMPLES, 0, NULL, NULL);
	

}

void Display(){
	
		GLCD_Setup();
		GLCD_SetFont(Font5x8,5,8,GLCD_Overwrite);
		GLCD_InvertScreen();
		GLCD_Clear();
		GLCD_GotoXY(5,30);
		GLCD_PrintDouble(media,3);
		GLCD_PrintString(" /");
		GLCD_PrintDouble(entrada[1],4);
		GLCD_Render();
	
}

void loop() {
	struct SensorData sensorData = readSensor();
	
	media= sensorData.media;	
	for (int i = 0; i < SAMPLES; ++i) {
		struct SensorData sensorData = readSensor();
		media= sensorData.media;
		entrada[i] = media;
	 }
	// Preencher fin com dados reais (substitua pelos dados do sensor)
	
	// FFT
	//FFT_SM(entrada[SAMPLES]);
	
	

	// Calcular velocidade atual
	//VelocidadeAtual = frequencySM * 60.0;

	// Imprimir a velocidade atual
	Display();
	

	// Se necessário, adicione aqui a lógica para controlar o ventilador com PID
}

/*
void analogWrite25k(int value) {
	OCR1B = value;
}*/

struct SensorData readSensor() {
	struct SensorData sensorData ;


		// Leitura de A0
		ADMUX = (1 << REFS0) | (proximitySensorPin & 0x0F);
		ADCSRA |= (1 << ADEN) | (1 << ADSC);
		while (ADCSRA & (1 << ADSC))
		;
		uint16_t sensorValue0 = (ADCL | (ADCH << 8));
		sensorData.distance0 = (sensorValue0 / 1023.0) * distanceRange;

		// Leitura de A1
		ADMUX = (1 << REFS0) | (proximitySensorPin1 & 0x0F);
		ADCSRA |= (1 << ADEN) | (1 << ADSC);
		while (ADCSRA & (1 << ADSC))
		;
		uint16_t sensorValue1 = (ADCL | (ADCH << 8));
		sensorData.distance1 = (sensorValue1 / 1023.0) * distanceRange;

		// Leitura de A2
		ADMUX = (1 << REFS0) | (proximitySensorPin2 & 0x0F);
		ADCSRA |= (1 << ADEN) | (1 << ADSC);
		while (ADCSRA & (1 << ADSC))
		;
		uint16_t sensorValue2 = (ADCL | (ADCH << 8));
		sensorData.distance2 = (sensorValue2 / 1023.0) * distanceRange;

		// Leitura de A3
		ADMUX = (1 << REFS0) | (proximitySensorPin3 & 0x0F);
		ADCSRA |= (1 << ADEN) | (1 << ADSC);
		while (ADCSRA & (1 << ADSC))
		;
		uint16_t sensorValue3 = (ADCL | (ADCH << 8));
		sensorData.distance3 = (sensorValue3 / 1023.0) * distanceRange;
	
	for (int i = 0; i < 4; ++i) {
			sensorData.media= (sensorData.distance0 + sensorData.distance1 + sensorData.distance2 + sensorData.distance3)/4;
	};
return sensorData;
}



/*float FFT_SM(double entrada[SAMPLES]) {
	// Tamanho da FFT (deve ser uma potência de 2)
	int nfft = SAMPLES;

	// Matrizes de entrada e saída
	kiss_fft_cpx fin[nfft];
	kiss_fft_cpx fout[nfft];

	


		// Preencher fin com dados reais (substitua pelos dados do sensor)
	for (int i = 0; i < nfft; ++i) {
		fin[i].r = entrada[i];  // Exemplo: usar a média dos sensores como entrada
		fin[i].i = 0;    // Parte imaginária (pode ser 0 se seus dados são reais)
	}
	
	// Realizar a FFT
	kiss_fft(cfg, fin, fout);

	// Encontrar o pico na FFT
	double maxMagnitude = 0.001;
	int maxIndex = 1;

	for (int i = 1; i < nfft / 2; ++i) {
		float magnitude = sqrt((fout[i].r * fout[i].r) +  (fout[i].i * fout[i].i ));
		if (magnitude > maxMagnitude) {
			maxMagnitude = magnitude;
			maxIndex = i;
		}
	}

	// Calcular a frequência correspondente ao pico
	
	float frequencyMax = maxIndex * (SAMPLING_FREQUENCY/ nfft);
	
	return frequencyMax;
	
}*/

int main(void){
	setup();
	while (1) {
		loop();
		_delay_ms(1000);
	}
	return 0;
}
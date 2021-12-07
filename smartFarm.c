#include <wiringPi.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include "MQTTClient.h"

#define ADDRESS "test.mosquitto.org" // 브로커에 접속하기 위한 주소
#define CLIENTID "ClientPubE"

//다양한 핀 선언
#define MAX_TIME 100
#define DHTPIN 25

//dc motor pins
#define PIN_INA 26
#define PIN_INB 23

#define SOIL_ADC_CH 5
#define LIGHT_ADC_CH 0
#define DUST_ADC_CH 3
#define OUT 4
#define SPI_CH 0
#define ADC_CS 29
#define SPI_SPEED 500000

int dht11_data[5] = { 0, 0, 0, 0, 0 }; //온도&습도 값 저장을 위한 배열 선언

MQTTClient client; //MQTT 클라이언트 선언
MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer; //MQTT 연결 시작해주는 코드


void* read_soil(void* arg) { //soil moisture 센서 값을 읽는 코드
    printf("I'm soil\n");
    MQTTClient_message pubmsg_soil = MQTTClient_message_initializer; //MQTT로 보낼 메세지 선언

    int value = 0, i;
    unsigned char buf[3];

    pinMode(ADC_CS, OUTPUT);

    while (1) { //센서값을 읽고 보내는 무한루프
        buf[0] = 0x06 | ((SOIL_ADC_CH & 0x04) >> 2);
        buf[1] = ((SOIL_ADC_CH & 0x03) << 6);
        buf[2] = 0x00;

        digitalWrite(ADC_CS, 0);
        wiringPiSPIDataRW(SPI_CH, buf, 3);

        buf[1] = 0x0F & buf[1];

        value = (buf[1] << 8) | buf[2];

        digitalWrite(ADC_CS, 1);

        printf("Soil = %d\n", value);
        if(value != 0){
            char msg[10]; //메세지 길이 선언
            sprintf(msg, "%d", value); //메세지에 담을 값을 넣어줌
            pubmsg_soil.payload = msg;
            pubmsg_soil.payloadlen = (int)strlen(msg);
            MQTTClient_publishMessage(client, "soil_hk", &pubmsg_soil, NULL); //soil_hk라는 Topic으로 메세지를 pub함
            delay(10000); //값들을 계속 보내면 과부화가 올 수 있어 delay를 줌
        }
        else{
            delay(10000); //값들을 계속 보내면 과부화가 올 수 있어 delay를 줌
        }
    }
}

void* read_light(void* arg) { //light 센서 값을 읽는 코드
    printf("I'm light\n");
    MQTTClient_message pubmsg_light = MQTTClient_message_initializer; //MQTT로 보낼 메세지 선언

    int value = 0, i;
    unsigned char buf[3];

    pinMode(ADC_CS, OUTPUT);

    while (1) { //센서값을 읽고 보내는 무한루프
        buf[0] = 0x06 | ((LIGHT_ADC_CH & 0x04) >> 2);
        buf[1] = ((LIGHT_ADC_CH & 0x03) << 6);
        buf[2] = 0x00;

        digitalWrite(ADC_CS, 0);
        wiringPiSPIDataRW(SPI_CH, buf, 3);

        buf[1] = 0x0F & buf[1];

        value = (buf[1] << 8) | buf[2];

        digitalWrite(ADC_CS, 1);

        printf("Light = %d\n", value);
        if(value != 0){ // 값이 0이 아닐때에만 메시지 보냄
            char msg[10]; //메세지 길이 선언
            sprintf(msg, "%d", value/10); //메세지에 담을 값을 넣어줌 //msg는 str타입임
            pubmsg_light.payload = msg;
            pubmsg_light.payloadlen = (int)strlen(msg);
            MQTTClient_publishMessage(client, "light_hk", &pubmsg_light, NULL); //light_hk라는 Topic으로 메세지를 pub함
            delay(7000); //값들을 계속 보내면 과부화가 올 수 있어 delay를 줌
        }
        delay(7000); //값들을 계속 보내면 과부화가 올 수 있어 delay를 줌
    }
}

void* read_dust(void* arg) { //dust 센서 값을 읽는 코드
    printf("I'm dust\n");
    MQTTClient_message pubmsg_dust = MQTTClient_message_initializer; //MQTT로 보낼 메세지 선언

    int value = 0, i;
    int j = 0;
    unsigned char buf[3];

    pinMode(OUT, OUTPUT);

    digitalWrite(OUT, LOW);
    delayMicroseconds(280);

    while (1) { //센서값을 읽고 보내는 무한루프
        buf[0] = 0x06 | ((DUST_ADC_CH & 0x04) >> 2);
        buf[1] = ((DUST_ADC_CH & 0x03) << 6);
        buf[2] = 0x00;

        digitalWrite(ADC_CS, 0);
        wiringPiSPIDataRW(SPI_CH, buf, 3);

        buf[1] = 0x0F & buf[1];

        value = (buf[1] << 8) | buf[2];

        digitalWrite(ADC_CS, 1);

        delayMicroseconds(40);
        digitalWrite(OUT, HIGH);
        delayMicroseconds(9680);

        printf("Dust = %d\n", value);

        char msg[10]; //메세지 길이 선언
        sprintf(msg, "%d", value); //메세지에 담을 값을 넣어줌
        pubmsg_dust.payload = msg;
        pubmsg_dust.payloadlen = (int)strlen(msg);
        MQTTClient_publishMessage(client, "dust_hk", &pubmsg_dust, NULL); //dust_hk라는 Topic으로 메세지를 pub함
        delay(5000); //값들을 계속 보내면 과부화가 올 수 있어 delay를 줌
    }
}


void* read_dht11(void* arg) { //온도 & 습도 값을 읽는 코드
    while (1) { //센서값을 읽고 보내는 무한루프
        printf("I'm dht\n");
        MQTTClient_message pubmsg_temp = MQTTClient_message_initializer; //MQTT로 보낼 메세지 선언
        MQTTClient_message pubmsg_humi = MQTTClient_message_initializer; //MQTT로 보낼 메세지 선언

        //uint8_t laststate = HIGH;
        uint8_t laststate = 1;
        uint8_t counter = 0;
        uint8_t j = 0, i = 0;
        // add
        float f_temp;

        //dht11_data[0] = dht11_data[1] = dht11_data[2] = dht11_data[3] = dht11_data[4] = 0;
        int dht11_data[5] = {0, 0, 0, 0, 0};

        //DC motor 핀 모드 선언
        pinMode(PIN_INA, OUTPUT);
        pinMode(PIN_INB, OUTPUT);

        // start read value
        printf("dht start!\n");
        pinMode(DHTPIN, OUTPUT);
        digitalWrite(DHTPIN, LOW);
        delay(18);
        digitalWrite(DHTPIN, HIGH);
        delayMicroseconds(40);
        pinMode(DHTPIN, INPUT);

        laststate = HIGH;
        j = 0;
        for (i = 0; i < MAX_TIME; i++)
        {
            counter = 0;
            while (digitalRead(DHTPIN) == laststate)
            {
                counter++;
                delayMicroseconds(1);
                if (counter == 255)
                {
                    break;
                }
            }
            laststate = digitalRead(DHTPIN);

            if (counter == 255)
                break;

            if ((i >= 4) && (i % 2 == 0)) {
                dht11_data[j / 8] <<= 1;
                if (counter > 16)
                    dht11_data[j / 8] |= 1;
                j++;
            }
        }
        f_temp = dht11_data[2]*9./5.+32;

        // if (dht11_data[2] >= 28) { //온도가 28도 이상인 경우
        //     digitalWrite(PIN_INA, HIGH); //DC motor on
        //     printf("Fan on\n");

        // }
        // else if (dht11_data[2] < 28) { //온도가 28도 미만인 경우
        //     digitalWrite(PIN_INA, LOW); //DC motor off
        //     printf("Fan off\n");
        // }

        if ((j >= 40) && (dht11_data[4] == ((dht11_data[0] + dht11_data[1] + dht11_data[2] + dht11_data[3]) & 0xFF))) {
            printf("Temperature = %d.%d C\n", dht11_data[2], dht11_data[3]);
            char msg_temp[40]; //메세지 길이 선언
            sprintf(msg_temp, "%d.%d", dht11_data[2], dht11_data[3]); //메세지에 담을 값을 넣어줌
            pubmsg_temp.payload = msg_temp; 
            pubmsg_temp.payloadlen = (int)strlen(msg_temp);
            MQTTClient_publishMessage(client, "temp_hk", &pubmsg_temp, NULL); //temp_hk라는 Topic으로 메세지를 pub함

            printf("Humidity = %d.%d %%\n", dht11_data[0], dht11_data[1]);
            char msg_humi[40]; //메세지 길이 선언
            sprintf(msg_humi, "%d.%d", dht11_data[0], dht11_data[1]); //메세지에 담을 값을 넣어줌
            pubmsg_humi.payload = msg_humi;
            pubmsg_humi.payloadlen = (int)strlen(msg_humi);
            MQTTClient_publishMessage(client, "humi_hk", &pubmsg_humi, NULL); //humi_hk라는 Topic으로 메세지를 pub함
            delay(10000); //값들을 계속 보내면 과부화가 올 수 있어 delay를 줌
        }
        else {
            printf("Data not good, skip\n");
            delay(10000); //값들을 계속 보내면 과부화가 올 수 있어 delay를 줌
        }
    }
}

int main() {
    printf("Starting Raspberry Pi test program\n");

    pthread_t pthread[4]; //thread 5개 선언

    if (wiringPiSetup() == -1) //핀 셋업
        exit(1);

    if (wiringPiSPISetup() == -1) //핀 셋업
        exit(1);


    int rc; //MQTT connection 확인을 위한 변수

    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL); //연결할 MQTT 클라이언트 생성

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect by sound, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }


    pthread_create(&pthread[0], NULL, read_soil, NULL); //soil moisture thread 생성
    pthread_create(&pthread[1], NULL, read_light, NULL); //light thread 생성
    pthread_create(&pthread[2], NULL, read_dust, NULL); //dust thread 생성
    pthread_create(&pthread[3], NULL, read_dht11, NULL); //온습도 thread 생성
    

    //thread 종료를 기다림
    pthread_join(pthread[0], NULL);
    pthread_join(pthread[1], NULL);
    pthread_join(pthread[2], NULL);
    pthread_join(pthread[3], NULL);


    MQTTClient_disconnect(client, 10000); //MQTT연결 해제
    MQTTClient_destroy(&client); //MQTT연결 파괴 이쪽 코드 불릴일 없음

    printf("Finish!\n");
}
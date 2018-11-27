#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Ultrasonic.h> // Declaração de biblioteca


#define RST_PIN D3
#define SS_PIN D8

Ultrasonic ultrasonic(16, 5);
//#define trigPin 16
//#define echoPin 5

#define BOTtoken "764419886:AAGXK5IfzdyAuitZZSX061BeZP6WgoVsFlE"//Define o Token do *seu* BOT

Servo servoD2;

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);
// Inicializando a conexao WIFI com o Roteador
char ssid[] = "brisa-352909"; // nome do seu roteador WIFI (SSID)
char password[] = "eq2buvsh"; // senha do roteador WIFI

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

String id, text;//Váriaveis para armazenamento do ID e TEXTO gerado pelo Usuario
unsigned long tempo;

//int v = 0;
long duracao;
float distancia, aux;

void connect()//Funçao para Conectar ao wifi e verificar à conexao.
{
  WiFi.disconnect();            // desconecta o WIFI
  delay(100);                   // atraso de 100 milisegundos

  Serial.print("Conectando Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)   // aguardando a conexão WEB
  {
    Serial.print(".");
    delay(500);                           // atraso de 0,5 segundos
  }
  Serial.println("");
  Serial.println("WiFi conectado");       // WIFI conectado
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void readTel()//Funçao que faz a leitura do Telegram.
{
  int newmsg = bot.getUpdates(bot.last_message_received + 1);

  for (int i = 0; i < newmsg; i++)//Caso haja X mensagens novas, fara este loop X Vezes.
  {
    id = bot.messages[i].chat_id;//Armazenara o ID do Usuario à Váriavel.
    text = bot.messages[i].text;//Armazenara o TEXTO do Usuario à Váriavel.
    //text.toUpperCase();//Converte a STRING_TEXT inteiramente em Maiuscúla.

    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";

    if (text == "/start")                                  // comando comeca
    {
      String welcome = "Bem-vindo ao Estacionamento monitorado pelo Telegram Bot, " + from_name + ".\n";
      welcome += "Esse é um exemplo de controle de comandos.\n\n";
      welcome += "/vagas : para saber a quantidade de vagas\n";
      //welcome += "/ledoff : para apagar o LED\n";
      //welcome += "/status : mostra o estado do LED\n";
      bot.sendMessage(id, welcome, "Markdown");
    }

    else if (text == "/vagas")//Caso o texto recebido contenha "\vagas"
    {
      if (ultrasonic.distanceRead() <= 9)
      {
        bot.sendMessage(id, "Vaga numero 1: Ocupada", "");//Envia uma Mensagem para a pessoa que enviou o Comando.
      } else {
        bot.sendMessage(id, "Vaga numero 1: Livre", "");//Envia uma Mensagem para a pessoa que enviou o Comando.
      }
    }
    else//Caso o texto recebido nao for nenhum dos acima, Envia uma mensagem de erro.
    {
      bot.sendSimpleMessage(id, "Comando Invalido", "");
    }
  }
}

void RFID()// Funcao para Leitura de RFID
{
  // Procura por cartao RFID
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  // Seleciona o cartao RFID
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  //Mostra UID na serial
  Serial.print("UID da tag :");
  String conteudo = "";
  byte letra;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  conteudo.toUpperCase();
  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); //imprime os detalhes tecnicos do cartão/tag
  if ((conteudo.substring(1) == "EC A6 95 79") || (conteudo.substring(1) == "3F 8F D7 29")) //UID 1 - Cartao
  {
    Serial.println("Acesso liberado !");
    Serial.println();
    servoD2.write(90); // Angulaçao para o servo motor, abrir cancela
    delay(3000); // DELAY /espera 3 segundos
    servoD2.write(5); // Angulaçao para o servo motor, fechar cancela
  }
  else
  {
    Serial.println("Acesso negado !");
    Serial.println();
    //digitalWrite(D2, HIGH); // LIGA LED OU/ ativa rele, abre trava solenoide
    delay(3000); // DELAY /espera 3 segundos
    //digitalWrite(D2, LOW); // DESlIGA LED OU /desativa rele, fecha trava solenoide
  }
}

/*void sonic()
  {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(trigPin, LOW);
  }*/

void dist()
{
  /*  duracao = pulseIn(echoPin, HIGH, 1000000);
    distancia = (aux*340/10000)/2;
    Serial.print("Distancia:");
    Serial.print(distancia);
    Serial.print("");

    Serial.print("Duracao:");
    Serial.print(duracao);
    Serial.print("");*/

  Serial.print("Distancia: "); // Escreve texto na tela
  Serial.print(ultrasonic.distanceRead());// distância medida em cm
  Serial.println("cm"); // escreve texto na tela e pula uma linha
}

void setup()
{
  Serial.begin(9600); // Inicia a serial

  //sonic(); // Funçao de inicializaçao do sensor ultrassonico

  WiFi.mode(WIFI_STA);//Define o WiFi como Estaçao
  connect();//Funçao para Conectar ao WiFi

  servoD2.attach(4); // porta D1 para o servo
  servoD2.write(5); //posicao inicial

  SPI.begin(); // Inicia SPI bus
  mfrc522.PCD_Init(); // Inicia MFRC522
  Serial.println("Aproxime o seu cartao do leitor...");
  Serial.println();
}


void loop()
{
  if (millis() - tempo > 1000)//Faz a verificaçao das funçoes a cada 1 Segundos
  {
    ///  connect();//Funçao para verificar se ainda há conexao
    readTel();//Funçao para ler o telegram
    RFID();// Funçao para leitura RFID
    dist();
    tempo = millis();//Reseta o tempo
  }
}

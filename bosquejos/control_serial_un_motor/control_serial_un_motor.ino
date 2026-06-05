/* 
 * DEMO: Código prueba para un solo motor.
 * Mueve el motor según el comando serial recibido.
 * Este demo es para usar con el monitor serial, por eso el movimiento está acotado.
 * https://cursos.mcielectronics.cl/2023/02/23/como-funciona-el-encoder-rotatorio-y-la-interfaz-con-arduino/
 */

// Microsevomotor
// Verde: tierra
// Amarillo: 3.3 a 5V

const int A_CLK_PIN = 2;    // Azul
const int B_DT_PIN = 3;     // Blanco
const int IN1 = 8;
const int IN2 = 9;
const int ENA = 10;

int speed = 60;

const int NUM_VUELTAS = 1;
const int TICKS_POR_VUELTA = 7;
const int MAX_COUNT = NUM_VUELTAS * TICKS_POR_VUELTA;

const unsigned int MAX_MESSAGE_LENGTH = 64;

int counter;
int start_counter;    // Valor de counter al iniciar el movimiento
int currentStateCLK;
int lastStateCLK;

enum Direction {
  CCW = 1,
  STOP = 2,
  CW = 3
};

Direction direction;


String currentDir = "";

void setup() {
  // estado
  direction = STOP;
  counter = 0;

  // entradas
  pinMode(A_CLK_PIN, INPUT);
  pinMode(B_DT_PIN, INPUT);

  // dirección del motor
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  analogWrite(ENA, speed);

  Serial.begin(9600);

  // estado inicial del CLK
  lastStateCLK = digitalRead(A_CLK_PIN);

  attachInterrupt(0, updateEncoder, CHANGE);
	attachInterrupt(1, updateEncoder, CHANGE);
}

void loop() {
  // Call this constantly; it will return true only when a full line arrives
  if (readLineAndCheckComplete()) {
    // Your main loop code continues instantly without lagging
  }

  int diff = counter - start_counter;
  if(direction == CW && diff >= MAX_COUNT) {
    stop();
  } else if (direction == CCW && diff <= -MAX_COUNT) {
    stop();
  }
}

bool readLineAndCheckComplete() {
  static char message[MAX_MESSAGE_LENGTH];
  static unsigned int message_pos = 0;

  while (Serial.available() > 0) {
    char inByte = Serial.read();

    if (inByte != '\n' && inByte != '\r') {
      if (message_pos < MAX_MESSAGE_LENGTH - 1) {
        message[message_pos] = inByte;
        message_pos++;
      }
    } 
    else if (inByte == '\n') { // Line end flag hit
      message[message_pos] = '\0'; // Terminate string
      
      start_counter = counter;
      Serial.print("Requesting direction: ");
      Serial.print(message);
      Serial.print(" ... counter = ");
      Serial.println(start_counter);

      switch(message[0]) {
        case '1':
          moveCCW();
          break;
        case '2':
          stop();
          break;
        case '3':
          moveCW();
          break;
        default:
          Serial.println("  Other");
      }
      
      message_pos = 0; // Reset buffer index for next command
      return true;
    }
  }
  return false;
}

void updateEncoder(){
	// leer el estado actual del CLK
	currentStateCLK = digitalRead(A_CLK_PIN);

  // Si el ultimo estado actual del CLK es distinto, entonces ocurrió un pulso 
  // reacciona solo a un cambio de estado para evitar un conteo doble
	if (currentStateCLK != lastStateCLK && currentStateCLK == 1){

    // si el estado del DT es diferente que el estado del CLK 
    // entonces el encoder está rotando en sentido antihorario CCW asi que //decrementa		
    if (digitalRead(B_DT_PIN) != currentStateCLK) {
			counter --;
			currentDir ="CCW";
		} else {
      // Encoder está rotando en sentido horario CW así que incrementa
			counter ++;
			currentDir ="CW";
		}

		Serial.print("Direction: ");
		Serial.print(currentDir);
		Serial.print(" | Counter: ");
		Serial.print(counter);
    Serial.print(" | direction state = ");
    Serial.println(direction);
	}

	// guarda el ultimo estado del CLK 
	lastStateCLK = currentStateCLK;
}

void moveCW() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  direction = CW;
}

void moveCCW() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  direction = CCW;
}

void stop() {
  direction = STOP;
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
}
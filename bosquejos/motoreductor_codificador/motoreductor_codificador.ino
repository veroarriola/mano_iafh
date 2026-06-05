/* 
 * DEMO: Código prueba para un solo motor.
 * Al iniciar mueve el motor en direcciones ccw y cw alternando.
 * Ojo: hay un pequeño retraso entre la interrupción y la inversión del movimiento del dedo,
 *      esto provoca que el motor se vaya desplazando más en una dirección que en otra entre
 *      iteraciones.
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

int counter;
int currentStateCLK;
int lastStateCLK;
bool ccw;


String currentDir = "";

void setup() {
  // entradas
  pinMode(A_CLK_PIN, INPUT);
  pinMode(B_DT_PIN, INPUT);

  // dirección del motor
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  moveCCW();
  analogWrite(ENA, speed);

  Serial.begin(9600);

  // estado inicial del CLK
  lastStateCLK = digitalRead(A_CLK_PIN);

  attachInterrupt(0, updateEncoder, CHANGE);
	attachInterrupt(1, updateEncoder, CHANGE);
}

void loop() {
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
    Serial.print(" ccw =");
    Serial.println(ccw);

    // TODO: Ojo con los negativos
    if(counter >= MAX_COUNT) {
      if (!ccw) {
        stop();
        moveCCW();
      } else {
        Serial.println("ERROR: no es cw");
        stop();
      }
    } else if (counter <= -MAX_COUNT) {
      if (ccw) {
        stop();
        moveCW();
      } else {
        Serial.println("ERROR: no es ccw");
        stop();
      }
    }
	}

	// guarda el ultimo estado del CLK 
	lastStateCLK = currentStateCLK;
}

void moveCW() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  ccw = false;
  counter = 0;
}

void moveCCW() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  ccw = true;
  counter = 0;
}

void stop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  counter = 0;
}
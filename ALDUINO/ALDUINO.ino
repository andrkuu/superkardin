int light = 10;
long timeCount = 3;
long timeCounter;
long startTime;
//int inerval = 1000;
//int lightSensor;
//int lightButton = 0;
int motor = 9;
int in1 = 7;
int in2 = 6;
int esp1 = A3;
int esp2 = A4;
int esp3 = A5;
int button = 2;
int espValue = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(motor, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(esp1, INPUT);
  pinMode(esp2, INPUT);
  pinMode(esp3, INPUT);
}

void loop()
{
  readPin();
  controlMotor();
  //lightButtonfn();
  /*if(timeCount >= 1 && timeCount <= 5000){
    timeCounter = millis()/1000;
    if (espValue == 3){
      moveDown();
    }
    if (espValue == 2){
      moveUp();
    }
  }
  
  Serial.println(timeCount);
  Serial.println(espValue);
  if (espValue == 3 && timeCount <= 50000){
      timeCounter = millis()/1000;
      moveDown();
  } 
  if (espValue == 2 && timeCount >= 1){
      timeCounter = millis()/1000;
      moveUp();
  } 
  if (timeCount <= 1 || timeCount >= 50000 || espValue == 1){
    stopMotor();
  } */

/*
  lightSensor = analogRead(A1);
  if (lightButton == 1){
    if (lightSensor < light){
      moveDown();
    }
    if (lightSensor > light){
      moveUp();
    }
  }*/
}

void readPin(){
  esp1 = analogRead(A3);
  esp2 = analogRead(A4);
  esp3 = analogRead(A5);
  if(esp1 < 400){
    if(esp2 < 400){
      if(esp3 < 400){
        espValue = 1;
      } else if (esp3 > 400){
        espValue = 2;
        startTime = millis();
      }
    }else if (esp2 > 400){
      if(esp3 < 400){
        espValue = 3;
        startTime = millis();
      }else if(esp3 > 400){
        espValue = 4;
      }
    }
  } else if (esp1 > 400) {
    if(esp2 < 400){
      if(esp3 < 400){
        espValue = 5;
      } else if (esp3 > 400){
        espValue = 6;
      }
    }else if (esp2 > 400){
      if(esp3 < 400){
        espValue = 7;
      }else if(esp3 > 400){
        espValue = 8;
      }
    }
  }
}
/*
void lightButtonfn(){
  if (espValue == 4){
    lightButton = 1;
  }
  if (espValue == 5){
    lightButton = 0;
  }
}*/

void controlMotor(){
  Serial.println(timeCount);
  Serial.println(espValue);
  if (espValue == 3 && timeCount <= 20000){
      timeCounter = millis();
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
      timeCount = timeCount + (timeCounter - startTime);
      analogWrite(motor, 255);
  } 
  if (espValue == 2 && timeCount >= 1){
      timeCounter = millis();
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      timeCount = timeCount - (timeCounter - startTime);
      analogWrite(motor, 255);  
  } 
  if (timeCount <= 1 || timeCount >= 20000 || espValue == 1){
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
  }
}/*

void stopMotor(){
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
}

void moveUp(){
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  timeCount = timeCount - timeCounter;
  analogWrite(motor, 255);
}
void moveDown(){
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  timeCount = timeCount + timeCounter;
  analogWrite(motor, 255);  
}*/

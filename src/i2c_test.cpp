#include <Arduino.h>
#include <I2C.h>

#define I2C_ADDRESS 0x33U
uint8_t glBuf[]={0x11U,'B','C','D'};
uint8_t rdBuf[10];

void setup() {
  //setup I/O
  I2c.begin();

  Serial.begin(57600); // start serial for output and input
  while (!Serial);      // wait for serial monitor
  Serial.println("\nI2C Scanner, enter 1,2,3,4 or 5 for scan(w), scan(r), register read, register write, noregister read respectively");
  Serial.setTimeout(60000);  
}
void write2I2C(uint8_t address,uint8_t *buf,uint8_t bufamt){//only write without i2c restart 
  uint8_t error;
  if(bufamt==0U){// no/empty buffer - stop after sending slave address
    error=I2c.start();
    if(error==0U){
      error=I2c.sendAddress(SLA_W(address));
      if(error!=0U){
        Serial.print("I2C sendAddress error ");
        Serial.println(error,HEX);
      }else{
        Serial.println("I2C device found");
      }
    }else{
      Serial.print("I2C start error ");
      Serial.println(error,HEX);
    }
    I2c.stop();
  }else if(bufamt==1U){// single data byte write
    error=I2c.write(address,buf[0]);
    if(error!=0U){
      Serial.print(": I2C write error ");
      Serial.println(error,HEX);
    }else{
      Serial.println("one byte written");
    }
  }else{// multiple data bytes write
    error=I2c.write(address,buf[0],buf+1,bufamt-1);
    if(error!=0U){
      Serial.print(": I2C writebuf error ");
      Serial.println(error);
    }else{
      Serial.println("multiple bytes written");
    }
  }
}
void read4I2C(uint8_t address,uint8_t quantity,uint8_t reg){//only read, or read after i2c restart
  uint8_t bread;
  uint8_t error;
  if(reg==0xFFU&&quantity==0U){//just send slave address in read mode, fails on Uno side by keeping SDA LOW
    error=I2c.start();
    if(error==0U){
      error=I2c.sendAddress(SLA_R(address));
      if(error!=0U){
        Serial.print("I2C sendAddress error ");
        Serial.println(error,HEX);
      }else{
        Serial.println("I2C device found");
      }
    }else{
      Serial.print("I2C start error ");
      Serial.println(error,HEX);
    }
    error=I2c.stop();
    if(error!=0U){
        Serial.print("I2C stop error ");
        Serial.println(error,HEX);
    }
  }else if(reg==0xFFU){//do not send register, just read
    error=I2c.read(address,quantity);
    if(error!=0U){
      Serial.print("I2C read error ");
      Serial.println(error,HEX);
    }else{
      bread=I2c.available();
      Serial.print("read ");
      Serial.print(bread);
      Serial.print(" bytes:");
      while(I2c.available()>0)
        Serial.print(I2c.receive(),HEX);
      Serial.println();
    }
  }else{//send register and read value(s) after restart
    error = I2c.read(address, reg,quantity);
    if(error!=0){
      Serial.print("I2C read error ");
      Serial.println(error,HEX);
    }else{
      bread=I2c.available();
      Serial.print("read ");
      Serial.print(bread);
      Serial.print(" bytes:");
      while(I2c.available()>0)
        Serial.print(I2c.receive(),HEX);
      Serial.println();
    }
  }
}
void loop() {
  long op=Serial.parseInt(SKIP_WHITESPACE);
  switch (op){
    case 1:
      Serial.print("scan(w)");break;
    case 2:
      Serial.print("scan(r)");break;
    case 3:
      Serial.print("read reg");break;
    case 4:
      Serial.print("write reg");break;
    case 5:
      Serial.print("read status");break;
    default:
    return;
  }
  Serial.print(" in 3 ...");
  delay(1000);
  Serial.print("2 ...");
  delay(1000);
  Serial.print("1 ...");
  delay(1000);
  Serial.println(0);
  switch (op){
    case 1:
      write2I2C(0x33U,NULL,0U);break;
    case 2:
      read4I2C(0x33U,0U,0xFFU);break;
    case 3:
      read4I2C(0x33U,4U,0x11U);break;
    case 4:
      write2I2C(0x33U,glBuf,4U);break;
    case 5:
      read4I2C(0x33U,4U,0xFFU);break;
    default:
    return;
  }
}

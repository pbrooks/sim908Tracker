// Settings
char url[] = "http://apiendpoint";
char pin[] = "0000";
char apn[] = "*******";
char user_name[] = "*******";
char password[] = "*******";
char phone_number[] = "***********";

long loopDelay = 60000;

int8_t answer;
int onModulePin= 2;

char data[100];
int data_size;

char aux_str[100];
char aux;
int x = 0;

char frame[200];
char sms_frame[200];
char aux_string[30];

void setup()
{
		setup_module();	
		setup_cell();

		// Basic comms ready, tell the API via SMS
    send_SMS("WKE");
    
		setup_GPS();

		// XXX: Feature to be added
		//setup_apn();
}

void setup_module()
{
    pinMode(onModulePin, OUTPUT);
    Serial.begin(115200);   

    Serial.println("Starting...");
    power_on();

		// wait for the module to be ready
    delay(3000);
}

void power_on(){

    uint8_t answer=0;

    // checks if the module is started
    answer = sendATcommand("AT", "OK", 2000);
    if (answer == 0)
    {
        // power on pulse
        digitalWrite(onModulePin,HIGH);
        delay(3000);
        digitalWrite(onModulePin,LOW);

        // waits for an answer from the module
        while(answer == 0){  
            // Send AT every two seconds and wait for the answer   
            answer = sendATcommand("AT", "OK", 2000);    
        }
    }
}

void setup_cell()
{
    // sets the PIN code
    snprintf(aux_str, sizeof(aux_str), "AT+CPIN=%s", pin);
    sendATcommand(aux_str, "OK", 2000);

		// wait for the cell to be ready
    delay(3000);
}

void setup_GPS()
{
    // starts the GPS and waits for signal
    while ( start_GPS() == 0);

    while (sendATcommand("AT+CREG?", "+CREG: 0,1", 2000) == 0);
}

int8_t start_GPS(){

    unsigned long previous;

    previous = millis();
    // starts the GPS
    sendATcommand("AT+CGPSPWR=1", "OK", 2000);
    sendATcommand("AT+CGPSRST=0", "OK", 2000);

    // waits for fix GPS
    while(( (sendATcommand("AT+CGPSSTATUS?", "2D Fix", 5000) || 
        sendATcommand("AT+CGPSSTATUS?", "3D Fix", 5000)) == 0 ) && 
        ((millis() - previous) < 90000));

    if ((millis() - previous) < 90000)
    {
        return 1;
    }
    else
    {
        return 0;    
    }
}

void setup_apn()
{
    // sets APN , user name and password
    sendATcommand("AT+SAPBR=3,1,\"Contype\",\"GPRS\"", "OK", 2000);
    snprintf(aux_str, sizeof(aux_str), "AT+SAPBR=3,1,\"APN\",\"%s\"", apn);
    sendATcommand(aux_str, "OK", 2000);
    
    snprintf(aux_str, sizeof(aux_str), "AT+SAPBR=3,1,\"USER\",\"%s\"", user_name);
    sendATcommand(aux_str, "OK", 2000);
    
    snprintf(aux_str, sizeof(aux_str), "AT+SAPBR=3,1,\"PWD\",\"%s\"", password);
    sendATcommand(aux_str, "OK", 2000);

    // gets the GPRS bearer
    while (sendATcommand("AT+SAPBR=1,1", "OK", 20000) == 0)
    {
        delay(5000);
    }
}

void loop(){

    // gets GPS data
    get_GPS(0);
    sprintf(sms_frame, "GPS%s", frame);
    send_SMS(sms_frame);

		// Wait until the next iteration
    delay(loopDelay);
}

int8_t get_GPS(int mode){

    int8_t counter, answer;
    long previous;

    // First get the NMEA string
    // Clean the input buffer
    while( Serial.available() > 0) Serial.read(); 
    // request Basic string

    char command[13];

    sprintf(command, "AT+CGPSINF=%d", mode);
    
    sendATcommand(command, "AT+CGPSINF=0\r\n\r\n", 2000);

    counter = 0;
    answer = 0;
    memset(sms_frame, '\0', 100);
    memset(frame, '\0', 100);    // Initialize the string
    previous = millis();
    // this loop waits for the NMEA string
    do{

        if(Serial.available() != 0){    
            frame[counter] = Serial.read();
            counter++;
            // check if the desired answer is in the response of the module
            if (strstr(frame, "OK") != NULL)    
            {
                answer = 1;
            }
        }
        // Waits for the asnwer with time out
    }
    while((answer == 0) && ((millis() - previous) < 2000));  

    frame[counter-3] = '\0'; 
    return answer;
}

void send_SMS(char* sms_text){

    Serial.print("Setting SMS mode...");
    sendATcommand("AT+CMGF=1", "OK", 1000);    // sets the SMS mode to text
    Serial.println("Sending SMS");
    
    sprintf(aux_string,"AT+CMGS=\"%s\"", phone_number);
    answer = sendATcommand(aux_string, ">", 2000);    // send the SMS number
    if (answer == 1)
    {
        Serial.println(sms_text);
        Serial.write(0x1A);
        answer = sendATcommand("", "OK", 20000);
        if (answer == 1)
        {
            Serial.print("Sent ");    
        }
        else
        {
            Serial.print("error ");
        }
    }
    else
    {
        Serial.print("error ");
        Serial.println(answer, DEC);
    }
}

int8_t sendATcommand(char* ATcommand, char* expected_answer1, unsigned int timeout){

    uint8_t x=0,  answer=0;
    char response[100];
    unsigned long previous;

    memset(response, '\0', 100);    // Initialize the string

    delay(100);

    while( Serial.available() > 0) Serial.read();    // Clean the input buffer

    Serial.println(ATcommand);    // Send the AT command 

    x = 0;
    previous = millis();

    // this loop waits for the answer
    do{
        if(Serial.available() != 0){    
            response[x] = Serial.read();
            x++;
            // check if the desired answer is in the response of the module
            if (strstr(response, expected_answer1) != NULL)    
            {
                answer = 1;
            }
        }
        // Waits for the asnwer with time out
    }
    while((answer == 0) && ((millis() - previous) < timeout));    

    return answer;
}

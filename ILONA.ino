/*
Name:        replyKeyboard.ino
Created:     07/10/2019
Author:      Stefano Ledda <shurillu@tiscalinet.it>
Description: a simple example that do:
			 1) if a "show keyboard" text message is received, show the reply keyboard,
				otherwise reply the sender with "Try 'show keyboard'" message
			 2) if "Simple button" reply keyboard button is pressed, a "Simple button" message is sent
			 3) if "Contact request" reply keyboard button is pressed, a contact message is sent
			 4) if "Location request" reply keyboard is pressed, a location message is sent 
			 5) if "Hide replyKeyboard" inline keyboard button is pressed, a "Simple button" message is sent 
			    and the bot will hide the reply keyboard
*/
#include "CTBot.h"

CTBot myBot;
CTBotReplyKeyboard myKbd;   // reply keyboard object helper
bool isKeyboardActive;      // store if the reply keyboard is shown

String ssid = "Soerabajaplace_27";
String pass = "jagung27";
String token = "1399184950:AAFyeYOj741v6uiAqvGN33RiF2-AeCF2MR4";

void setup() {
  pinMode(2,OUTPUT);
  pinMode(16,OUTPUT);
	// initialize the Serial
	Serial.begin(115200);
	Serial.println("Starting TelegramBot...");
  digitalWrite(2,1);
  digitalWrite(16,1);

	// connect the ESP8266 to the desired access point
	myBot.wifiConnect(ssid, pass);

	// set the telegram bot token
	myBot.setTelegramToken(token);

	// check if all things are ok
	if (myBot.testConnection())
  {
		Serial.println("\ntestConnection OK");
    digitalWrite(2,0);
  }
	else
		{Serial.println("\ntestConnection NOK");}

  myKbd.addButton("LED ON");
  myKbd.addButton("LED OFF");
  myKbd.addRow();
  myKbd.addButton("Contact request", CTBotKeyboardButtonContact);
  myKbd.addButton("Location request", CTBotKeyboardButtonLocation);
  myKbd.addRow();
  
	myKbd.addButton("Hide ReplyKeyboard");
	// resize the keyboard to fit only the needed space
	myKbd.enableResize();
	isKeyboardActive = false;
}

void loop() {
	// a variable to store telegram message data
	TBMessage msg;

	// if there is an incoming message...
	if (myBot.getNewMessage(msg)) {
		// check what kind of message I received
		if (msg.messageType == CTBotMessageText) {
			// received a text message
			if (msg.text.equalsIgnoreCase("/showkeyboard")) {
				// the user is asking to show the reply keyboard --> show it
				myBot.sendMessage(msg.sender.id, "Reply Keyboard enable."
				"\nYou can control the leds, send your contact/your location or hide the Reply Keyboard", myKbd);
				isKeyboardActive = true;
			}
      else if (msg.text.equalsIgnoreCase("LED ON")) 
      {
        digitalWrite(16,0);myBot.sendMessage(msg.sender.id, "LED Enabled");
      }
      else if (msg.text.equalsIgnoreCase("LED OFF")) 
      {
        digitalWrite(16,1);myBot.sendMessage(msg.sender.id, "LED Disabled");
      }
			// check if the reply keyboard is active 
			else if (isKeyboardActive) {
				// is active -> manage the text messages sent by pressing the reply keyboard buttons
				if (msg.text.equalsIgnoreCase("Hide ReplyKeyboard")) {
					// sent the "hide keyboard" message --> hide the reply keyboard
					myBot.removeReplyKeyboard(msg.sender.id, "Reply Keyboard removed");
					isKeyboardActive = false;
				} else {
					// print every others messages received
					myBot.sendMessage(msg.sender.id, msg.text);
				}
			} else {
				// the user write anything else and the reply keyboard is not active --> show a hint message
				myBot.sendMessage(msg.sender.id, "Hi " + msg.sender.firstName + "."
				"\nTry /showkeyboard to show the Reply Keyboard Button.");
			}
		} else if (msg.messageType == CTBotMessageLocation) {
			// received a location message --> send a message with the location coordinates
			myBot.sendMessage(msg.sender.id, "Longitude: " + (String)msg.location.longitude +
				"\nLatitude: " + (String)msg.location.latitude);

		} else if (msg.messageType == CTBotMessageContact) {
			// received a contact message --> send a message with the contact information
			myBot.sendMessage(msg.sender.id, "Name: " + (String)msg.contact.firstName +
				"\nSurname: " + (String)msg.contact.lastName +
				"\nPhone: " + (String)msg.contact.phoneNumber +
				"\nID: " + (String)msg.contact.id +
				"\nvCard: " + (String)msg.contact.vCard);
		}
	}
	delay(100);
}

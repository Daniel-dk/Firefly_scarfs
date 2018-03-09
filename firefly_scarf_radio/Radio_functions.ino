
/*main loop radio handler*/
void handleRadio() {

	//  after send timeout, send sync
	// populate radiomessage_u radioMessage
	if (dataRecvd) {
		uLong_union macUnion;
		// (1) syncronise timer
		syncedMillis = 0;
		
		// (2) apply broadcaster's command

		// (3) apply broadcaster's gHue to our gHue
		blending = true;
		// (4) apply broadcaster unique addr to randomSeed
		macUnion.uLong_arr[0] = rxMACarr[5];
		macUnion.uLong_arr[1] = rxMACarr[4];
		macUnion.uLong_arr[2] = rxMACarr[3];
		macUnion.uLong_arr[3] = rxMACarr[2];

		randomSeed(macUnion.uLong);

		//reset timeout
		dataRecvd = false;
		syncTimeout = smillis() + syncTime;

		// **** only use lightning for the  beacon, not scarfs
		 lightning(2, 8, radioMessage_rx.OriginColour);

	}

	if (after(syncTimeout)) {
		// if we send a sync, set current Hue to our mac Addr
		gHue = stringToByte( WiFi.macAddress().charAt(15), WiFi.macAddress().charAt(16));
		currentPalette = CRGBPalette16(CHSV(gHue, 192, random8(128, 255)), CHSV(gHue + random8(8), 255, random8(128, 255)), CHSV(gHue, 192, random8(128, 255)), CHSV(gHue + random8(8), 255, random8(128, 255)));

		//Serial.print("Hue set to: ");
		//Serial.println(gHue);
		
		sendData();
		//lightning(0, 8, gHue);
		syncTimeout = smillis() + syncTime;
	
	}
}


unsigned char charToHexDigit(char c)
{
	if (c >= 'A')
		return c - 'A' + 10;
	else
		return c - '0';
}

unsigned char stringToByte(char c1, char c2)
{
	return charToHexDigit(c1) * 16 + charToHexDigit(c2);
}


// Init ESP Now with fallback
void InitESPNow() {
	if (esp_now_init() == ESP_OK) {
		Serial.println("ESPNow Init Success");
	}
	else {
		Serial.println("ESPNow Init Failed");
		// Retry InitESPNow, add a counte and then restart?
		// InitESPNow();
		// or Simply Restart
		ESP.restart();
	}
}



boolean initBroadcastPeer() {
	// clear slave data
	memset(&peer, 0, sizeof(peer));
	// set peer addr to ff:ff:ff:ff:ff:ff so we can broadcast packets
	for (int i = 0; i < 6; ++i) {
		peer.peer_addr[i] = (uint8_t)0xff;
	}
	peer.channel = CHANNEL; // pick a channel
	peer.encrypt = 0; // no encryption for broadcast

	const esp_now_peer_info_t *peerPtr = &peer;
	const uint8_t *peer_addrPtr = peer.peer_addr;

	bool exists = esp_now_is_peer_exist(peer_addrPtr);

	if (exists) {
		// Slave already paired.
		Serial.println("Already Paired");
		return true;
	}
	else {
		// Slave not paired, attempt pair
		esp_err_t addStatus = esp_now_add_peer(peerPtr);
		if (addStatus == ESP_OK) {
			// Pair success
			Serial.println("Pair success");
			return true;
		}
		else if (addStatus == ESP_ERR_ESPNOW_NOT_INIT) {
			// How did we get so far!!
			Serial.println("ESPNOW Not Init");
			return false;
		}
		else if (addStatus == ESP_ERR_ESPNOW_ARG) {
			Serial.println("Invalid Argument");
			return false;
		}
		else if (addStatus == ESP_ERR_ESPNOW_FULL) {
			Serial.println("Peer list full");
			return false;
		}
		else if (addStatus == ESP_ERR_ESPNOW_NO_MEM) {
			Serial.println("Out of memory");
			return false;
		}
		else if (addStatus == ESP_ERR_ESPNOW_EXIST) {
			Serial.println("Peer Exists");
			return true;
		}
		else {
			Serial.println("Not sure what happened");
			return false;
		}
	}

}



// send data
void sendData() {


	radioMessage_tx.MessageType = 0x42; // set to 0x42 so we don't accidentally accept other people's broadcasts
	radioMessage_tx.Command = broadcastCommand;
	radioMessage_tx.OriginMode = gPatternNumber;
	radioMessage_tx.OriginColour =  gHue;
	radioMessage_tx.Checksum = 0; // will be calculated

	for (int i = 0; i < (radioMessageStructSize - 1); i++) {
		radioMessage_tx.Checksum ^= radioMessage_tx.messageArr[i];
	}

	const uint8_t *peer_addrPtr = peer.peer_addr;

	Serial.print("sendData() :Sending: ");
	for (int i = 0; i < radioMessageStructSize; i++) {
		Serial.print(radioMessage_tx.messageArr[i]);
		Serial.print(" ");
	}
	Serial.println("");

	esp_err_t result = esp_now_send(peer_addrPtr, radioMessage_tx.messageArr, sizeof(radioMessage_tx.messageArr));

	Serial.print("Send Status: ");
	if (result == ESP_OK) {
		Serial.println("Success");
	}
	else if (result == ESP_ERR_ESPNOW_NOT_INIT) {
		// How did we get so far!!
		Serial.println("ESPNOW not Init.");
	}
	else if (result == ESP_ERR_ESPNOW_ARG) {
		Serial.println("Invalid Argument");
	}
	else if (result == ESP_ERR_ESPNOW_INTERNAL) {
		Serial.println("Internal Error");
	}
	else if (result == ESP_ERR_ESPNOW_NO_MEM) {
		Serial.println("ESP_ERR_ESPNOW_NO_MEM");
	}
	else if (result == ESP_ERR_ESPNOW_NOT_FOUND) {
		Serial.println("Peer not found.");
	}
	else {
		Serial.println("Not sure what happened");
	}
}

// callback when data is sent from Master to Slave
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
	//	char macStr[18];
	//	snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
	//		mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
	//	Serial.print("OnDataSent : Last Packet Sent to: "); Serial.println(macStr);
	Serial.print("OnDataSent : Last Packet Send Status: "); Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
	
	/*
	Serial.print ("sent Colour :" );
	Serial.print(radioMessage_tx.OriginColour);
	Serial.print(" (0x");
	Serial.print(radioMessage_tx.OriginColour,HEX);
	Serial.println(")");
	*/
}

// callback when data is recv from Master
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {

	//	char macStr[18];
	// get MAC addr of messgae sender
		snprintf(rxMACarr, sizeof(rxMACarr), "%02x:%02x:%02x:%02x:%02x:%02x",
			mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
	//	Serial.print("OnDataRecv : Last Packet Recv from: "); Serial.println(macStr);
	//	Serial.print("OnDataRecv : Last Packet Size: "); Serial.println(data_len);
	//	Serial.println("OnDataRecv : Last Packet Recv Data: ");

	if (data_len <= radioMessageStructSize) {
		for (int i = 0; i < data_len; i++) {
			Serial.print(data[i]);
			Serial.print(" ");
			radioMessage_rx.messageArr[i] = data[i];
		}
		Serial.println("");

		//	Serial.println("");
		uint8_t csum = 0;
		for (int i = 0; i < (radioMessageStructSize - 1); i++) {
			csum ^= radioMessage_rx.messageArr[i];
		}

		if (radioMessage_rx.Checksum == csum) {
		//			Serial.println("OnDataRecv : Checksum OK ");
			dataRecvd = true;
			Serial.print("received Colour :" );
			Serial.print(radioMessage_rx.OriginColour);
			Serial.print(" (0x");
			Serial.print(radioMessage_rx.OriginColour, HEX);
			Serial.println(")");
			
		}
	}
	
}


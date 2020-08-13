#include "serial_handling.h"
#include "map_drawing.h"

extern shared_vars shared;

bool stop = false;

/*
  Description : Sends the appropiate message to the server which includes,
                the 'R' char and start and end points, with correct formatting
  Param : start and end objects of type lon_lat_32

  Return : None
*/
void server_request(lon_lat_32 point1, lon_lat_32 point2) {
  Serial.flush();
  Serial.print("R");
  Serial.print(" ");
  Serial.print(point1.lat);
  Serial.print(" ");
  Serial.print(point1.lon);
  Serial.print(" ");
  Serial.print(point2.lat);
  Serial.print(" ");
  Serial.println(point2.lon);
  Serial.flush();
}

/*
  Description : The first stage of the program. The Arduino (client) sends
                the information about the start and end coordinates. The client
                waits for a max of 10 seconds for reply back from server or else
                the timeout is triggered.

  Param : start and end objects of type lon_lat_32, bool timeout

  Return : timeout or no timeout
*/
bool send_server_request(bool timeout, lon_lat_32 point1, lon_lat_32 point2) {
  uint32_t max_timeout = 10000;
  // A timeout feed is needed to ensure that once a 
  // server request is sent, within 10 seconds data
  // is sent from the server involving the number of
  // waypoints
  uint32_t start = millis();
  while (!timeout && !Serial.available()) {
    server_request(point1, point2);
    uint32_t end;
    while (!Serial.available()) {
      end = millis();
      if ((end - start) >= max_timeout) {
        timeout = true;
        break;
      }
    }
  }

  return timeout;
}

/*
  Description : This function allows us to read in upcoming data in the serial
                sent by the server. There is a timeout to ensure that if server
                takes too long then the client can restart the process

  Param : bool timeout

  Return : Output from Serial as string
*/
String read_from_serial(uint32_t timeout) {
  String temp_string;
  char temp_char;
  uint32_t start = millis();

  while (true) {
    if (Serial.available()) {
      temp_char = Serial.read();
      if (temp_char != '\n' && temp_char != ' ') {
        temp_string += temp_char;
      } else {
        return temp_string;
      }
    } else if (millis() - start > timeout) {
      return "";
    }
  }
}

/*
  Description : Once the client has read all of the waypoints, the server will send
                'E' character that marks the end of the transaction between the serverr
                and the client

  Param : bool timeout

  Return : timeout or no timeout
*/
bool get_E_char(bool timeout) {
  char temp_char;
  if (Serial.available() && !timeout) {
    temp_char = Serial.read();
    if (temp_char != 'E') {
      status_message("NOT CLEAR");
      delay(1000);
      timeout = true;
    }
  }

  return timeout;
}

/*
  Description : Main driver function which coordinates the retrieval of the waypoints.
                As a short summary, the client starts off by sending the server with request
                of path between start and end points. Client waits fot server to send the number
                of waypoints (n) between those points and also client acknowledges. Then the client 
                reads in waypoint n number of time, while sending acknowlegdment after each retrieval.
                Lastly, the client recieves the 'E' character and the communication between the server
                and client ends for that query.

  Param : start and end points from client.cpp

  Return : "1" if succesfully retrieved all waypoints
*/
uint8_t get_waypoints(const lon_lat_32& start, const lon_lat_32& end) {
  status_message("Receiving . . .");
  String temp_string = "";

  while (true) {
    bool timeout = false;

    // sends request to server
    timeout = send_server_request(timeout, start, end);
    
    // checks if data is in serial
    if (Serial.available() && !timeout) {
      char temp_char;
      String temp_string;
      // reads for character N
      temp_char = Serial.read();
      Serial.read();
      if (temp_char == 'N') {
        // read the number of waypoints as String and then
        // convert them into integer and store in shared
        // structure
        bool check1 = false;
        while (!check1) {
          if (Serial.available()) {
            temp_char = Serial.read();
            if (temp_char != '\n') {
              temp_string += temp_char;
            } else {
              check1 = true;
            }
          }
        }
        shared.num_waypoints = 0;
        int32_t num = temp_string.toInt();

        // if number of waypoints is 0 or less
        // then no path is displayed on the ardiuno
        if (num <= 0) {
          status_message("From?");
          timeout = true;
          break;
        } else {
          //stores into shared object
          shared.num_waypoints = num;
          // Send acknowledgement to server
          Serial.println("A");
        } 
      } else {
        timeout = true;
      }
    } 

    // Server starts sending waypoints while the client
    // recieves and ACK after each retrieval
    for (int i = 0; (i < shared.num_waypoints) && !timeout; i++) {
      String temp_string = read_from_serial(1000);
      if (temp_string == "") {
        timeout = true;
      } else if (temp_string == "W") {
        lon_lat_32 point;
        // gets the lat of the point
        point.lat = (int32_t)read_from_serial(1000).toInt();
        // gets the lon of the point
        point.lon = (int32_t)read_from_serial(1000).toInt();
        // sends acknowledgment
        shared.waypoints[i] = point;
        Serial.println("A");
        timeout = false;
      } else {
        timeout = true;
      }
    }

    timeout = get_E_char(timeout);
    
    // Usually does not happen
    if (stop) {
      stop = false;
      break;
    } 

    if (!timeout) {
      Serial.flush();
      continue;
    } else {
      Serial.flush();
      break;
    }
  }

  // return to client.cpp
  return 1;
}


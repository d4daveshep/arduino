
const float EarthRadius = 6371000.0; // radius of earth in m

// coords of start pin
const float pinlat = -36.8317;
const float pinlon = 174.7485;

// coords of start committee hut
const float commlat = -36.8355;
const float commlon = 174.7470;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);

  Serial.println("Test GPS Maths");

  // calculate start line
  float bearingPinToComm = bearingDeg(pinlat,pinlon,commlat,commlon);
  Serial.print("Bearing Pin to Comm = ");
  Serial.println(bearingPinToComm);
  
  float distPinToComm = distance(pinlat,pinlon,commlat,commlon);
  Serial.print("Dist Pin to Comm  = ");
  Serial.print(distPinToComm);
  Serial.println(" m");
//print("------")
  

}

void loop() {
  // put your main code here, to run repeatedly: 
  
}



// distance and bearing formulas from http://www.movable-type.co.uk/scripts/latlong.html

// distance between two points using Haversine formula 
float distance( float lat1, float lon1, float lat2, float lon2 ) { // lat, lon in degrees

  float L1 = radians(lat1);
  float L2 = radians(lat2);
  float dLat = radians(lat2-lat1);
  float dLon = radians(lon2-lon1);

  float a = sin(dLat/2.0) * sin(dLat/2.0) + cos(L1) * cos(L2) * sin(dLon/2.0) * sin(dLon/2.0);
  float c = 2.0 * atan2(sqrt(a), sqrt(1.0-a));

  float dist = EarthRadius * c;
  return dist;

}

// calculate bearing from point1 to point2 in degrees
float bearingDeg(float lat1, float lon1, float lat2, float lon2) { // lat, lon in degrees
  float L1 = radians(lat1);
  float L2 = radians(lat2);
  float dLon = radians(lon2-lon1);

  float y = sin(dLon)*cos(L2);
  float x = cos(L1)*sin(L2) - sin(L1)*cos(L2)*cos(dLon);
  float brng = atan2(y, x);
  float brngdeg = degrees(brng);
  brngdeg = brngdeg - 360.0 * floor(brngdeg/360.0);
  return brngdeg;
}
  

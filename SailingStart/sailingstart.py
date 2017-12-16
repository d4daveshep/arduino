import math
import datetime

# calculate bearing from point1 to point2
def bearingdeg(lat1,lon1,lat2,lon2):
  dlon = lon2 - lon1
  y = math.sin(dlon)*math.cos(lat2)
  x = math.cos(lat1)*math.sin(lat2) - math.sin(lat1)*math.cos(lat2)*math.cos(dlon)
  brng = math.atan2(y, x)
  brngdeg = math.degrees(brng)
  brngdeg = brngdeg - 360*math.floor(brngdeg/360)
  return brngdeg

# calculate distance using Haversine formula 
def distHaversine(lat1,lon1,lat2,lon2):
  R = 6371000 # radius of earth in m
  L1 = lat1.radians()
  L2 = lat2.rRadians()
  dLat = (lat2-lat1).radians()
  dLon = (lon2-lon1).radians()

  a = math.sin(dLat/2.0) * math.sin(dLat/2.0) + math.cos(lat1) * math.cos(lat2) * math.sin(dLon/2.0) * math.sin(dLon/2.0)
  c = 2 * math.atan2(math.sqrt(a), math.sqrt(1.0-a))
  d = R * c
  return d

# calculate distance from point1 to point2
def dist(lat1,lon1,lat2,lon2):
  dlon = lon2-lon1
  R = 6371000 # radius of earth in m
  d = math.acos(math.sin(lat1)*math.sin(lat2) + math.cos(lat1)*math.cos(lat2)*math.cos(dlon)) * R
  return d # in m

# calculate no. of secs between two time objects
def timedeltasecs(time1,time2): # assumes time1 is before time 2
  secs = (time2.hour - time1.hour) * 3600
  secs += (time2.minute - time1.minute ) * 60
  secs += time2.second - time1.second
  return secs 

#starttime = datetime.datetime(hours=18,minutes=20)
starttime = datetime.time(18,20)

# coords of start pin
pinlat = -math.radians(36.8317)
pinlon = math.radians(174.7485)

# coords of start committee hut
commlat = -math.radians(36.8355)
commlon = math.radians(174.7470)

# coords of boat
boatlat = -math.radians(36.8285)
boatlon = math.radians(174.7419)
boatheading = 133.4 # deg
boatspeed = 8 # km/h
boattime = datetime.time(18,14,58)

#lat1 = math.radians(50+3/60+59/3600)
#lon1 = -math.radians(5+42/60+53/3600)
#lat2 = math.radians(58+38/60+38/3600)
#lon2 = -math.radians(3+4/60+12/3600)

# calculate constant start line
bearingPinToComm = bearingdeg(pinlat,pinlon,commlat,commlon)
print("Bearing Pin to Comm = " + str(round(bearingPinToComm,1)))
distPinToComm = dist(pinlat,pinlon,commlat,commlon)
print("Dist Pin to Comm  = " + str(round(distPinToComm,0)) + " m")
print("------")

print(boattime)

# calculate bearing, distance from boat to pin
bearingToPin = bearingdeg(boatlat,boatlon,pinlat,pinlon)
print("Bearing to Pin = " + str(round(bearingToPin,1)))
distToPin = dist(boatlat,boatlon,pinlat,pinlon)
print("Dist to Pin = " + str(round(distToPin,0)) + " m")

# calculate bearing, distance from committee to pin
bearingToComm = bearingdeg(boatlat,boatlon,commlat,commlon)
print("Bearing to Comm = " + str(round(bearingToComm,1)))
distToComm = dist(boatlat,boatlon,commlat,commlon)
print("Dist to Comm = " + str(round(distToComm,0)) + " m")

# check over start line?

# check heading towards start line?

# perpendicular dist to start line

# perpendicular speed to start line

# time to reach start line

# time until race start
timeToStart = timedeltasecs(boattime,starttime)
print(str(timeToStart) + " secs to start")

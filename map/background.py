 # -*- coding: utf-8 -*-
import urllib2
import json
import urllib
import codecs
import math
import time

'''
{
	"纬度 id"：25755
	"经度 id"：25762
	"海拔 id"：25765
}

'''

id_serson = [25755,25762,25765]
userkey="a449390aebc645c09ee7bdf4be8852e1"


def last_data():
	all_sensor_url='http://www.lewei50.com/api/v1/user/getsensorswithgateway'
	data={
		'userkey':userkey
	}
	data = urllib.urlencode(data)
	request = urllib2.Request(all_sensor_url,data)
	request.add_header('accept-charset','utf-8')
	response = urllib2.urlopen(request)
	RCMS_URL_response= response.read()

	json_obj = json.loads(RCMS_URL_response)
	list_rcms = []

	for channels in json_obj:
	    CID = channels["sensors"]
	    list_rcms.append(CID)
	josn_rcms = json.dumps(list_rcms,ensure_ascii=False)

	list_c=[]

	js_r = json.loads(josn_rcms)
	js_h = js_r[1]	
	for c in js_h:
		Name = c["name"]
		Value= c["value"]
		lastUpdateTime = c["lastUpdateTime"]
		if not Name==u'海拔高度':
			list_c.append({"name":Name,"value":lat_lon_change(float(Value)),"lastUpdateTime":lastUpdateTime})
		else:
			list_c.append({"name":Name,"value":Value})
	with codecs.open('last_data.js','w','utf-8') as f:
		f.write("[")	
		for js_c in list_c:
			js_data = json.dumps(js_c,ensure_ascii=False)
			if not js_c == list_c[-1]:
				f.write(js_data+",")
			else:
				f.write(js_data)

		f.write("]")	
	print 'ok'



def history():
	history_url_lat = 'http://www.lewei50.com/api/V1/sensor/GetHistoryData/25755'
	history_url_lon = 'http://www.lewei50.com/api/V1/sensor/GetHistoryData/25762'
	history_url_alt = 'http://www.lewei50.com/api/V1/sensor/GetHistoryData/25765'		
	data={
	'userkey':userkey,
	'Order':0
	}

	#获取纬度历史数据
	data = urllib.urlencode(data)
	request = urllib2.Request(history_url_lat,data)
	request.add_header('accept-charset','utf-8')
	response = urllib2.urlopen(request)
	LAT_URL_response= response.read()
	ddate = json.loads(LAT_URL_response)
	data_array_lat=[] 
	data_array_lat=ddate['Data']
	#获取经度历史数据
	request = urllib2.Request(history_url_lon,data)
	request.add_header('accept-charset','utf-8')
	response = urllib2.urlopen(request)
	LAT_URL_response= response.read()
	ddate = json.loads(LAT_URL_response)
	data_array_lon=[] 
	data_array_lon=ddate['Data']
	#获取海拔历史数据
	request = urllib2.Request(history_url_alt,data)
	request.add_header('accept-charset','utf-8')
	response = urllib2.urlopen(request)
	LAT_URL_response= response.read()
	ddate = json.loads(LAT_URL_response)
	data_array_alt=[] 
	data_array_alt=ddate['Data']
	list_h=[]
	for po in range(0,len(data_array_alt)):
		lat_data =  data_array_lat[po]
		lon_data =  data_array_lon[po]
		alt_data =  data_array_alt[po]
		time = lat_data['updateTime']
		lat_position = lat_lon_change(float(lat_data['value']))
		lon_position = lat_lon_change(float(lon_data['value']))
		alt_position = alt_data['value']
		list_h.append({"time":time,"lat":lat_position,"lon":lon_position,"alt":alt_position})
	with codecs.open('history.js','w','utf-8') as f:
		f.write("[")	
		for js_c in list_h:
			js_data = json.dumps(js_c,ensure_ascii=False)
			if not js_c==list_h[-1]:
				f.write(js_data+',\r\n')
			else:
				f.write(js_data+'\r\n')
		f.write("]")	



def lat_lon_change(data_of_lat_or_lon):
	d_c_int = math.floor(data_of_lat_or_lon/100)
	d_c_floor = ((data_of_lat_or_lon) - d_c_int*100)/60
 	d_c_data = d_c_int+d_c_floor
 	return d_c_data

#	data_array = json.dumps(data_array,ensure_ascii=False)
#	js_c = json.dumps(data_array[1],ensure_ascii=False)
#	print js_c

if __name__ == '__main__':
	while 1:
		print u"正在更新数据。。。。。".encode("GBK")
		history()
		last_data()
		print u"等待中。。。。1分钟后刷新".encode("GBK")
		time.sleep(60)

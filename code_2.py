import serial
import csv
from time import gmtime, strftime
from datetime import datetime
data_to_append = [['Time','AC Current','Motor Temperature','Motor Controller Temperature','ERPM','Throttle','DC Current','Instantaneous Pack Voltage','State of charge','High Temperarture','Low Temperature','MFR (L/MIN)','MC FAULTS','BMS FAULTS']]
if __name__ == '__main__':
    ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
    ser.flush()
    file = open(r"/home/harsh/Downloads/datalog/logfile.csv", 'a', newline='')
    writer = csv.writer(file)
    writer.writerows(data_to_append)
    file.close()

    while True:

        if ser.in_waiting > 0:
            data = str(ser.readline())
            datanew = ""
            startvar = 0
            endvar = 0
            file = open(r"/home/harsh/Downloads/datalog/logfile.csv", 'a', newline='')
            # data_to_append = [['Time','AC Current','Motor Temperature','Motor Controller Temperature','ERPM','Throttle','DC Current','Instantaneous Pack Voltage','State of charge','High Temperarture','Low Temperature','MFR (L/MIN)','MC FAULTS','BMS FAULTS']]
            writer = csv.writer(file)
            #writer.writerows(data_to_append)
            data_to_append = []
            for i in data:
                if i == "'":
                    startvar = startvar+1
                elif i == '!' or i == '@':
                    endvar = endvar+1
                elif startvar == 1 and endvar != 1:
                    datanew = datanew+i
                
            datalist0 = datanew.split(",")
            #print(datalist0)
            if len(datalist0) == 12:
                datalist1 = datalist0[11].split(";")
                finaldatalist = [datetime.now().strftime('%Y-%m-%d %H:%M:%S')]
                for i in range(11):
                    finaldatalist.append(datalist0[i])
                finaldatalist.append(datalist1[0])
                finaldatalist.append(datalist1[1])
                data_to_append.append(finaldatalist)
                
                writer.writerows(data_to_append)
            file.close()

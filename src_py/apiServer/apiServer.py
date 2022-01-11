from multiprocessing import Process
from transmitter import Transmitter
import globalVars as globe
from receiver import *
import time
import requests
import threading

class ApiServer():
    def __init__(self): 

        mainServerIP = globe.map.mainServerIp
        mainServerPort = globe.map.mainServerPort
        self.mainServerAddress = 'http://' + mainServerIP + ':' + mainServerPort
        
        # Starting receiver flask server process
        self.serverThread = threading.Thread(target=initReceiver, args=())
        self.serverThread.start()
        time.sleep(1)

        self.transmitter = Transmitter(self.mainServerAddress)

    def getTransmitter(self):
        return self.transmitter

    def stopServer(self):
        receiver.stop()
        return True

    def getQueueData(self):
        received = False
        
        while not received:
            if not multiProcQueue.empty():
                print("New loss map created!")
                multiProcQueue.get()
                received = True
            time.sleep(0.1)
   
    def train(self):
        globe.lossMaps.append({})
        self.transmitter.train()
        self.getQueueData()
        print(globe.lossMaps[-1])
        return globe.lossMaps[-1]

    def predict(self):
        globe.lossMaps.append({})
        self.transmitter.predict()
        self.getQueueData()
        print(globe.lossMaps[-1])
        return globe.lossMaps[-1]
    
    def statistics(self):
        self.transmitter.statistics()

if __name__ == "__main__":
    apiServerInst = ApiServer()
    apiServerInst.train()
    apiServerInst.predict()
    apiServerInst.statistics()
    #transmitterInst = apiServerInst.getTransmitter()
    #transmitterInst.testPost()

'''
 def exitHandler(self):
        print("\nServer shutting down")
        exitReq = requests.get(self.mainServerAddress + '/shutdown')
        if exitReq.ok:
            exit(0)
        else:
            print("Server shutdown failed")
            exit(1)
'''
'''
        #serverState = None

        #while(serverState != SERVER_DONE):
         #  if not serverState:
          #    if it is integer: 
           #         serverState = self.managerQueue.get()

                #TODO add timeout mechanism (if mainServer falls kill after X seconds)
            #sleep(5)
        # wait on Queue
        #return ack.json() 
'''
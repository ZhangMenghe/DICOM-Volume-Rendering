import numpy as np 
import cv2

with open("mchessboard2",'rb') as f:
    data = f.read()
    arr = np.frombuffer(data, dtype=np.uint8)
    data_len = 480*640
    n = int(len(arr) / data_len)
    for i in range(n):
        img = np.reshape(arr[data_len*i:data_len*(i+1)], (480,640))
        cv2.imwrite("img_"+str(i+12)+".png", img)
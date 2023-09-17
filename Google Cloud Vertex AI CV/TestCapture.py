import os
import cv2
import math
import json
import numpy as np
from google.cloud import vision

folder = 'output-labeled'
os.mkdir(folder)

cap = cv2.VideoCapture('resources/CarFootage.mp4')#(0)

# Check if the webcam is opened correctly
if not cap.isOpened():
    raise IOError("Cannot access video")

client = vision.ImageAnnotatorClient()

count = 0

while True:
    count += 1
    success, frame = cap.read()
    if not success: break
    # Scale down frame for faster processing
    #frame = cv2.resize(frame, None, fx=0.3, fy=0.3, interpolation=cv2.INTER_AREA)

    image4google = vision.Image()
    _, jpeg = cv2.imencode('.jpeg', frame)
    content = jpeg.tobytes()
    image4google = vision.Image(content=content)
    
    objects = client.object_localization(
        image=image4google).localized_object_annotations
    print('Number of objects found: {}'.format(len(objects)))

    resY, resX, cDepth = frame.shape
    with open(os.path.join(folder,"frame{:d}.txt".format(count)), "w") as outfile:
        dictionary = []
        for object_ in objects:
            if not (object_.name in ["Car", "Truck", "Van", "Bicycle"]):
                print("Skip")
                continue
            verticies = object_.bounding_poly.normalized_vertices
            start_point = (int(verticies[0].x*resX), int(verticies[0].y*resY))
            end_point = (int(verticies[2].x*resX), int(verticies[2].y*resY))
            cv2.rectangle(frame, start_point, end_point, (0, 0, 255), 3)
            dictionary.append({
                "name": object_.name,
                "x1": int(verticies[0].x*resX),
                "y1": int(verticies[0].y*resY),
                "x2": int(verticies[1].x*resX),
                "y2": int(verticies[1].y*resY),
                "x3": int(verticies[2].x*resX),
                "y3": int(verticies[2].y*resY),
                "x4": int(verticies[3].x*resX),
                "y4": int(verticies[3].y*resY),
            })
        json.dump(dictionary, outfile)
        cv2.imshow('Input', frame)
        cv2.imwrite(os.path.join(folder,"frame{:d}.jpg".format(count)), frame)     # save frame as JPEG file

    c = cv2.waitKey(1)
    if c == 27:
        break

cap.release()
cv2.destroyAllWindows()
#!/usr/bin/env python

import io
import os

from google.cloud import vision

import math
from PIL import Image, ImageDraw, ImageFont

def localize_objects(path):
    client = vision.ImageAnnotatorClient()

    image = vision.Image()
    if os.path.exists(path):
        with open(path, 'rb') as image_file:
            content = image_file.read()
        image = vision.Image(content=content)
    else:
        image.source.image_uri = path

    objects = client.object_localization(
        image=image).localized_object_annotations

    print('Number of objects found: {}'.format(len(objects)))
    with Image.open(path) as im:
        draw = ImageDraw.Draw(im)
        [resX, resY] = im.size
        for object_ in objects:
            if not (object_.name in ["Car", "Truck", "Van", "Bicycle"]):
                print("Skip")
                continue
            print('\n{} (confidence: {})'.format(object_.name, object_.score))
            print('Normalized bounding polygon vertices: ')
            verticies = object_.bounding_poly.normalized_vertices
            #for vertex in verticies:
            #    print(' - ({}, {})'.format(vertex.x, vertex.y))
            #draw.line((0, 0) + im.size, fill=128, width=10)
            #draw.line((0, im.size[1], im.size[0], 0), fill=128, width=10)
            #shape = [(0, 0), (100, 100)]
            #draw.rectangle(shape, outline ="red", width=10)
            shape = [(verticies[0].x*resX, verticies[0].y*resY), (verticies[2].x*resX, verticies[2].y*resY)]
            #print(shape)
            draw.rectangle(shape, outline ="red", width=10)
            font = ImageFont.truetype("resources/arial.ttf", 10)
            #draw.text((verticies[0].x, verticies[0].y), font=font, text=object_.name)
            draw.text((verticies[0].x*resX, verticies[0].y*resY), font=font, text=object_.name, fill='#FFFF00')
        im.show()
        # write to stdout
        im.save("output.png", "PNG")


if __name__ == '__main__':

    # Instantiates a client
    # [START vision_python_migration_client]
    client = vision.ImageAnnotatorClient()
    # [END vision_python_migration_client]

    # The name of the image file to annotate
    #file_name = os.path.abspath('resources/wakeupcat.jpg')
    #file_name = os.path.abspath('resources/Park.jpg')
    #file_name = "resources/fruit.jpg"
    #file_name = "resources/Park.jpg"
    #file_name = "resources/webcam.jpg"
    file_name = "resources/frame5769.jpg"

    localize_objects(file_name)
    '''

    # Loads the image into memory
    with io.open(file_name, 'rb') as image_file:
        content = image_file.read()

    image = vision.Image(content=content)

    # Performs label detection on the image file
    response = client.label_detection(image=image)
    labels = response.label_annotations

    print('Labels:')
    for label in labels:
        print(label.description)
    # [END vision_quickstart]
    '''
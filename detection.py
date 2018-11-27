import cv2
import database
from datetime import datetime
import os.path
# captura de video na USB 1
cap = cv2.VideoCapture(1)


# Classificadores em cascata
car_cascade = cv2.CascadeClassifier('cars.xml')

i = 1


#loop de video
while True:
    # captura cada frame com fps reduzido
    ret, frames = cap.read()
    k = cv2.waitKey(30)
    # conversão em escala de cinza
    gray = cv2.cvtColor(frames, cv2.COLOR_BGR2GRAY)

    # detecta carros através de diferentes parâmetros de escala para cada 'subimagem' detectada e seus valores semelhantes
    '''cars = car_cascade.detectMultiScale(gray, 1.2, 1)'''
    '''cars = car_cascade.detectMultiScale(gray, 1.08, 1)'''
    cars = car_cascade.detectMultiScale(gray, 1.15, 1)

    # desenha um retangulo na imagem
    for (x, y, l, a) in cars:
        altura = y+a
        largura = x+l

        print(str(largura)+', '+str(altura))
        detectedCars = cv2.resize(frames[y:y+a,x:x+l], (220,220))

        # caso a largura do objeto detectado seja maior do que 450 grava uma imagem e exibe
        if (largura > 450):
            cv2.imshow('Detected Cars', detectedCars)
            #salva em um repositorio (ou diretorio a ler lido pelo apache2)
            cv2.imwrite('/home/tallys/PycharmProjects/CarDetection/carros/car' + str(i) + '.jpg', detectedCars)
            cv2.imwrite('/home/tallys/PycharmProjects/CarDetection/carros/carf' + str(i) + '.jpg', frames)
            if(os.path.isfile('car'+str(i)+'.jpg')):
                i = i + 1
                cv2.imwrite('/home/tallys/PycharmProjects/CarDetection/carros/car'+str(i)+'.jpg',detectedCars)
                cv2.imwrite('/home/tallys/PycharmProjects/CarDetection/carros/carf'+str(i)+'.jpg',frames)
                cv2.imshow('Frame',frames)
                database.insert(str(datetime.now), frames)

        cv2.rectangle(frames, (x, y), (x + l, y + a), (0, 0, 255), 2)


    cv2.imshow('video2', frames)

# pressionar tecla esc (27) e esperar 0ms para sair
    if cv2.waitKey(0) == 27:
        break

# limpar
cv2.destroyAllWindows() 